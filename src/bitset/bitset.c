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

#include <string.h>
#include <target-utils/bitset/bitset.h>
#include <Judy.h>
#include <target-sys/bf_sal/bf_sys_intf.h>

/* Number of uint64_t elements in the BitSet. */
static inline size_t length(bf_bitset_t *bs) {
  return BF_BITSET_ARRAY_SIZE(bs->width);
}

void bf_bs_init(bf_bitset_t *bs, int width, uint64_t *mem) {
  /* Expect 64-bit unsigned long */
  bf_sys_assert(sizeof(unsigned long long) == sizeof(uint64_t));

  bf_sys_assert(bs);
  bf_sys_assert(mem);
  bf_sys_assert(width > 0);
  bs->bs = mem;
  bs->width = width;
}

bool bf_bs_set(bf_bitset_t *bs, int position, int val) {
  bf_sys_assert(bs);
  bf_sys_assert(position >= 0);
  bf_sys_assert((unsigned)position < bs->width);

  if (position < 0 || (unsigned)position >= bs->width) {
    return false;
  }

  size_t idx = position >> 6;
  int shift = position & 0x3F;
  uint64_t tmp = UINT64_C(1) << shift;
  bool prev = bs->bs[idx] & tmp;

  if (val && !prev)
    bs->bs[idx] |= tmp;
  else if (!val && prev)
    bs->bs[idx] &= ~tmp;
  return prev;
}

bool bf_bs_get(bf_bitset_t *bs, int position) {
  bf_sys_assert(bs);
  bf_sys_assert(position >= 0);
  bf_sys_assert((unsigned)position < bs->width);

  if (position < 0 || (unsigned)position >= bs->width) {
    return false;
  }

  size_t idx = position >> 6;
  int shift = position & 0x3F;
  return (bs->bs[idx] >> shift) & 1;
}

static inline uint64_t top_word_mask(bf_bitset_t *bs) {
  uint64_t x = bs->width % 64;
  return x ? (UINT64_C(1) << x) - 1 : UINT64_C(0xFFFFFFFFFFFFFFFF);
}

void bf_bs_set_all(bf_bitset_t *bs, int val) {
  bf_sys_assert(bs);
  size_t len = length(bs);
  if (1 == val) {
    memset(bs->bs, 0xFF, 8 * len);
    bs->bs[len - 1] &= top_word_mask(bs);
  } else if (0 == val) {
    memset(bs->bs, 0, 8 * len);
  } else {
    bf_sys_assert(1 == val || 0 == val);
  }
}

int bf_bs_first_set(bf_bitset_t *bs, int position) {
  ++position;
  bf_sys_assert(bs);
  bf_sys_assert(position >= 0);
  bf_sys_assert((unsigned)position <= bs->width);
  if (position < 0 || (unsigned)position >= bs->width) {
    return -1;
  }

  size_t len = length(bs);
  size_t i = position / 64;
  int shift = position % 64;
  int x;

  bs->bs[len - 1] &= top_word_mask(bs);
  x = __builtin_ffsll(bs->bs[i] >> shift);
  if (x) {
    return 64 * i + x - 1 + shift;
  }

  if (i == (len - 1)) {
    return -1;
  }

  for (i = i + 1; i < len - 1; ++i) {
    x = __builtin_ffsll(bs->bs[i]);
    if (x) {
      return 64 * i + x - 1;
    }
  }
  x = __builtin_ffsll(bs->bs[i]);
  if (x && ((64 * i + x - 1) < bs->width)) {
    return 64 * i + x - 1;
  }
  return -1;
}

int bf_bs_first_clr(bf_bitset_t *bs, int position) {
  ++position;
  bf_sys_assert(bs);
  bf_sys_assert(position >= 0);
  bf_sys_assert((unsigned)position <= bs->width);
  if (position < 0 || (unsigned)position >= bs->width) {
    return -1;
  }

  size_t len = length(bs);
  size_t i = position / 64;
  int shift = position % 64;
  int x;

  bs->bs[len - 1] &= top_word_mask(bs);
  x = __builtin_ffsll((~bs->bs[i]) >> shift);
  if (x) {
    return 64 * i + x - 1 + shift;
  }

  if (i == (len - 1)) {
    return -1;
  }

  for (i = i + 1; i < len - 1; ++i) {
    x = __builtin_ffsll(~bs->bs[i]);
    if (x) {
      return 64 * i + x - 1;
    }
  }
  x = __builtin_ffsll(~bs->bs[i]);
  if (x && ((64 * i + x - 1) < bs->width)) {
    return 64 * i + x - 1;
  }
  return -1;
}

bool bf_bs_all_1s(bf_bitset_t *bs) {
  bf_sys_assert(bs);

  size_t len = length(bs);
  size_t i;
  for (i = 0; i < len - 1; ++i) {
    if (bs->bs[i] != INT64_C(0xFFFFFFFFFFFFFFFF)) {
      return false;
    }
  }
  /* Special handling for last index. */
  int left_over = bs->width & 0x3F;
  if (left_over) {
    return bs->bs[i] == ((UINT64_C(1) << left_over) - 1);
  } else {
    return bs->bs[i] == UINT64_C(0xFFFFFFFFFFFFFFFF);
  }
}

bool bf_bs_all_0s(bf_bitset_t *bs) {
  bf_sys_assert(bs);

  size_t len = length(bs);
  size_t i;
  bs->bs[len - 1] &= top_word_mask(bs);
  for (i = 0; i < len; ++i) {
    if (bs->bs[i]) {
      return false;
    }
  }
  return true;
}

bool bf_bs_equal(bf_bitset_t *x, bf_bitset_t *y) {
  bf_sys_assert(x);
  bf_sys_assert(y);
  if (x->width != y->width) return false;

  x->bs[length(x) - 1] &= top_word_mask(x);
  y->bs[length(y) - 1] &= top_word_mask(y);
  unsigned int i;
  for (i = 0; i < length(x); ++i) {
    uint64_t xx = bf_bs_get_word(x, i * 64, 64);
    uint64_t yy = bf_bs_get_word(y, i * 64, 64);
    if (xx != yy) return false;
  }
  return true;
}
void bf_bs_set_word(bf_bitset_t *bs, int position, int size, uint64_t val) {
  bf_sys_assert(bs);
  bf_sys_assert(position >= 0);
  bf_sys_assert((unsigned)position < bs->width);
  bf_sys_assert(size > 0);
  bf_sys_assert(size <= 64);

  /* MIN of requested size and number of bits from position to end of set. */
  size = (bs->width - (unsigned)position) < (unsigned)size
             ? bs->width - (unsigned)position
             : (unsigned)size;

  int mod_bit_offset = position % 64;
  /* Clear the existing bits in that position. */
  if (size < 64) {
    val &= ((UINT64_C(1) << size) - 1);
    bs->bs[position / 64] &= ~(((UINT64_C(1) << size) - 1) << mod_bit_offset);
  } else {
    bs->bs[position / 64] &= ~(UINT64_C(0xFFFFFFFFFFFFFFFF) << mod_bit_offset);
  }
  bs->bs[position / 64] |= (val << mod_bit_offset);
  int spill = mod_bit_offset + size - 64;
  if (spill > 0) {  // Might not spill into next uint64_t
    bs->bs[(position / 64) + 1] &= ~((UINT64_C(1) << spill) - 1);
    bs->bs[(position / 64) + 1] |= (val >> (size - spill));
  }
}

uint64_t bf_bs_get_word(bf_bitset_t *bs, int bit_offset, int n_bits) {
  bf_sys_assert(bs);
  bf_sys_assert(bit_offset >= 0);
  bf_sys_assert((unsigned)bit_offset < bs->width);
  bf_sys_assert(n_bits > 0);
  bf_sys_assert(n_bits <= 64);

  uint64_t word = 0;
  n_bits = (bs->width - (unsigned)bit_offset) < (unsigned)n_bits
               ? bs->width - (unsigned)bit_offset
               : (unsigned)n_bits;
  word = bs->bs[bit_offset / 64];
  int mod_bit_offset = bit_offset % 64;
  if (mod_bit_offset != 0) {
    word >>= mod_bit_offset;
    int spill = mod_bit_offset + n_bits - 64;
    if (spill > 0) {  // Might not spill into next uint64_t
      uint64_t word2 = bs->bs[(bit_offset / 64) + 1];
      word |= (word2 << (n_bits - spill));
    }
  }
  if (n_bits < 64) {
    word &= (UINT64_C(1) << n_bits) - 1;
  }
  return word;
}

void bf_bs_copy(bf_bitset_t *dst, bf_bitset_t *src) {
  bf_sys_assert(src);
  bf_sys_assert(dst);
  bf_sys_assert(src->width == dst->width);
  size_t i;
  for (i = 0; i < src->width / 64; ++i) dst->bs[i] = src->bs[i];
  dst->bs[i] = src->bs[i] & ((UINT64_C(1) << (src->width % 64)) - 1);
}

void bf_bs_copy_range(bf_bitset_t *dst,
                      unsigned int dst_offset,
                      bf_bitset_t *src,
                      unsigned int src_offset,
                      unsigned int n_bits) {
  bf_sys_assert(src);
  bf_sys_assert(dst);
  bf_sys_assert(src->width > (src_offset + n_bits - 1));
  bf_sys_assert(dst->width > (dst_offset + n_bits - 1));
  size_t i = 0;
  uint64_t x;
  for (i = 0; i < src->width / 64; ++i) {
    x = bf_bs_get_word(src, src_offset + i * 64, 64);
    bf_bs_set_word(dst, dst_offset + i * 64, 64, x);
  }
  x = bf_bs_get_word(src, src_offset + i * 64, n_bits % 64);
  bf_bs_set_word(dst, dst_offset + i * 64, n_bits % 64, x);
}

int bf_bs_pop_count(bf_bitset_t *bs) {
  bf_sys_assert(bs);
  int cnt = 0;
  size_t len = length(bs);
  bs->bs[len - 1] &= top_word_mask(bs);
  unsigned int i;
  for (i = 0; i < length(bs); ++i) cnt += __builtin_popcountll(bs->bs[i]);
  return cnt;
}
