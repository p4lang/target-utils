/*
 * Copyright(c) 2021 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this software except as stipulated in the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <target-sys/bf_sal/bf_sys_intf.h>
#include <tommyhashlin.h>
#include "target-utils/hashtbl/bf_hashtbl.h"
#include "xxhash.h"

typedef struct bf_hashtbl_node_ {
  void *data;
  tommy_node tommy_obj;
} bf_hashtbl_node_t;

static bf_hash_t construct_hash(bf_hashtable_t *htbl, unsigned char *key) {
  uint8_t key_sz = htbl->key_sz;

  return XXH32(key, key_sz, htbl->seed);
}

static void bf_htbl_foreach_free_fn(void *arg, void *obj) {
  bf_hashtbl_node_t *htbl_node = obj;
  bf_hashtable_t *htbl = arg;
  if (htbl->free_fn) {
    htbl->free_fn(htbl_node->data);
  }
  bf_sys_free(obj);
  return;
}

void *bf_hashtbl_get_cmp_data(const void *arg) {
  return (((bf_hashtbl_node_t *)(arg))->data);
}

bf_hashtbl_sts_t bf_hashtbl_init(bf_hashtable_t *htbl,
                                 int (*fn)(const void *, const void *),
                                 void (*free_fn)(void *),
                                 uint8_t key_sz,
                                 uint8_t data_sz,
                                 uint32_t seed) {
  if (htbl == NULL) {
    return BF_HASHTBL_INVALID_ARG;
  }
  if (key_sz == 0) {
    return BF_HASHTBL_INVALID_ARG;
  }
  if (data_sz == 0) {
    return BF_HASHTBL_INVALID_ARG;
  }
  if (fn == NULL) {
    return BF_HASHTBL_INVALID_ARG;
  }
  htbl->cmp_fn = fn;
  htbl->free_fn = free_fn;
  htbl->key_sz = key_sz;
  htbl->data_sz = data_sz;
  htbl->seed = seed;

  htbl->phtbl = bf_sys_malloc(sizeof(tommy_hashlin));
  tommy_hashlin_init((tommy_hashlin *)(htbl->phtbl));

  return BF_HASHTBL_OK;
}

void *bf_hashtbl_search(bf_hashtable_t *htbl, void *key) {
  bf_hash_t hash = 0;
  bf_hashtbl_node_t *htbl_node = NULL;

  if (htbl == NULL) {
    return NULL;
  }
  if (key == NULL) {
    return NULL;
  }
  hash = construct_hash(htbl, (unsigned char *)key);

  htbl_node = tommy_hashlin_search(
      (tommy_hashlin *)htbl->phtbl, htbl->cmp_fn, (unsigned char *)key, hash);

  if (htbl_node == NULL) {
    return NULL;
  }
  return htbl_node->data;
}

bf_hashtbl_sts_t bf_hashtbl_insert(bf_hashtable_t *htbl,
                                   void *node,
                                   void *key) {
  bf_hash_t hash = 0;
  bf_hashtbl_node_t *hash_tbl_node = NULL;
  if (htbl == NULL) {
    return BF_HASHTBL_INVALID_ARG;
  }
  if (node == NULL) {
    return BF_HASHTBL_INVALID_ARG;
  }
  if (key == NULL) {
    return BF_HASHTBL_INVALID_ARG;
  }

  hash_tbl_node = bf_sys_calloc(1, sizeof(bf_hashtbl_node_t));
  if (hash_tbl_node == NULL) {
    return BF_HASHTBL_ERR;
  }
  hash_tbl_node->data = node;

  hash = construct_hash(htbl, (unsigned char *)key);

  tommy_hashlin_insert((tommy_hashlin *)htbl->phtbl,
                       &hash_tbl_node->tommy_obj,
                       hash_tbl_node,
                       hash);

  return BF_HASHTBL_OK;
}

void *bf_hashtbl_get_remove(bf_hashtable_t *htbl, void *key) {
  bf_hash_t hash = 0;
  bf_hashtbl_node_t *htbl_node = NULL;
  void *ret_node = NULL;

  if (htbl == NULL) {
    return NULL;
  }
  if (key == NULL) {
    return NULL;
  }

  hash = construct_hash(htbl, (unsigned char *)key);
  htbl_node = tommy_hashlin_remove(
      (tommy_hashlin *)htbl->phtbl, htbl->cmp_fn, (unsigned char *)key, hash);

  if (htbl_node == NULL) {
    return NULL;
  }
  ret_node = htbl_node->data;

  if (htbl_node) {
    bf_sys_free(htbl_node);
  }

  return ret_node;
}

void bf_hashtbl_foreach_fn(bf_hashtable_t *htbl,
                           bf_hashtable_foreach_fn_t *foreach_fn,
                           void *arg) {
  if (htbl == NULL) {
    return;
  }
  if (foreach_fn == NULL) {
    return;
  }

  tommy_hashlin_foreach_arg((tommy_hashlin *)htbl->phtbl, foreach_fn, arg);
  return;
}

void bf_hashtbl_delete(bf_hashtable_t *htbl) {
  if (htbl == NULL) {
    return;
  }

  tommy_hashlin_foreach_arg(
      (tommy_hashlin *)htbl->phtbl, bf_htbl_foreach_free_fn, htbl);

  tommy_hashlin_done((tommy_hashlin *)htbl->phtbl);
  bf_sys_free(htbl->phtbl);

  return;
}
