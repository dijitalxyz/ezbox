/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_tzdata.c
 *
 * Description  : get string from config file
 *
 * Copyright (C) 2010-2011 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-22   0.1       Write it from scrach
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

struct tz_pair {
	char *tz_name;
	char *tz_desc;
};

struct tz_pair ezcfg_support_areas[] = {
	{ "Africa", "Africa" },
	{ "America", "America" },
	{ "Antarctica", "Antarctica" },
	{ "Arctic", "Arctic Ocean" },
	{ "Asia", "Asia" },
	{ "Atlantic", "Atlantic Ocean" },
	{ "Australia", "Australia" },
	{ "Europe", "Europe" },
	{ "Indian", "Indian Ocean" },
	{ "Pacific", "Pacific Ocean" },
	{ "none", "Posix TZ format" },
};

struct tz_pair ezcfg_africa_locations[] = {
	{ "Algeria", "Algeria" },
};

struct tz_pair ezcfg_america_locations[] = {
	{ "Anguilla", "Anguilla" },
};

struct tz_pair ezcfg_antarctica_locations[] = {
	{ "McMurdo", "McMurdo Station, Ross Island" },
};

struct tz_pair ezcfg_arctica_locations[] = {
	{ "Longyearbyen", "Svalbard & Jan Mayen" },
};

struct tz_pair ezcfg_asia_locations[] = {
	{ "Shanghai", "east China - Beijing, Guangdong, Shanghai, etc." },
	{ "Hong_Kong", "Hong Kong" },
};

struct tz_pair ezcfg_atlantic_locations[] = {
	{ "Bermuda", "Bermuda" },
};

struct tz_pair ezcfg_australia_locations[] = {
	{ "Lord_Howe", "Lord Howe Island" },
};

struct tz_pair ezcfg_europe_locations[] = {
	{ "Mariehamn", "Aaland Islands" },
};

struct tz_pair ezcfg_indian_locations[] = {
	{ "Chagos", "British Indian Ocean Territory" },
};

struct tz_pair ezcfg_pacific_locations[] = {
	{ "Chatham", "Chatham Islands" },
};

struct tz_pair ezcfg_none_locations[] = {
	{ "GST-10", "GST-10" },
};

char *ezcfg_util_get_tz_area_desc(char *tz_area)
{
	int i;
	struct tz_pair *tzp;
	for (i = 0; i < ARRAY_SIZE(ezcfg_support_areas); i++) {
		tzp = &(ezcfg_support_areas[i]);
		if (strcmp(tzp->tz_name, tz_area) == 0)
			return tzp->tz_desc;
	}
	return NULL;
}

char *ezcfg_util_get_tz_location_desc(char *tz_area, char *tz_location)
{
	int i, location_length;
	struct tz_pair *tzp;
	if (strcmp(tz_area, "Africa") == 0) {
		tzp = ezcfg_africa_locations;
		location_length = ARRAY_SIZE(ezcfg_africa_locations);
	}
	else if (strcmp(tz_area, "America") == 0) {
		tzp = ezcfg_america_locations;
		location_length = ARRAY_SIZE(ezcfg_america_locations);
	}
	else if (strcmp(tz_area, "Antarctica") == 0) {
		tzp = ezcfg_antarctica_locations;
		location_length = ARRAY_SIZE(ezcfg_antarctica_locations);
	}
	else if (strcmp(tz_area, "Arctica") == 0) {
		tzp = ezcfg_arctica_locations;
		location_length = ARRAY_SIZE(ezcfg_arctica_locations);
	}
	else if (strcmp(tz_area, "Asia") == 0) {
		tzp = ezcfg_asia_locations;
		location_length = ARRAY_SIZE(ezcfg_asia_locations);
	}
	else if (strcmp(tz_area, "Atlantic") == 0) {
		tzp = ezcfg_atlantic_locations;
		location_length = ARRAY_SIZE(ezcfg_atlantic_locations);
	}
	else if (strcmp(tz_area, "Australia") == 0) {
		tzp = ezcfg_australia_locations;
		location_length = ARRAY_SIZE(ezcfg_australia_locations);
	}
	else if (strcmp(tz_area, "Europe") == 0) {
		tzp = ezcfg_europe_locations;
		location_length = ARRAY_SIZE(ezcfg_europe_locations);
	}
	else if (strcmp(tz_area, "Indian") == 0) {
		tzp = ezcfg_indian_locations;
		location_length = ARRAY_SIZE(ezcfg_indian_locations);
	}
	else if (strcmp(tz_area, "Pacific") == 0) {
		tzp = ezcfg_pacific_locations;
		location_length = ARRAY_SIZE(ezcfg_pacific_locations);
	}
	else if (strcmp(tz_area, "none") == 0) {
		tzp = ezcfg_none_locations;
		location_length = ARRAY_SIZE(ezcfg_none_locations);
	}
	else {
		return NULL;
	}

	for (i = 0; i < location_length; i++, tzp++) {
		if (strcmp(tzp->tz_name, tz_location) == 0)
			return tzp->tz_desc;
	}
	return NULL;
}

