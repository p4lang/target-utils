/*
 * konf/tree/private.h
 */
#ifndef _konf_tree_private_h
#define _konf_tree_private_h

#include "target-utils/konf/tree.h"
#include "target-utils/lub/types.h"
#include "target-utils/lub/list.h"

/*---------------------------------------------------------
 * PRIVATE TYPES
 *--------------------------------------------------------- */
struct konf_tree_s {
	lub_list_t *list;
	char *line;
	unsigned short priority;
	unsigned short seq_num;
	unsigned short sub_num;
	bool_t splitter;
	int depth;
};

#endif
