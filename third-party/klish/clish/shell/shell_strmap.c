/*
 * shell_strmap.c
 */

#include <stdio.h>
#include <string.h>
#include "target-utils/clish/shell.h"
#include "private.h"
#include "target-utils/lub/bintree.h"
#include "target-utils/lub/string.h"
#include "target-utils/lub/porting.h"

typedef struct {
    lub_bintree_node_t node;
    char *key;
    char *value;
} map_node;

static int node_cmp(const void *clientnode, const void *clientkey) {
    map_node *node = (map_node *) clientnode;
    return lub_string_nocasecmp(node->key, *(char **) clientkey);
}

static void get_key(const void *clientnode, lub_bintree_key_t *key) {
    map_node *node = (map_node *) clientnode;
    *(char**)key = node->key;
}

lub_bintree_t *clish_shell_strmap_new() {
    lub_bintree_t *strmap = lub_malloc(sizeof(lub_bintree_t));
    if (!strmap)
        return NULL;
    memset(strmap, 0, sizeof(*strmap));
    lub_bintree_init(strmap, offsetof(map_node, node), node_cmp, get_key);
    return strmap;
}

void clish_shell_strmap_delete(clish_shell_t *shell) {
    // lub_bintree_findfirst doesn't check for null pointer
    if (!shell->strmap) {
	return;
    }
    map_node *node = lub_bintree_findfirst(shell->strmap);
    while (node) {
        lub_bintree_remove(shell->strmap, node);
        if (node->key)
            lub_free(node->key);
        if (node->value)
            lub_free(node->value);
        lub_free(node);
        node = lub_bintree_findfirst(shell->strmap);
    }
    lub_free(shell->strmap);
    shell->strmap = NULL;
}

int clish_shell_strmap_insert(clish_context_t *ctx, const char *key) {
    char *value = clish_shell_strmap_get(ctx, key);
    // if key already exists, do nothing and return success
    if (value != (char*)1) {
        return 0;
    }

    map_node *node = lub_malloc(sizeof(map_node));
    if (!node)
        return -1;
    memset(node, 0, sizeof(*node));
    lub_bintree_node_init(&node->node);
    node->key = lub_string_dup(key);
    //node->value = value;
    return lub_bintree_insert(ctx->shell->strmap, node);
}

char *clish_shell_strmap_get(clish_context_t *ctx, const char *key) {
    map_node *node = lub_bintree_find(ctx->shell->strmap, &key);
    if (!node)
        /*
         * we use 1 as a special "entry doesn't exist" value
         * because pretty much all systems will crash if you
         * try to access that memory... whereas NULL is a valid
         * pointer value one might want to store
         */
        return (char*)0x1;
    return node->value;
}

int clish_shell_strmap_set(clish_context_t *ctx, const char *key, const char *value) {
    map_node *node = lub_bintree_find(ctx->shell->strmap, &key);
    if (!node)
        return -1;
    if (node->value)
        lub_free(node->value);
    node->value = lub_string_dup(value);
    return 0;
}
