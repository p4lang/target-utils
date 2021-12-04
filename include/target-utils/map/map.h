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
#ifndef _bf_map_h_
#define _bf_map_h_

#include <stdint.h>

/* Utility to map an unsigned long to a pointer.  Pointers can be added to a
 * database using unsigned longs as their keys.  They can then be looked up
 * and/or removed from the database using the key.
 * Implemented as a wrapper around JudyL
 */

typedef void *bf_map_t;

typedef enum bf_map_sts_t {
  BF_MAP_OK,
  BF_MAP_ERR,
  BF_MAP_NO_KEY,
  BF_MAP_KEY_EXISTS
} bf_map_sts_t;

bf_map_sts_t bf_map_add(bf_map_t *map, unsigned long key, void *data);
bf_map_sts_t bf_map_rmv(bf_map_t *map, unsigned long key);
bf_map_sts_t bf_map_get(bf_map_t *map, unsigned long key, void **data);
bf_map_sts_t bf_map_get_rmv(bf_map_t *map, unsigned long key, void **data);
bf_map_sts_t bf_map_get_first(bf_map_t *map, unsigned long *key, void **data);
bf_map_sts_t bf_map_get_next(bf_map_t *map, unsigned long *key, void **data);
bf_map_sts_t bf_map_get_first_rmv(bf_map_t *map,
                                  unsigned long *key,
                                  void **data);
bf_map_sts_t bf_map_init(bf_map_t *map);
void bf_map_destroy(bf_map_t *map);
uint32_t bf_map_count(bf_map_t *map);

#endif
