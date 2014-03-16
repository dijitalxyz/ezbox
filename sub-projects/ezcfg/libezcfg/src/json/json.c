/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : json/json.c
 *
 * Description  : simple implementation of RFC7159
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-03   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

/*
object
    {}
    { members } 
members
    pair
    pair , members
pair
    string : value
array
    []
    [ elements ]
elements
    value
    value , elements
value
    string
    number
    object
    array
    true
    false
    null
*/

struct ezcfg_json_value {
	int type; /* false / null / true / object / array / number / string */
	char *element;
	struct ezcfg_json_value *value;
	struct ezcfg_json_value *sibling;
	int visited; /* used for stack operation */
};

struct json_stack_node {
	struct json_stack_node *next;
	struct ezcfg_json_value *elem;
};

struct ezcfg_json {
	struct ezcfg *ezcfg;
	struct ezcfg_json_value text; /* JSON-text = object / array */
	struct json_stack_node stack;
	int msg_len;
};

/* Private functions */
/*
RFC7159
      ws = *(
                %x20 /              ; Space
                %x09 /              ; Horizontal tab
                %x0A /              ; Line feed or New line
                %x0D                ; Carriage return
            )
*/
static char *skip_ws(char *buf)
{
	char *p = buf;
	while ((*p == 0x20) || (*p == 0x09) ||
	       (*p == 0x0a) || (*p == 0x0d))
		p++;
	return p;
}

static char *find_ws(char *buf)
{
	char *p = buf;
	while ((*p != 0x20) && (*p != 0x09) &&
	       (*p != 0x0a) && (*p != 0x0d) &&
	       (*p != '\0'))
		p++;

	if (*p == '\0')
		return NULL;
	else 
		return p;
}

#if 0
static bool is_number(char *buf)
{
	char *p = buf;
	if (*p == '-') p++;
	if ((*p < '0') || (*p > '9'))
		goto fail_out;

	/* int */
	if (*p == '0') {
		p++;
		if (*p == '\0')
			return true;
		if (*p == '.')
			goto frac;
		else if ((*p == 'e') || (*p == 'E'))
			goto exp;
		else
			goto fail_out;
	}
	else {
		while ((*p >= '0') && (*p <= '9')) p++;
		if (*p == '\0')
			return true;
		if (*p == '.')
			goto frac;
		else if ((*p == 'e') || (*p == 'E'))
			goto exp;
		else
			goto fail_out;
	}

frac:
	/* frac */
	p++;
	while ((*p >= '0') && (*p <= '9')) p++;
	if (*p == '\0')
		return true;

	if ((*p != 'e') && (*p != 'E'))
		goto fail_out;

exp:
	/* exp */
	p++;
	if ((*p == '+') || (*p == '-')) p++;
	while ((*p >= '0') && (*p <= '9')) p++;
	if (*p == '\0')
		return true;

fail_out:
	return false;
}
#endif

static char * get_number(char *buf)
{
	char *p = buf;
	if (*p == '-') p++;
	if ((*p < '0') || (*p > '9'))
		return NULL;

	/* int */
	if (*p == '0') {
		p++;
		if (*p == '\0')
			return p;
		if (*p == '.')
			goto frac;
		else if ((*p == 'e') || (*p == 'E'))
			goto exp;
		else
			return p;
	}
	else {
		while ((*p >= '0') && (*p <= '9')) p++;
		if (*p == '\0')
			return p;
		if (*p == '.')
			goto frac;
		else if ((*p == 'e') || (*p == 'E'))
			goto exp;
		else
			return p;
	}

frac:
	/* frac */
	p++;
	if ((*p < '0') || (*p > '9'))
		return (p-1);
	while ((*p >= '0') && (*p <= '9')) p++;
	if (*p == '\0')
		return p;

	if ((*p != 'e') && (*p != 'E'))
		return p;

exp:
	/* exp */
	p++;
	if ((*p != '+') && (*p != '-') && ((*p < '0') || (*p > '9')))
		return (p-1);
	if ((*p == '+') || (*p == '-')) p++;
	while ((*p >= '0') && (*p <= '9')) p++;

	return p;
}

static bool stack_push(struct json_stack_node *sp, struct ezcfg_json_value *vp)
{
	struct json_stack_node *n;

	n = calloc(1, sizeof(struct json_stack_node));
	if (n == NULL) {
		return false;
	}
	n->elem = vp;
	n->next = sp->next;
	sp->next = n;
	return true;
}

static struct ezcfg_json_value *stack_pop(struct json_stack_node *sp)
{
	struct json_stack_node *n;
	struct ezcfg_json_value *vp;

	if (sp->next == NULL) {
		return NULL;
	}
	n = sp->next;
	sp->next = n->next;
	vp = n->elem;
	free(n);
	return vp;
}

static bool stack_is_empty(struct json_stack_node *sp)
{
	if (sp->next == NULL)
		return true;
	else
		return false;
}

static bool stack_clean_up(struct json_stack_node *sp, struct ezcfg_json_value *skip_vp)
{
	struct ezcfg_json_value *vp = NULL;
	bool ret = false;

	while (stack_is_empty(sp) == false) {
		vp = stack_pop(sp);
		if (vp == NULL) {
			continue;
		}
		if (vp->sibling) {
			stack_push(sp, vp->sibling);
			vp->sibling = NULL;
		}
		if (vp->value) {
			stack_push(sp, vp->value);
			vp->value = NULL;
		}
		/* delete vp */
		if (vp->element) {
			free(vp->element);
			vp->element = NULL;
		}
		if (vp == skip_vp) {
			printf("%s(%d) it's skip_vp!\n", __func__, __LINE__);
		}
		else {
			free(vp);
		}
	}

	ret = true;

	return ret;
}

static struct ezcfg_json_value *json_value_new(int type)
{
	struct ezcfg_json_value *vp;

	vp = calloc(1, sizeof(struct ezcfg_json_value));
	if (vp) {
		vp->type = type;
	}
	return vp;
}

static struct ezcfg_json_value *json_value_create(char *buf)
{
	if (buf == NULL) {
		return NULL;
	}

	if (*buf == '"') {
		return json_value_new(EZCFG_JSON_VALUE_TYPE_STRING);
	}
	else if (*buf == '{') {
		return json_value_new(EZCFG_JSON_VALUE_TYPE_OBJECT);
	}
	else if (*buf == '[') {
		return json_value_new(EZCFG_JSON_VALUE_TYPE_ARRAY);
	}
	else if ((strncmp(buf, "false", 5) == 0) &&
		 (find_ws(buf) == buf+5)) {
		return json_value_new(EZCFG_JSON_VALUE_TYPE_FALSE);
	}
	else if ((strncmp(buf, "null", 4) == 0) &&
		 (find_ws(buf) == buf+4)) {
		return json_value_new(EZCFG_JSON_VALUE_TYPE_NULL);
	}
	else if ((strncmp(buf, "true", 4) == 0) &&
		 (find_ws(buf) == buf+4)) {
		return json_value_new(EZCFG_JSON_VALUE_TYPE_TRUE);
	}
	else if ((*buf == '-' ) ||
		 ((*buf >= '0') &&
	          (*buf <= '9'))) {
		return json_value_new(EZCFG_JSON_VALUE_TYPE_NUMBER);
	}
	else {
		return NULL;
	}
}

static struct ezcfg_json_value *json_value_find_last_sibling(struct ezcfg_json_value *vp)
{
	struct ezcfg_json_value *tmp_vp = vp;

	while (tmp_vp->sibling != NULL) {
		tmp_vp = tmp_vp->sibling;
	}

	return tmp_vp;
}

/* false / null / true / object / array / number / string */
static bool json_value_is_native_value(struct ezcfg_json_value *vp)
{
	if ((vp->type == EZCFG_JSON_VALUE_TYPE_FALSE)  ||
	    (vp->type == EZCFG_JSON_VALUE_TYPE_NULL)   ||
	    (vp->type == EZCFG_JSON_VALUE_TYPE_TRUE)   ||
	    (vp->type == EZCFG_JSON_VALUE_TYPE_OBJECT) ||
	    (vp->type == EZCFG_JSON_VALUE_TYPE_ARRAY)  ||
	    (vp->type == EZCFG_JSON_VALUE_TYPE_NUMBER) ||
	    (vp->type == EZCFG_JSON_VALUE_TYPE_STRING)) {
		return true;
	}
	else {
		return false;
	}
}

static bool json_value_set_element(struct ezcfg_json_value *vp, char *buf, int len)
{
	vp->element = strndup(buf, len);
	if (vp->element == NULL) {
		return false;
	}
	else {
		return true;
	}
}

static bool json_value_set_nvpair_string(struct ezcfg_json_value *vp, char *buf, int len)
{
	return json_value_set_element(vp, buf, len);
}

static bool json_value_set_string_element(struct ezcfg_json_value *vp, char *buf, int len)
{
	return json_value_set_element(vp, buf, len);
}

static bool json_value_set_number_element(struct ezcfg_json_value *vp, char *buf, int len)
{
	return json_value_set_element(vp, buf, len);
}

/* Public functions */
void ezcfg_json_reset(struct ezcfg_json *json)
{
	//struct ezcfg *ezcfg;

	ASSERT(json != NULL);

	//ezcfg = json->ezcfg;

	/* first clean up stack stuff */
	stack_clean_up(&(json->stack), &(json->text));

	/* then clean up text stuff */
	if (json->text.sibling) {
		stack_push(&(json->stack), json->text.sibling);
		json->text.sibling = NULL;
	}
	if (json->text.value) {
		stack_push(&(json->stack), json->text.value);
		json->text.value = NULL;
	}

	stack_clean_up(&(json->stack), &(json->text));

	if (json->text.element) {
		free(json->text.element);
		json->text.element = NULL;
	}
	json->msg_len = 0;
}

void ezcfg_json_delete(struct ezcfg_json *json)
{
	//struct ezcfg *ezcfg;

	ASSERT(json != NULL);

	ezcfg_json_reset(json);

	free(json);
}

/**
 * ezcfg_json_new:
 * Create ezcfg json parser data structure
 * Returns: a new ezcfg json parser data structure
 **/
struct ezcfg_json *ezcfg_json_new(struct ezcfg *ezcfg)
{
	struct ezcfg_json *json = NULL;

	ASSERT(ezcfg != NULL);

	/* initialize json parser data structure */
	json = calloc(1, sizeof(struct ezcfg_json));
	if (json == NULL) {
		err(ezcfg, "initialize json parser error.\n");
		return NULL;
	}

	return json;
}

int ezcfg_json_get_text_type(struct ezcfg_json *json)
{
	//struct ezcfg *ezcfg;

	ASSERT(json != NULL);

	//ezcfg = json->ezcfg;

	return json->text.type;
}

bool ezcfg_json_parse_text(struct ezcfg_json *json, char *text, int len)
{
	//struct ezcfg *ezcfg;
	char *cur, *p;
	struct ezcfg_json_value *vp, *last_vp;
	bool ret = false;
	int buf_len;

	ASSERT(json != NULL);
	ASSERT(text != NULL);
	ASSERT(len > 0);

	cur = skip_ws(text);
	if (*cur == '{') {
		/* It's object start tag */
		json->text.type = EZCFG_JSON_VALUE_TYPE_OBJECT;
	}
	else if (*cur == '[') {
		/* It's array start tag */
		json->text.type = EZCFG_JSON_VALUE_TYPE_ARRAY;
	}
	else {
		return false;
	}

	stack_push(&(json->stack), &(json->text));

	while ((*cur != '\0') &&
	       (stack_is_empty(&(json->stack)) == false)) {
		cur = skip_ws(cur);
		vp = stack_pop(&(json->stack));
		switch (vp->type) {
		case EZCFG_JSON_VALUE_TYPE_OBJECT:
			if (vp->value == NULL) {
				if (*cur != '{') {
					goto func_out;
				}
				cur++;
				json->msg_len++; /* increase for '{' */
				cur = skip_ws(cur);
				if (*cur == '}') {
					/* it's an empty set object */
					vp->value = json_value_new(EZCFG_JSON_VALUE_TYPE_EMPTYSET);
					if (vp->value == NULL) {
						goto func_out;
					}
					cur++;
					json->msg_len++; /* increase for '}' */
					continue;
				}
				if (*cur != '"') {
					goto func_out;
				}
				/* It maybe a name string start tag of nvpair */
				vp->value = json_value_new(EZCFG_JSON_VALUE_TYPE_NVPAIR);
				if (vp->value == NULL) {
					goto func_out;
				}
				/* push back to the stack */
				if (stack_push(&(json->stack), vp) == false) {
					goto func_out;
				}
				if (stack_push(&(json->stack), vp->value) == false) {
					goto func_out;
				}
			}
			else if (vp->sibling == NULL) {
				if (*cur != '}') {
					goto func_out;
				}
				/* It's object end tag */
				cur++;
				json->msg_len++; /* increase for '}' */
			}
			else {
				goto func_out;
			}
			break;
		case EZCFG_JSON_VALUE_TYPE_NVPAIR:
			if (vp->value == NULL) {
				if (*cur != '"') {
					goto func_out;
				}
				cur++;
				json->msg_len++; /* increase for string start '"' */
				p = strchr(cur, '"');
				if (p == NULL) {
					goto func_out;
				}
				buf_len = p - cur;

				if (json_value_set_nvpair_string(vp, cur, buf_len) == false) {
					goto func_out;
				}
				json->msg_len += buf_len; /* increase for string content */
				json->msg_len++; /* increase for string end '"' */

				/* cur move to ": value" part */
				cur = skip_ws(p+1);

				/* value part of name-value pair not ready! */
				if (*cur != ':') {
					goto func_out;
				}
				cur++;
				json->msg_len++; /* increase for ':' */
				cur = skip_ws(cur);

				/* It maybe a value */
				vp->value = json_value_create(cur);
				if (vp->value == NULL) {
					goto func_out;
				}
				/* push back to the stack */
				if (stack_push(&(json->stack), vp) == false) {
					goto func_out;
				}
				if (stack_push(&(json->stack), vp->value) == false) {
					goto func_out;
				}
			}
			else if (vp->sibling == NULL) {
				if (*cur == '}') {
					/* It's object end tag */
					continue;
				}
				else if (*cur != ',') {
					goto func_out;
				}
				/* move to next name-value pair */
				cur++;
				json->msg_len++; /* increase for ',' */
				/* It maybe a name string start tag of nvpair */
				vp->sibling = json_value_new(EZCFG_JSON_VALUE_TYPE_NVPAIR);
				if (vp->sibling == NULL) {
					goto func_out;
				}

				/* push back to the stack */
				if (stack_push(&(json->stack), vp->sibling) == false) {
					goto func_out;
				}
			}
			else {
				goto func_out;
			}
			break;
		case EZCFG_JSON_VALUE_TYPE_ARRAY:
			if (vp->value == NULL) {
				if (*cur != '[') {
					goto func_out;
				}
				cur++;
				json->msg_len++; /* increase for '[' */
				cur = skip_ws(cur);
				if (*cur == ']') {
					/* it's an empty set object */
					vp->value = json_value_new(EZCFG_JSON_VALUE_TYPE_EMPTYSET);
					if (vp->value == NULL) {
						goto func_out;
					}
					cur++;
					json->msg_len++; /* increase for ']' */
					continue;
				}
				/* It maybe a value */
				vp->value = json_value_create(cur);
				if (vp->value == NULL) {
					goto func_out;
				}
				/* push back to the stack */
				if (stack_push(&(json->stack), vp) == false) {
					goto func_out;
				}
				if (stack_push(&(json->stack), vp->value) == false) {
					goto func_out;
				}
			}
			else if (vp->sibling == NULL) {
				if (*cur == ']') {
					/* It's array end tag */
					cur++;
					json->msg_len++; /* increase for ']' */
					continue;
				}
				if (*cur != ',') {
					goto func_out;
				}
				cur++;
				json->msg_len++; /* increase for ',' */
				cur = skip_ws(cur);
				/* It maybe a value */
				vp->sibling = json_value_create(cur);
				if (vp->sibling == NULL) {
					goto func_out;
				}

				/* check if vp->value is json native value */
				if (json_value_is_native_value(vp->value) == false) {
					goto func_out;
				}

				/* find the last element of the array */
				last_vp = json_value_find_last_sibling(vp->value);
				if (last_vp == NULL) {
					goto func_out;
				}
				last_vp->sibling = vp->sibling;
				vp->sibling = NULL;

				/* push back to the stack */
				if (stack_push(&(json->stack), vp) == false) {
					goto func_out;
				}
				if (stack_push(&(json->stack), last_vp->sibling) == false) {
					goto func_out;
				}
			}
			else {
				goto func_out;
			}
			break;
		case EZCFG_JSON_VALUE_TYPE_STRING:
			if (*cur != '"') {
				goto func_out;
			}
			/* It's name string start tag */
			cur++;
			json->msg_len++; /* increase for string start '"' */
			p = strchr(cur, '"');
			if (p == NULL) {
				goto func_out;
			}
			buf_len = p - cur;
			if (json_value_set_string_element(vp, cur, buf_len) == false) {
				goto func_out;
			}
			json->msg_len += buf_len; /* increase for string content */
			json->msg_len++; /* increase for string end '"' */
			cur = p+1;
			break;
		case EZCFG_JSON_VALUE_TYPE_FALSE:
			/* skip "false" */
			cur += 5;
			json->msg_len += 5; /* increase for false */
			break;
		case EZCFG_JSON_VALUE_TYPE_NULL:
			/* skip "null" */
			cur += 4;
			json->msg_len += 4; /* increase for null */
			break;
		case EZCFG_JSON_VALUE_TYPE_TRUE:
			/* skip "true" */
			cur += 4;
			json->msg_len += 4; /* increase for true */
			break;
		case EZCFG_JSON_VALUE_TYPE_NUMBER:
			p = get_number(cur);
			if (p == NULL) {
				goto func_out;
			}
			buf_len = p - cur;
			if (json_value_set_number_element(vp, cur, buf_len) == false) {
				goto func_out;
			}
			json->msg_len += buf_len; /* increase for number */
			cur = p; /* p point to the next char of number */
			break;
		default:
			goto func_out;
			break;
		}
	}

	cur = skip_ws(cur);
	if ((*cur == '\0') && (stack_is_empty(&(json->stack)) == true)) {
		ret = true;
	}

func_out:
	if (ret == false) {
		ezcfg_json_reset(json);
	}
	return ret;
}

int ezcfg_json_write_message(struct ezcfg_json *json, char *buf, int len)
{
	//struct ezcfg *ezcfg;
	int ret = -1;
	char *cur, *p;
	struct ezcfg_json_value *vp;
	int cur_len = 0;

	ASSERT(json != NULL);
	ASSERT(buf != NULL);
	ASSERT(len > 0);

	/* check if the buffer length is enough */
	if (len <= json->msg_len) {
		return ret;
	}

	/* check if the value type is valid */
	if ((json->text.type != EZCFG_JSON_VALUE_TYPE_OBJECT) &&
	    (json->text.type != EZCFG_JSON_VALUE_TYPE_ARRAY)) {
		return ret;
	}

	/* check if the stack is empty */
	if (stack_is_empty(&(json->stack)) == false) {
		return ret;
	}

	cur = buf;
	cur_len = 0;
	stack_push(&(json->stack), &(json->text));

	while ((cur_len < json->msg_len) &&
	       (stack_is_empty(&(json->stack)) == false)) {
		vp = stack_pop(&(json->stack));
		switch (vp->type) {
		case EZCFG_JSON_VALUE_TYPE_OBJECT:
			if (vp->visited == 0) {
				*cur = '{';
				cur++;
				cur_len++;
				vp->visited = 1;

				if (stack_push(&(json->stack), vp) == false) {
					goto func_out;
				}
				if (vp->value) {
					if (stack_push(&(json->stack), vp->value) == false) {
						goto func_out;
					}
				}
			}
			else if (vp->visited == 1) {
				*cur = '}';
				cur++;
				cur_len++;
				vp->visited = 0;

				if (vp->sibling) {
					*cur = ',';
					cur++;
					cur_len++;
					if (stack_push(&(json->stack), vp->sibling) == false) {
						goto func_out;
					}
				}
			}
			else {
				goto func_out;
			}
			break;
		case EZCFG_JSON_VALUE_TYPE_NVPAIR:
			if (vp->visited == 0) {
				if (vp->value == NULL) {
					goto func_out;
				}

				*cur = '"';
				cur++;
				cur_len++;

				p = vp->element;
				while(*p != '\0') {
					*cur++ = *p++;
					cur_len++;
				}

				*cur = '"';
				cur++;
				cur_len++;
				*cur = ':';
				cur++;
				cur_len++;
				vp->visited = 1;

				if (stack_push(&(json->stack), vp) == false) {
					goto func_out;
				}
				if (stack_push(&(json->stack), vp->value) == false) {
					goto func_out;
				}
			}
			else if (vp->visited == 1) {
				vp->visited = 0;
				if (vp->sibling) {
					*cur = ',';
					cur++;
					cur_len++;
					if (stack_push(&(json->stack), vp->sibling) == false) {
						goto func_out;
					}
				}
			}
			else {
				goto func_out;
			}
			break;
		case EZCFG_JSON_VALUE_TYPE_ARRAY:
			if (vp->visited == 0) {
				*cur = '[';
				cur++;
				cur_len++;
				vp->visited = 1;

				if (stack_push(&(json->stack), vp) == false) {
					goto func_out;
				}
				if (vp->value) {
					if (stack_push(&(json->stack), vp->value) == false) {
						goto func_out;
					}
				}
			}
			else if (vp->visited == 1) {
				*cur = ']';
				cur++;
				cur_len++;
				vp->visited = 0;

				if (vp->sibling) {
					*cur = ',';
					cur++;
					cur_len++;
					if (stack_push(&(json->stack), vp->sibling) == false) {
						goto func_out;
					}
				}
			}
			else {
				goto func_out;
			}
			break;
		case EZCFG_JSON_VALUE_TYPE_EMPTYSET:
			if (vp->value) {
				goto func_out;
			}

			if (vp->sibling) {
				goto func_out;
			}

			if (vp->element) {
				goto func_out;
			}
			break;
		case EZCFG_JSON_VALUE_TYPE_STRING:
			if (vp->value) {
				goto func_out;
			}

			*cur = '"';
			cur++;
			cur_len++;

			p = vp->element;
			while(*p != '\0') {
				*cur++ = *p++;
				cur_len++;
			}

			*cur = '"';
			cur++;
			cur_len++;

			if (vp->sibling) {
				*cur = ',';
				cur++;
				cur_len++;
				if (stack_push(&(json->stack), vp->sibling) == false) {
					goto func_out;
				}
			}
			break;
		case EZCFG_JSON_VALUE_TYPE_FALSE:
			if (vp->value) {
				goto func_out;
			}

			strcpy(cur, "false");
			cur += 5;
			cur_len += 5;

			if (vp->sibling) {
				*cur = ',';
				cur++;
				cur_len++;
				if (stack_push(&(json->stack), vp->sibling) == false) {
					goto func_out;
				}
			}
			break;
		case EZCFG_JSON_VALUE_TYPE_NULL:
			if (vp->value) {
				goto func_out;
			}

			strcpy(cur, "null");
			cur += 4;
			cur_len += 4;

			if (vp->sibling) {
				*cur = ',';
				cur++;
				cur_len++;
				if (stack_push(&(json->stack), vp->sibling) == false) {
					goto func_out;
				}
			}
			break;
		case EZCFG_JSON_VALUE_TYPE_TRUE:
			if (vp->value) {
				goto func_out;
			}

			strcpy(cur, "true");
			cur += 4;
			cur_len += 4;

			if (vp->sibling) {
				*cur = ',';
				cur++;
				cur_len++;
				if (stack_push(&(json->stack), vp->sibling) == false) {
					goto func_out;
				}
			}
			break;
		case EZCFG_JSON_VALUE_TYPE_NUMBER:
			if (vp->value) {
				goto func_out;
			}

			p = vp->element;
			while(*p != '\0') {
				*cur++ = *p++;
				cur_len++;
			}

			if (vp->sibling) {
				*cur = ',';
				cur++;
				cur_len++;
				if (stack_push(&(json->stack), vp->sibling) == false) {
					goto func_out;
				}
			}
			break;
		default:
			goto func_out;
			break;
		}
	}

	if ((cur_len == json->msg_len) &&
	    (stack_is_empty(&(json->stack)) == true)) {
		ret = cur_len;
	}

func_out:
	return ret;
}

