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
/*
 * Some bit twiddling utility functions gathered from the web and aim_utils.h
 * Particularly http://graphics.stanford.edu/~seander/bithacks.html
 */
#ifndef _BIT_UTILS_H_
#define _BIT_UTILS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

/* Calculate the log2 of a uint32 value and return it's ceil
 * log2(4) = 2
 * log2(5) = 3
 */
static inline uint32_t log2_uint32_ceil(uint32_t x) {
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  return (x > 1) ? (32 - __builtin_clz(x - 1)) : 0;
#else
  // not implemented
  assert(0);
  return 0;
#endif
}

/* Calculate the log2 of a uint32 value and return it's floor
 * log2(3) = 1
 * log2(4) = 2
 * log2(5) = 2
 */
static inline uint32_t log2_uint32_floor(uint32_t x) {
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  return x ? (31 - __builtin_clz(x)) : 0;
#else
  // not implemented
  assert(0);
  return 0;
#endif
}

/* Count the number of trailing zeroes. */
static inline uint32_t ctz(uint32_t x) {
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  return x ? __builtin_ctz(x) : 32;
#else
  // not implemented
  assert(0);
  return 0;
#endif
}

/**
 * Check whether an integer is a power of 2
 */
static inline bool is_uint32_power2(uint32_t x) {
  return x > 0 && (x & (x - 1)) == 0;
}

/* Calculate the next pow2 value. If x is a pow2, x is returned */
static inline uint32_t next_pow2(uint32_t x) {
  return (1 << log2_uint32_ceil(x));
}

#endif  //_BIT_UTILS_H_
