/*
 * udata.h - private interface to the userdata class
 */

#include "target_utils/clish/udata.h"

struct clish_udata_s {
	char *name;
	void *data;
};
