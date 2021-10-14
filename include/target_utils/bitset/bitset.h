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
#ifndef _BF_BITSET_H_
#define _BF_BITSET_H_

#include <inttypes.h>
#include <stdbool.h>

typedef struct bf_bitset_t {
  unsigned width;  // Number of bits
  uint64_t *bs;
} bf_bitset_t;

#define BF_BITSET_ARRAY_SIZE(width) (((width) + ((64) - 1)) / (64))

#define BF_BITSET(var, width)                         \
  bf_bitset_t var;                                    \
  uint64_t var##_[BF_BITSET_ARRAY_SIZE(width)] = {0}; \
  bf_bs_init(&var, width, var##_);

/* Initialize a bitset. */
void bf_bs_init(bf_bitset_t *bs, int width, uint64_t *mem);

/* Set the bit at "position" to "val", return it's previous value. */
bool bf_bs_set(bf_bitset_t *bs, int position, int val);

/* Return the value of the bit at "position". */
bool bf_bs_get(bf_bitset_t *bs, int position);

/* Set all bits to "val". */
void bf_bs_set_all(bf_bitset_t *bs, int val);

/* Returns the bit position of the first bit set, starts searching at
 * "position" and returns -1 if no bits are set. */
int bf_bs_first_set(bf_bitset_t *bs, int position);

/* Returns the bit position of the first bit clear, starts searching at
 * "position" and returns -1 if no bits are clear. */
int bf_bs_first_clr(bf_bitset_t *bs, int position);

/* Returns true if all bits in the set are 1. */
bool bf_bs_all_1s(bf_bitset_t *bs);

/* Returns true if all bits in the set are 0. */
bool bf_bs_all_0s(bf_bitset_t *bs);

/* Returns true if the two sets have the same values. */
bool bf_bs_equal(bf_bitset_t *x, bf_bitset_t *y);

/* Set upto 64-bits from the specified offset. */
void bf_bs_set_word(bf_bitset_t *bs, int position, int size, uint64_t val);

/* Get upto 64-bits from the specified offset. */
uint64_t bf_bs_get_word(bf_bitset_t *bs, int bit_offset, int n_bits);

/* Copies the contents of one bitset to another. */
void bf_bs_copy(bf_bitset_t *dst, bf_bitset_t *src);

/* Copies a portion of one bitset to another. */
void bf_bs_copy_range(bf_bitset_t *dst,
                      unsigned int dst_offset,
                      bf_bitset_t *src,
                      unsigned int src_offset,
                      unsigned int n_bits);

/* Get the number of bits set. */
int bf_bs_pop_count(bf_bitset_t *bs);

#endif /* _BF_BITSET_H_ */
