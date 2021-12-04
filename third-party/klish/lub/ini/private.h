/*
 * lub/ini/private.h
 *
 * Class to parse ini-like strings/files.
 */

#include "target-utils/lub/string.h"
#include "target-utils/lub/list.h"
#include "target-utils/lub/ini.h"

struct lub_pair_s {
	char *name;
	char *value;
};

struct lub_ini_s {
	lub_list_t *list;
};
