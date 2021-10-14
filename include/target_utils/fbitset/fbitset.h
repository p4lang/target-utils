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
/** fbitset.h - Faster bitset implemented using judy.
 * Almost double the performance over bitset implemented using
 * bit arrays
 */

#ifndef _BF_FBITSET_H_
#define _BF_FBITSET_H_

#include <inttypes.h>
#include <stdbool.h>

typedef struct bf_fbitset_s {
  unsigned int width;  // Number of bits;
  void *fbs;
} bf_fbitset_t;

typedef enum bf_fbitset_sts_e {
  BF_FBITSET_OK,
  BF_FBITSET_MALLOC_ERR
} bf_fbitset_sts_t;

/* Initialize a bitset. */
void bf_fbs_init(bf_fbitset_t *bs, unsigned int width);

/* Set the bit at "position" to "val", return it's previous value. */
bool bf_fbs_set(bf_fbitset_t *bs, int position, int val);

/* Return the value of the bit at "position". */
bool bf_fbs_get(bf_fbitset_t *bs, int position);

/* Position is exclusive. Start search from width to find the last free */
int bf_fbs_prev_clr_contiguous(bf_fbitset_t *bs,
                               int position,
                               unsigned int count);

/* Position is exclusive. Start searching from -1 to find the first free*/
int bf_fbs_first_clr_contiguous(bf_fbitset_t *bs,
                                int position,
                                unsigned int count);

/* Position is exclusive. Start searching from -1 to find the first set. */
int bf_fbs_first_set(bf_fbitset_t *bs, int position);

void bf_fbs_destroy(bf_fbitset_t *bs);

#endif /* _BF_FBITSET_H_ */
