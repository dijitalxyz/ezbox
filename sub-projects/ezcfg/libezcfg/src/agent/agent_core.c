/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : agent/agent_core.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2013-07-29   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>
#include <signal.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 0
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *dbg_fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	dbg_fp = fopen(path, "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, "tid=[%d] ", (int)gettid()); \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

/*
 * ezcfg_agent:
 *
 * Opaque object handling one event source.
 * Multi-Agents System model - agent part.
 */
struct ezcfg_agent {
	struct ezcfg *ezcfg;
	char *name;
	int state;
	struct ezcfg_socket *listening_sockets;
	int sq_len; /* Length of the socket queue */
	struct ezcfg_nvram *nvram; /* Name-Value random access memory */
};

/* Private functions */
/*
 * Deallocate ezcfg agent context, free up the resources
 * only delete agent_new() allocated resources before pthread_mutex initialized
 * other resources should be deleted in agent_finish()
 */
static void agent_delete(struct ezcfg_agent *agent)
{
	if (agent == NULL)
		return;

	if (agent->name != NULL) {
		free(agent->name);
	}

	if (agent->nvram != NULL) {
		ezcfg_nvram_delete(agent->nvram);
	}

	free(agent);
}

/*
 * Deallocate ezcfg agent context, free up the resources
 * when agent_new() success, this function will be called before agent_delete()
 */
static void agent_finish(struct ezcfg_agent *agent)
{
	/* Close all listening sockets */
	if (agent->listening_sockets != NULL) {
		ezcfg_socket_list_delete(&(agent->listening_sockets));
		agent->listening_sockets = NULL;
	}

        /* signal environment agent that we're done */
        agent->state = AGENT_STATE_STOPPED;
}

/**
 * agent_new:
 *
 * Create ezcfg agent.
 *
 * Returns: a new ezcfg agent
 **/
static struct ezcfg_agent *agent_new(struct ezcfg *ezcfg)
{
	struct ezcfg_agent *agent;

	ASSERT(ezcfg != NULL);

	agent = calloc(1, sizeof(struct ezcfg_agent));
	if (agent == NULL) {
		err(ezcfg, "calloc ezcfg_agent fail: %m\n");
		return NULL;
	}

	/* initialize ezcfg library context */
	memset(agent, 0, sizeof(struct ezcfg_agent));

	/* set ezcfg library context */
	agent->ezcfg = ezcfg;

	/* get nvram memory */
	agent->nvram = ezcfg_nvram_new(ezcfg);
	if(agent->nvram == NULL) {
		err(ezcfg, "agent alloc nvram fail: %m\n");
		goto fail_exit;
	}

	/* initialize nvram */
	ezcfg_nvram_fill_storage_info(agent->nvram, ezcfg_common_get_config_file(ezcfg));
	if (ezcfg_nvram_initialize(agent->nvram) == false) {
		err(ezcfg, "agent init nvram fail: %m\n");
		goto fail_exit;
	}

	/* initialize socket queue */
        agent->sq_len = AGENT_SOCKET_QUEUE_LENGTH;

	/* Successfully create agent */
	return agent;

fail_exit:
	agent_delete(agent);
	return NULL;
}

/**
 * agent_add_socket:
 * @agent: ezcfg agent context
 * @socket_path: unix socket path
 *
 * Add a specified socket to the agent. The
 * path to a socket either points to an existing socket file, or if
 * the socket path starts with a '@' character, an abstract namespace
 * socket will be used.
 *
 * A socket file will not be created. If it does not already exist,
 * it will fall-back and connect to an abstract namespace socket with
 * the given path. The permissions adjustment of a socket file, as
 * well as the later cleanup, needs to be done by the caller.
 *
 * Returns: socket, or NULL, in case of an error
 **/
static struct ezcfg_socket *agent_add_socket(struct ezcfg_agent *agent, int family, int type, int proto, const char *socket_path)
{
	struct ezcfg_socket *listener;
	struct ezcfg *ezcfg;

	ASSERT(agent != NULL);
	ezcfg = agent->ezcfg;

	ASSERT(socket_path != NULL);

	/* initialize unix domain socket */
	listener = ezcfg_socket_new(ezcfg, family, type, proto, socket_path);
	if (listener == NULL) {
		err(ezcfg, "init socket fail: %m\n");
		return NULL;
	}

	if ((family == AF_LOCAL) &&
	    (socket_path[0] != '@')) {
		ezcfg_socket_set_need_unlink(listener, true);
	}

	if (ezcfg_socket_list_insert(&(agent->listening_sockets), listener) < 0) {
		err(ezcfg, "insert listener socket fail: %m\n");
		ezcfg_socket_delete(listener);
		listener = NULL;
	}

	return listener;
}

/**
 * agent_new_from_socket:
 * @ezcfg: ezcfg library context
 * @socket_path: unix socket path
 *
 * Create new ezcfg agent and connect to a specified socket. The
 * path to a socket either points to an existing socket file, or if
 * the socket path starts with a '@' character, an abstract namespace
 * socket will be used.
 *
 * A socket file will not be created. If it does not already exist,
 * it will fall-back and connect to an abstract namespace socket with
 * the given path. The permissions adjustment of a socket file, as
 * well as the later cleanup, needs to be done by the caller.
 *
 * Returns: a new ezcfg agent, or #NULL, in case of an error
 **/
static struct ezcfg_agent *agent_new_from_socket(struct ezcfg *ezcfg, const char *socket_path)
{
	struct ezcfg_agent *agent = NULL;
	struct ezcfg_socket *sp = NULL;

	ASSERT(ezcfg != NULL);
	ASSERT(socket_path != NULL);

	agent = agent_new(ezcfg);
	if (agent == NULL) {
		err(ezcfg, "new agent fail: %m\n");
		return NULL;
	}

	sp = agent_add_socket(agent, AF_LOCAL, SOCK_STREAM, EZCFG_PROTO_CTRL, socket_path);
	if (sp == NULL) {
		err(ezcfg, "add socket [%s] fail: %m\n", socket_path);
		goto fail_exit;
	}

	if (ezcfg_socket_enable_receiving(sp) < 0) {
		err(ezcfg, "enable socket [%s] receiving fail: %m\n", socket_path);
		ezcfg_socket_list_delete_socket(&(agent->listening_sockets), sp);
		goto fail_exit;
	}

	if (ezcfg_socket_enable_listening(sp, agent->sq_len) < 0) {
		err(ezcfg, "enable socket [%s] listening fail: %m\n", socket_path);
		ezcfg_socket_list_delete_socket(&(agent->listening_sockets), sp);
		goto fail_exit;
	}

	ezcfg_socket_set_close_on_exec(sp);

	return agent;

fail_exit:
	/* first clean up all resources in agent */
	agent_finish(agent);
	/* don't delete sp, agent_delete will do it! */
	agent_delete(agent);
	return NULL;
}

static void agent_update_inner_state(struct ezcfg_agent *agent)
{
	/* do nothing */
	return;
}

static void agent_think_about_changes(struct ezcfg_agent *agent)
{
	/* do nothing */
	return;
}

static void agent_perform_action(struct ezcfg_agent *agent)
{
	/* do nothing */
	return;
}

/* Public functions */
struct ezcfg_agent *ezcfg_agent_start(struct ezcfg *ezcfg)
{
	struct ezcfg_agent *agent = NULL;
	sigset_t sigset;

	ASSERT(ezcfg != NULL);

	/* There must be an agent control socket */
	agent = agent_new_from_socket(ezcfg, ezcfg_common_get_sock_agent_ctrl_path(ezcfg));
	if (agent == NULL) {
		err(ezcfg, "can not initialize agent control socket");
		goto start_out;
	}

	/* run main loop forever */
	while (agent->state == AGENT_STATE_RUNNING) {
		sigsuspend(&sigset);
		/* first update inner state */
		agent_update_inner_state(agent);
		/* then thinks about the inner state changs */
		agent_think_about_changes(agent);
		/* perform actions */
		agent_perform_action(agent);
	}

start_out:
	/* Start agent */
	if (agent != NULL) {
		agent_delete(agent);
		agent = NULL;
	}
	return agent;
}
