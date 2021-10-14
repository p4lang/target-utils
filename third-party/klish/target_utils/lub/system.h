/*
 * system.h
 *
 */
#ifndef _lub_system_h
#define _lub_system_h

#include <stddef.h>

#include "target_utils/lub/c_decl.h"
#include "target_utils/lub/types.h"
#include "target_utils/lub/argv.h"

_BEGIN_C_DECL bool_t lub_system_test(int argc, char **argv);
bool_t lub_system_argv_test(const lub_argv_t * argv);
bool_t lub_system_line_test(const char *line);
char *lub_system_tilde_expand(const char *path);

_END_C_DECL
#endif				/* _lub_system_h */
