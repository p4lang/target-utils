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
#ifndef _bf_hashtbl_h_
#define _bf_hashtbl_h_

#include <stdint.h>
#include <stddef.h>

typedef uint32_t bf_hash_t;

typedef enum bf_hashtbl_sts_t {
  BF_HASHTBL_OK,
  BF_HASHTBL_INVALID_ARG,
  BF_HASHTBL_ERR,
} bf_hashtbl_sts_t;

typedef int (*bf_htbl_cmp_fn)(const void *, const void *);
typedef void (*bf_htbl_free_fn)(void *);

typedef struct bf_hashtable_ {
  /* Comparison function */
  bf_htbl_cmp_fn cmp_fn;
  /* Free function during table cleanup */
  bf_htbl_free_fn free_fn;
  size_t key_sz;  /* Key size in bytes */
  size_t data_sz; /* Data size in bytes */
  uint32_t seed;  /* Seed for the hash table */
  void *phtbl;
} bf_hashtable_t;

bf_hashtbl_sts_t bf_hashtbl_init(bf_hashtable_t *htbl,
                                 int (*fn)(const void *, const void *),
                                 void (*free_fn)(void *),
                                 uint8_t key_sz,
                                 uint8_t data_sz,
                                 uint32_t seed);

void *bf_hashtbl_search(bf_hashtable_t *htbl, void *key);

bf_hashtbl_sts_t bf_hashtbl_insert(bf_hashtable_t *htbl, void *node, void *key);

void *bf_hashtbl_get_remove(bf_hashtable_t *htbl, void *key);

typedef void bf_hashtable_foreach_fn_t(void *arg, void *obj);

/* Invoke the function for each element in the hash table with arg as the
 * argument */
void bf_hashtbl_foreach_fn(bf_hashtable_t *htbl,
                           bf_hashtable_foreach_fn_t *foreach_fn,
                           void *arg);

void bf_hashtbl_delete(bf_hashtable_t *htbl);

void *bf_hashtbl_get_cmp_data(const void *arg);

#endif
