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
//
//  bf_id.h
//
//
//  Created on 6/20/14.
//
//

#ifndef _bf_id_h
#define _bf_id_h

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *bf_id_allocator;

bf_id_allocator *bf_id_allocator_new(unsigned int initial_size,
                                     bool zero_based);

void bf_id_allocator_destroy(bf_id_allocator *allocator);

unsigned int bf_id_allocator_allocate(bf_id_allocator *allocator);

int bf_id_allocator_allocate_contiguous(bf_id_allocator *allocator,
                                        uint8_t count);

void bf_id_allocator_release(bf_id_allocator *allocator, unsigned int id);

void bf_id_allocator_set(bf_id_allocator *allocator, unsigned int id);

int bf_id_allocator_is_set(bf_id_allocator *allocator, unsigned int id);

int bf_id_allocator_get_first(bf_id_allocator *allocator);

int bf_id_allocator_get_next(bf_id_allocator *allocator, unsigned int id);

void bf_id_allocator_copy(bf_id_allocator *dst_allocator,
                          bf_id_allocator *src_allocator);

#ifdef __cplusplus
}
#endif

#endif
