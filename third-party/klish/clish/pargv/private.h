/*
 * pargv.h
 */
#include "target-utils/clish/pargv.h"
#include "target-utils/clish/param.h"

/*--------------------------------------------------------- */
struct clish_parg_s {
	const clish_param_t *param;
	char *value;
};
struct clish_pargv_s {
	unsigned pargc;
	clish_parg_t **pargv;
};
/*--------------------------------------------------------- */
