/*
 * view.h
 */
#include "target_utils/clish/view.h"
#include "target_utils/lub/bintree.h"
#include "target_utils/lub/list.h"
#include "target_utils/clish/hotkey.h"

/*---------------------------------------------------------
 * PRIVATE TYPES
 *--------------------------------------------------------- */
struct clish_view_s {
	lub_bintree_t tree;
	lub_bintree_node_t bt_node;
	char *name;
	char *prompt;
	char *access;
	lub_list_t *nspaces;
	clish_hotkeyv_t *hotkeys;
	unsigned int depth;
	clish_view_restore_e restore;
};
