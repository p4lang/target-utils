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
/*!
 * @file power2_allocator.c
 * @date
 *
 * Allocator with special power2 constraints implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <Judy.h>
#include <target-utils/bit_utils/bit_utils.h>
#include <target-utils/power2_allocator/power2_allocator.h>
#include <target-sys/bf_sal/bf_sys_intf.h>

#if 0
#define POWER2_ALLOCATOR_ASSERT(x) power2_allocator_assert(x)
void power2_allocator_assert (power2_allocator_t *allocator);
#else
#define POWER2_ALLOCATOR_ASSERT(x)
#endif

/* Removes a particular free index of given free size. Size is used for
 * sanitization.
 */
static int power2_allocator_remove_free_index(power2_allocator_t *allocator,
                                              uint32_t free_index,
                                              uint32_t size) {
  PWord_t Pfree;
  int Rc_int;
  uint32_t tz = 0;
  uint32_t rem_size = 0, tmp_size = 0;
  uint32_t log2_size = 0, free_log2;
  uint32_t cur_index = 0;

  cur_index = free_index;
  rem_size = size;
  while (rem_size) {
    tz = ctz(cur_index);
    log2_size = log2_uint32_ceil(rem_size);

    if (tz < log2_size) {
      free_log2 = tz;
    } else {
      free_log2 = log2_size;
    }

    if (free_log2 >= allocator->no_free_lists) {
      free_log2 = allocator->no_free_lists - 1;
    }

    bf_sys_assert(free_log2 < 32);
    bf_sys_assert((cur_index % (1 << free_log2)) == 0);
    bf_sys_assert(ctz(cur_index) >= ctz(1 << free_log2));

    JLG(Pfree, allocator->free_lists[free_log2], (Word_t)cur_index);
    if (!Pfree) {
      bf_sys_assert(0);
      return 1;
    }

    tmp_size = *Pfree;
    JLD(Rc_int, allocator->free_lists[free_log2], (Word_t)cur_index);
    bf_sys_assert(Rc_int);

    if (rem_size < (1u << free_log2)) {
      bf_sys_assert(tmp_size == rem_size);
      cur_index += rem_size;
      rem_size = 0;
    } else {
      bf_sys_assert(tmp_size == (1u << free_log2));
      cur_index += (1 << free_log2);
      rem_size -= (1 << free_log2);
    }
  }

  return 0;
}

/* Removes one free index in the given log2 allocator which is greater
 * than or equal to the size requested and returns it
 */
static uint32_t power2_allocator_remove_one_free(power2_allocator_t *allocator,
                                                 uint32_t log2,
                                                 uint32_t size,
                                                 uint32_t *free_size) {
  Word_t index;
  PWord_t Pfree;
  int Rc_int;
  uint32_t free_index;

  if (log2 >= allocator->no_free_lists) {
    return -1;
  }
  bf_sys_assert(size <= (1u << log2));

  index = 0;
  JLF(Pfree, allocator->free_lists[log2], index);
  while (Pfree) {
    if (*Pfree >= size) {
      break;
    }
    JLN(Pfree, allocator->free_lists[log2], index);
  }

  if (Pfree == NULL) {
    return -1;
  }

  free_index = index;
  *free_size = *Pfree;

  JLD(Rc_int, allocator->free_lists[log2], index);
  bf_sys_assert(Rc_int);

  return free_index;
}

static int power2_allocator_insert_one_free(power2_allocator_t *allocator,
                                            uint32_t free_index,
                                            uint32_t size) {
  PWord_t Pfree;
  uint32_t tz = 0;
  uint32_t rem_size = 0;
  uint32_t log2_size = 0, free_log2;
  uint32_t cur_index = 0;

  cur_index = free_index;
  rem_size = size;
  while (rem_size) {
    /* Split the free size into chunks which are aligned
     * at 2^ boundaries
     */
    tz = ctz(cur_index);
    log2_size = log2_uint32_ceil(rem_size);

    if (tz < log2_size) {
      free_log2 = tz;
    } else {
      free_log2 = log2_size;
    }

    if (free_log2 >= allocator->no_free_lists) {
      free_log2 = allocator->no_free_lists - 1;
    }

    bf_sys_assert(free_log2 < 32);
    bf_sys_assert((cur_index % (1 << free_log2)) == 0);
    bf_sys_assert(ctz(cur_index) >= ctz(1 << free_log2));

    JLI(Pfree, allocator->free_lists[free_log2], (Word_t)cur_index);
    if (Pfree == PJERR) {
      bf_sys_assert(0);
      return -1;
    }

    bf_sys_assert(*Pfree == 0);

    if (rem_size < (1u << free_log2)) {
      bf_sys_assert(rem_size > ((1u << free_log2) >> 1));
      *Pfree = rem_size;
      cur_index += rem_size;
      rem_size = 0;
    } else {
      *Pfree = (1u << free_log2);
      cur_index += (1u << free_log2);
      rem_size -= (1u << free_log2);
    }
  }

  return 0;
}

static int power2_allocator_mark_inuse(power2_allocator_t *allocator,
                                       uint32_t alloc_index,
                                       uint32_t size) {
  Word_t index;
  PWord_t Pinuse;

  index = alloc_index;
  JLI(Pinuse, allocator->inuse_list, index);
  if (Pinuse == PJERR) {
    bf_sys_assert(0);
    return -1;
  }
  bf_sys_assert(*Pinuse == 0);
  *Pinuse = size;
  return 0;
}

uint32_t power2_allocator_get_index_size(power2_allocator_t *allocator,
                                         uint32_t alloc_index) {
  Word_t index;
  PWord_t Pinuse;
  uint32_t size;

  index = alloc_index;
  JLG(Pinuse, allocator->inuse_list, index);
  if (Pinuse == NULL) {
    return -1;
  }
  bf_sys_assert((Pinuse != PJERR) && *Pinuse);

  size = *Pinuse;
  return size;
}

static void power2_allocator_mark_free(power2_allocator_t *allocator,
                                       uint32_t alloc_index) {
  Word_t index;
  int Rc_int;

  index = alloc_index;
  JLD(Rc_int, allocator->inuse_list, index);
  bf_sys_assert(Rc_int == 1);
  return;
}

static uint32_t power2_allocator_get_next_inuse_block(
    power2_allocator_t *allocator, uint32_t alloc_index, uint32_t *size) {
  Word_t index;
  PWord_t Pinuse;

  index = alloc_index;
  JLN(Pinuse, allocator->inuse_list, index);
  if (Pinuse) {
    *size = *Pinuse;
    return index;
  } else {
    *size = 0;
    return -1;
  }
}

static uint32_t power2_allocator_get_prev_inuse_block(
    power2_allocator_t *allocator, uint32_t alloc_index, uint32_t *size) {
  Word_t index;
  PWord_t Pinuse;

  index = alloc_index;
  JLP(Pinuse, allocator->inuse_list, index);
  if (Pinuse) {
    *size = *Pinuse;
    return index;
  } else {
    *size = 0;
    return -1;
  }
}

/** \brief power2_allocator_make_copy
  *         Make a copy of the allocator and return the copy
  *
  * \param src The pointer to the power2 allocator that needs to be copied
  * \return Returns pointer to an allocator that has same state has the passed
  *in
  *          allocator.
  *         Null in case of failure
  */
power2_allocator_t *power2_allocator_make_copy(power2_allocator_t *src) {
  PWord_t Pfree;
  PWord_t Pinuse;
  PWord_t Pfree_dest;
  PWord_t Pinuse_dest;
  Word_t index;
  power2_allocator_t *dest;
  uint32_t i = 0;

  if (src == NULL) {
    return NULL;
  }

  dest = (power2_allocator_t *)bf_sys_calloc(sizeof(power2_allocator_t), 1);
  if (dest == NULL) {
    return NULL;
  }

  memcpy(dest, src, sizeof(power2_allocator_t));
  dest->free_lists =
      (Pvoid_t *)bf_sys_calloc(sizeof(Pvoid_t), src->no_free_lists);
  if (dest->free_lists == NULL) {
    bf_sys_free(dest);
    return NULL;
  }

  dest->inuse_list = (Pvoid_t)NULL;

  for (i = 0; i < dest->no_free_lists; i++) {
    index = 0;
    JLF(Pfree, src->free_lists[i], index);
    while (Pfree) {
      JLI(Pfree_dest, dest->free_lists[i], index);
      if (Pfree_dest == PJERR) {
        bf_sys_assert(0);
        goto cleanup;
      }
      *Pfree_dest = *Pfree;
      JLN(Pfree, src->free_lists[i], index);
    }
  }

  index = 0;
  JLF(Pinuse, src->inuse_list, index);
  while (Pinuse) {
    JLI(Pinuse_dest, dest->inuse_list, index);
    if (Pinuse_dest == PJERR) {
      bf_sys_assert(0);
      goto cleanup;
    }
    *Pinuse_dest = *Pinuse;
    JLN(Pinuse, src->inuse_list, index);
  }
  return dest;
cleanup:
  power2_allocator_destroy(dest);
  return NULL;
}

/** \brief power2_allocator_create
  *        Create a power2 allocator and initialize count number of
  *        size entries
  *
  * \param size Default size of the resource. This should be a power of 2
  * \param count The number of "size" resources available.
  * \return Returns the pointer to an allocator to use for future calls.
  *         Null in case of failure
  */
power2_allocator_t *power2_allocator_create(uint32_t size, uint32_t count) {
  uint32_t log2;
  power2_allocator_t *allocator = NULL;
  uint32_t free_index = 0;
  uint32_t i = 0;

  if (!is_uint32_power2(size)) {
    return NULL;
  }
  log2 = log2_uint32_ceil(size);

  allocator =
      (power2_allocator_t *)bf_sys_calloc(sizeof(power2_allocator_t), 1);
  if (allocator == NULL) {
    return NULL;
  }

  allocator->free_lists = (Pvoid_t *)bf_sys_calloc(sizeof(Pvoid_t), log2 + 1);
  if (allocator->free_lists == NULL) {
    bf_sys_free(allocator);
    return NULL;
  }

  allocator->inuse_list = (Pvoid_t)NULL;
  allocator->max_size = size;
  allocator->total_size = size * count;
  allocator->no_free_lists = log2 + 1;

  for (i = 0, free_index = 0; i < count; i++, free_index += size) {
    power2_allocator_insert_one_free(allocator, free_index, size);
  }
  return allocator;
}

/** \brief power2_allocator_destroy
  *        Destroy the state allocated for power2_allocator
  *
  * \param allocator The pointer to the power2 allocator returned by
  *power2_allocator_create
  * \return void
  */
void power2_allocator_destroy(power2_allocator_t *allocator) {
  if (!allocator) {
    return;
  }
  Word_t Rc_word;

  uint32_t i = 0;
  for (i = 0; i < allocator->no_free_lists; i++) {
    JLFA(Rc_word, allocator->free_lists[i]);
    (void)Rc_word;
  }
  JLFA(Rc_word, allocator->inuse_list);
  (void)Rc_word;

  bf_sys_free(allocator->free_lists);

  bf_sys_free(allocator);
}

/** \brief power2_allocator_alloc
  *        Allocate size resource such that the start index has at least
  *         log2(size) trailing zeroes.
  *
  * Example: If the request is for size = 5, the return index will have at least
  * 3 lsb bits set to zero. (0, 8, 16, 24 ....)
  *
  * \param allocator The power2 allocator
  * \param size The size to allocate
  * \return Index of the allocated space. -1 if there are any errors
  */
int power2_allocator_alloc(power2_allocator_t *allocator, uint32_t size) {
  uint32_t log2;
  uint32_t alloc_index, free_index = -1;
  int rc = 0;
  uint32_t i = 0;
  uint32_t free_size = 0;

  if (!allocator || (allocator->max_size < size)) {
    return -1;
  }

  /* Figure out the log of the size to start checking free lists */
  log2 = log2_uint32_ceil(size);
  for (i = log2; i < allocator->no_free_lists; i++) {
    free_index =
        power2_allocator_remove_one_free(allocator, i, size, &free_size);
    if (free_index != (uint32_t)-1) {
      break;
    }
  }

  if (free_index == (uint32_t)-1) {
    /* No Space */
    return -1;
  }

  /* Pick up the last index which satisfies the size in this free chunk */
  alloc_index = (free_index + free_size - 1) & ~((1ull << log2) - 1);
  if ((alloc_index + size) > (free_index + free_size)) {
    alloc_index -= (1 << log2);
  }

  bf_sys_assert(alloc_index >= free_index);
  bf_sys_assert((alloc_index + size) <= (free_index + free_size));
  /* Split the remaining space into various chunks */
  rc = power2_allocator_insert_one_free(
      allocator, free_index, (alloc_index - free_index));
  if (rc) {
    bf_sys_assert(0);
    return -1;
  }

  rc = power2_allocator_insert_one_free(
      allocator,
      alloc_index + size,
      (free_index + free_size) - (alloc_index + size));
  if (rc) {
    bf_sys_assert(0);
    return -1;
  }

  rc = power2_allocator_mark_inuse(allocator, alloc_index, size);
  if (rc) {
    return -1;
  }

  POWER2_ALLOCATOR_ASSERT(allocator);
  return alloc_index;
}

/** \brief power2_allocator_release
  *        Release the resource that was allocated at given index
  *
  * \param allocator The power2 allocator
  * \param index Index to release
  * \return Status of the operation. 0 for SUCCESS.
  *         non-zero in case of errors
  */
int power2_allocator_release(power2_allocator_t *allocator, uint32_t index) {
  uint32_t free_index = 0;
  uint32_t free_size = 0;
  uint32_t size = 0;
  uint32_t prev_index, prev_size;
  uint32_t next_index, next_size;
  int rc;

  size = power2_allocator_get_index_size(allocator, index);
  if (size == (uint32_t)-1) {
    return -1;
  }
  free_size = size;

  prev_index =
      power2_allocator_get_prev_inuse_block(allocator, index, &prev_size);
  next_index =
      power2_allocator_get_next_inuse_block(allocator, index, &next_size);

  if (prev_index != (uint32_t)-1) {
    free_index = prev_index + prev_size;
  }

  /* There is room for improvement here.
   * We don't have to coalesce all the free blocks. We need
   * identify the maximum contiguous block that can be formed
   */
  free_size += index - free_index;
  /* Remove the free blocks from the free list */
  rc = power2_allocator_remove_free_index(
      allocator, free_index, index - free_index);
  if (rc) {
    bf_sys_assert(0);
    return -1;
  }

  if (next_index == (uint32_t)-1) {
    next_index = allocator->total_size;
  }
  free_size += next_index - (index + size);
  /* Remove the free blocks from the free list */
  rc = power2_allocator_remove_free_index(
      allocator, index + size, next_index - (index + size));
  if (rc) {
    bf_sys_assert(0);
    return -1;
  }

  rc = power2_allocator_insert_one_free(allocator, free_index, free_size);
  if (rc) {
    bf_sys_assert(0);
    return -1;
  }

  power2_allocator_mark_free(allocator, index);
  POWER2_ALLOCATOR_ASSERT(allocator);
  return 0;
}

/** \brief power2_allocator_reserve
  *         Reserve a block of indexes as inuse in the power2 allocator
  *
  * \param allocator The power2 allocator
  * \param reserve_index The index to reserve
  * \param size The size to reserve
  * \return Status of the operation.
  *     0 in case of success.
  *     1 in case any indexes within the reserve_index, reserve_index+size are
  *already in use
  *     -1 in case of any other errors
  */
int power2_allocator_reserve(power2_allocator_t *allocator,
                             uint32_t reserve_index,
                             uint32_t reserve_size) {
  uint32_t prev_index, prev_size;
  uint32_t next_index, next_size;
  uint32_t prev_free_index, next_inuse_index;
  uint32_t size = 0;
  int rc = 0;

  size = power2_allocator_get_index_size(allocator, reserve_index);
  if (size != (uint32_t)-1) {
    /* This reserve_index is already in use */
    return 1;
  }
  prev_index = power2_allocator_get_prev_inuse_block(
      allocator, reserve_index, &prev_size);
  next_index = power2_allocator_get_next_inuse_block(
      allocator, reserve_index, &next_size);
  if (prev_index != (uint32_t)-1) {
    prev_free_index = prev_index + prev_size;
  } else {
    prev_free_index = 0;
  }

  if (next_index == (uint32_t)-1) {
    next_inuse_index = allocator->total_size;
  } else {
    next_inuse_index = next_index;
  }

  /* Check if the range is free */
  if (reserve_index < prev_free_index) {
    return 1;
  }

  if ((reserve_index + reserve_size) > next_inuse_index) {
    return 1;
  }

  /* Remove the free blocks from the free list */
  rc = power2_allocator_remove_free_index(
      allocator, prev_free_index, next_inuse_index - prev_free_index);
  if (rc) {
    bf_sys_assert(0);
    return -1;
  }

  /* Insert the 2 chunks to the left and right of the free block into free lists
   */
  rc = power2_allocator_insert_one_free(
      allocator, prev_free_index, reserve_index - prev_free_index);
  if (rc) {
    bf_sys_assert(0);
    return -1;
  }

  rc = power2_allocator_insert_one_free(
      allocator,
      reserve_index + reserve_size,
      next_inuse_index - (reserve_index + reserve_size));
  if (rc) {
    bf_sys_assert(0);
    return -1;
  }

  rc = power2_allocator_mark_inuse(allocator, reserve_index, reserve_size);
  if (rc) {
    return -1;
  }

  POWER2_ALLOCATOR_ASSERT(allocator);
  return 0;
}

static int power2_allocator_alloc_int_v2(power2_allocator_t *allocator,
                                         uint32_t log2,
                                         uint32_t size,
                                         uint32_t count,
                                         uint32_t align) {
  Word_t index;
  PWord_t Pfree;
  int Rc_int;
  bool found = false;
  uint32_t free_size[count];
  uint32_t free_index[count];
  uint32_t next_index;
  uint32_t i = 0;
  int rc;

  if (log2 != allocator->no_free_lists - 1 || count == 0) {
    return -1;
  }
  bf_sys_assert(size <= (1u << log2));

  index = 0;
  while (true) {
    next_index = index + align;
    JLF(Pfree, allocator->free_lists[log2], index);
    if (Pfree) {
      if (index % align) {
        index = next_index;
        continue;
      }

      /* Check if we have contiguous spaces for count no */
      for (i = 0; Pfree && (i < count); i++) {
        free_size[i] = *Pfree;
        free_index[i] = index;
        if (*Pfree < size) {
          break;
        }
        index += (1 << log2);
        JLG(Pfree, allocator->free_lists[log2], index);
      }

      if (i == count) {
        found = true;
        break;
      }
      index = next_index;
    } else {
      break;
    }
  }

  if (!found) {
    /* No Space */
    return -1;
  }

  /* Remove all of the free_indexes */
  for (i = 0; i < count; i++) {
    JLD(Rc_int, allocator->free_lists[log2], free_index[i]);
    bf_sys_assert(Rc_int);

    rc = power2_allocator_insert_one_free(
        allocator, free_index[i] + size, (free_size[i] - size));
    bf_sys_assert(rc == 0);
  }

  /* Mark everything as in use */
  for (i = 0; i < count; i++) {
    bf_sys_assert(free_index[i] == (free_index[0] + (i * (1 << log2))));
    rc = power2_allocator_mark_inuse(allocator, free_index[i], size);
    if (rc) {
      bf_sys_assert(0);
      return -1;
    }
  }
  return free_index[0];
}

/** \brief power2_allocator_alloc_multiple
  *        Allocate size resource such that the start index has at least
  *         log2(size) trailing zeroes.
  *
  * Example: If the request is for size = 5, the return index will have at least
  * 3 lsb bits set to zero. (0, 8, 16, 24 ....)
  *
  * \param allocator The power2 allocator
  * \param size The size to allocate
  * \return Index of the allocated space. -1 if there are any errors
  */
int power2_allocator_alloc_multiple(power2_allocator_t *allocator,
                                    uint32_t size,
                                    uint32_t count) {
  uint32_t log2;
  uint32_t alloc_index;
  uint32_t total_size = 0;
  uint32_t align = 0;

  if (count == 1) {
    return power2_allocator_alloc(allocator, size);
  }

  total_size = (1 << log2_uint32_ceil(size)) * count;
  if (!allocator || (allocator->total_size < total_size)) {
    return -1;
  }

  align = (1 << log2_uint32_ceil(total_size));

  /* Figure out the log of the size to start checking free lists */
  log2 = log2_uint32_ceil(size);

  /* If we need more than 1 blocks, the size to request
   * should be in this block
   * Handling other case is as good as first fit, which needs more traversal.
   * Currently not handled
   */
  if (log2 != allocator->no_free_lists - 1) {
    return -1;
  }

  alloc_index =
      power2_allocator_alloc_int_v2(allocator, log2, size, count, align);
  if (alloc_index == (uint32_t)-1) {
    /* No space */
    return -1;
  }

  POWER2_ALLOCATOR_ASSERT(allocator);
  return alloc_index;
}

/** \brief power2_allocator_release_multiple
  *        Release the resource that was allocated at given index
  *
  * \param allocator The power2 allocator
  * \param index Index to release
  * \return Status of the operation. 0 for SUCCESS.
  *         non-zero in case of errors
  */
int power2_allocator_release_multiple(power2_allocator_t *allocator,
                                      uint32_t index,
                                      uint32_t count) {
  uint32_t free_index;
  uint32_t free_size = 0;
  uint32_t size = 0;
  uint32_t next_index, next_size;
  uint32_t log2 = 0;
  uint32_t i = 0;
  int rc;

  if (count == 1) {
    return power2_allocator_release(allocator, index);
  }

  /* When count > 1, the index will be aligned at the highest log available.
   * So we do not have to coalesce the prev-free block
   */
  size = power2_allocator_get_index_size(allocator, index);
  if (size == (uint32_t)-1) {
    return -1;
  }

  log2 = log2_uint32_ceil(size);
  if (log2 != allocator->no_free_lists - 1) {
    return -1;
  }

  for (i = 0; i < count; i++) {
    free_size = size;
    free_index = index;

    next_index =
        power2_allocator_get_next_inuse_block(allocator, index, &next_size);
    if (next_index == (uint32_t)-1) {
      next_index = allocator->total_size;
    }
    free_size += next_index - (index + size);
    /* Remove it from the free list */
    rc = power2_allocator_remove_free_index(
        allocator, index + size, next_index - (index + size));
    if (rc) {
      bf_sys_assert(0);
      return -1;
    }

    rc = power2_allocator_insert_one_free(allocator, free_index, free_size);
    if (rc) {
      bf_sys_assert(0);
      return -1;
    }
    power2_allocator_mark_free(allocator, index);
    index += (1 << log2);
  }

  POWER2_ALLOCATOR_ASSERT(allocator);
  return 0;
}

/** \brief power2_allocator_alloc_count_by_size
  *        Get the number of allocated elements of a given size.
  *
  * \param allocator The power2 allocator.
  * \param size The size of the element.
  * \return The number of elements allocated of the specified size.
  */
int power2_allocator_alloc_count_by_size(power2_allocator_t *allocator,
                                         uint32_t size) {
  int count = 0;
  if (size > allocator->max_size) return 0;

  Word_t Index = 0;
  PWord_t PValue = NULL;
  JLF(PValue, allocator->inuse_list, Index);
  while (PValue) {
    if (*PValue == size) {
      ++count;
    }
    JLN(PValue, allocator->inuse_list, Index);
  }
  return count;
}

/* power2_allocator_usage basically gives the number of indices in use.
 * This is different from the power2_allocator_alloc_count and
 * power2_allocator_alloc_count_by_size which just gives the number of
 * in_use_lists and not the overall number of indices in use.
 */

int power2_allocator_usage(power2_allocator_t *allocator) {
  int count = 0;

  Word_t Index = 0;
  PWord_t PValue = NULL;
  JLF(PValue, allocator->inuse_list, Index);
  while (PValue) {
    count += (*PValue);
    JLN(PValue, allocator->inuse_list, Index);
  }
  return count;
}

uint32_t power2_allocator_alloc_count(power2_allocator_t *allocator) {
  if (!allocator) return 0;
  Word_t Rc_word;
  JLC(Rc_word, allocator->inuse_list, 0, -1);
  uint32_t r = Rc_word;
  return r;
}

/* The power2_allocator_set API is not complete and doesn't work for
 * all cases i.e. for alloc_index = 8 and size 5 if the 13-15 are
 * already in use, it's not working now. For now, instead of trying
 * to handle this (and expose any other issues), going ahead with making a
 * backup copy of the table and then restoring from it
 */
#ifdef NOT_COMPLETE
/** \brief power2_allocator_set
  *        This is used to handle the backup/restore operations where
  *         the client may need to set a particular index for use with certain
  *         size. The library however does some sanity checks to ensure that
  *         it's internal state is not harmed and returns error
  *
  * \param allocator The power2 allocator
  * \param index Index to set
  * \param size The size that was allocated for this index
  * \return Index of the allocated space. -1 if there are any errors
  */
int power2_allocator_set(power2_allocator_t *allocator,
                         uint32_t alloc_index,
                         uint32_t size) {
  if (!allocator || (allocator->max_size < size)) {
    return -1;
  }

  if ((alloc_index + size) > allocator->total_size) {
    return -1;
  }

  if (power2_allocator_get_index_size(allocator, alloc_index) != -1) {
    return -1;
  }

  /* Make sure that the alloc_index lies in the free range */
  uint32_t next_index =
      power2_allocator_get_next_used_index(allocator, alloc_index);
  if ((next_index != -1) && ((alloc_index + size) >= next_index)) {
    return -1;
  }

  uint32_t prev_index =
      power2_allocator_get_prev_used_index(allocator, alloc_index);
  if (prev_index != -1) {
    uint32_t prev_size = power2_allocator_get_index_size(allocator, prev_index);
    if ((prev_index + prev_size) >= alloc_index) {
      return -1;
    }
  }

  Word_t index;
  Word_t log2_index;
  PWord_t Pfree;
  int Rc_int;
  uint32_t log2;
  uint32_t found_index;
  uint32_t found_log2;
  bool found = false;

  log2 = log2_uint32_ceil(size);

  log2_index = log2;
  JLF(Pfree, allocator->free_list, log2_index);
  while (Pfree) {
    index = alloc_index;
    J1L(Rc_int, *(Pvoid_t *)Pfree, index);
    if (!Rc_int) {
      JLN(Pfree, allocator->free_list, log2_index);
      continue;
    }
    if (alloc_index < (index + (1 << log2_index))) {
      found_index = index;
      found_log2 = log2_index;
      found = true;
      break;
    }
    JLN(Pfree, allocator->free_list, log2_index);
  }
  if (!found) {
    return -1;
  }

  bf_sys_assert(found_index <= alloc_index);
  power2_allocator_remove_one_free(allocator, found_log2, false, found_index);
  power2_allocator_split_and_free(
      allocator, found_index, alloc_index - found_index);
  /* Free up the remaining space */
  power2_allocator_split_and_free(
      allocator,
      alloc_index + size,
      (found_index + (1 << found_log2)) - (size + alloc_index));

  /* Actually allocate this index by removing in free list */
  int rc = power2_allocator_mark_inuse(allocator, alloc_index, size);
  if (rc) {
    return -1;
  }

  return 0;
}
#endif

int power2_allocator_first_alloc(power2_allocator_t *allocator) {
  PWord_t Pinuse;
  Word_t index = 0;

  if (allocator == NULL) {
    return -1;
  }
  JLF(Pinuse, allocator->inuse_list, index);
  if (Pinuse) {
    return index;
  }
  return -1;
}

int power2_allocator_next_alloc(power2_allocator_t *allocator, int idx) {
  PWord_t Pinuse;
  Word_t index = idx;

  if (allocator == NULL) {
    return -1;
  }
  JLN(Pinuse, allocator->inuse_list, index);
  if (Pinuse) {
    return index;
  }
  return -1;
}

void power2_allocator_print(power2_allocator_t *allocator) {
  PWord_t Pfree;
  PWord_t Pinuse;
  Word_t index;
  uint32_t size = 0;

  if (allocator == NULL) {
    return;
  }

  printf("FREE\n");
  uint32_t log2_index;
  for (log2_index = 0; log2_index < allocator->no_free_lists; log2_index++) {
    printf("\tLog2 %d\n", log2_index);
    index = 0;
    JLF(Pfree, allocator->free_lists[log2_index], index);
    while (Pfree) {
      size = *Pfree;
      printf("\t\tIndex %-8d: Size %-8d\n", (uint32_t)index, size);
      JLN(Pfree, allocator->free_lists[log2_index], index);
    }
  }

  printf("USED\n");
  index = 0;
  JLF(Pinuse, allocator->inuse_list, index);
  while (Pinuse) {
    printf("\t\tIndex %-8d: Size %-8d\n", (uint32_t)index, (uint32_t)*Pinuse);
    JLN(Pinuse, allocator->inuse_list, index);
  }
}

void power2_allocator_free_list_assert(power2_allocator_t *allocator,
                                       uint32_t free_index,
                                       uint32_t size) {
  PWord_t Pfree;
  uint32_t tz = 0;
  uint32_t rem_size = 0, tmp_size = 0;
  uint32_t log2_size = 0, free_log2;
  uint32_t cur_index = 0;

  cur_index = free_index;
  rem_size = size;
  while (rem_size) {
    tz = ctz(cur_index);
    log2_size = log2_uint32_ceil(rem_size);

    if (tz < log2_size) {
      free_log2 = tz;
    } else {
      free_log2 = log2_size;
    }

    if (free_log2 >= allocator->no_free_lists) {
      free_log2 = allocator->no_free_lists - 1;
    }

    bf_sys_assert(free_log2 < 32);
    bf_sys_assert((cur_index % (1 << free_log2)) == 0);
    bf_sys_assert(ctz(cur_index) >= ctz(1 << free_log2));

    JLG(Pfree, allocator->free_lists[free_log2], (Word_t)cur_index);
    if (!Pfree) {
      bf_sys_assert(0);
      return;
    }

    tmp_size = *Pfree;

    if (rem_size < (1u << free_log2)) {
      bf_sys_assert(tmp_size == rem_size);
      cur_index += rem_size;
      rem_size = 0;
    } else {
      bf_sys_assert(tmp_size == (1u << free_log2));
      cur_index += (1u << free_log2);
      rem_size -= (1u << free_log2);
    }
  }
}

void power2_allocator_assert(power2_allocator_t *allocator) {
  Pvoid_t free_array = NULL;
  Pvoid_t inuse_array = NULL;
  PWord_t Pfree;
  PWord_t Pinuse;
  Word_t index;
  Word_t Rc_word;
  int Rc_int;
  uint32_t i = 0;
  uint32_t size = 0;

  if (allocator == NULL) {
    return;
  }

  /* For each of the indexes that are in the free list, make sure that they
   * are not in inuse list
   */
  uint32_t log2_index;
  for (log2_index = 0; log2_index < allocator->no_free_lists; log2_index++) {
    index = 0;
    JLF(Pfree, allocator->free_lists[log2_index], index);
    while (Pfree) {
      size = *Pfree;
      bf_sys_assert(size <= (1u << log2_index));
      bf_sys_assert(size > ((1u << log2_index) >> 1));
      for (i = index; i < index + size; i++) {
        Word_t free_index = i;
        J1S(Rc_int, free_array, free_index);
        bf_sys_assert(Rc_int);
      }
      JLN(Pfree, allocator->free_lists[log2_index], index);
    }
  }

  index = 0;
  JLF(Pinuse, allocator->inuse_list, index);
  while (Pinuse) {
    for (i = index; i < index + *Pinuse; i++) {
      Word_t inuse_index = i;
      J1S(Rc_int, inuse_array, inuse_index);
      bf_sys_assert(Rc_int);
    }
    JLN(Pinuse, allocator->inuse_list, index);
  }

  int Rc_free, Rc_inuse;
  /* Now verify that there's no overlap */
  for (i = 0; i < allocator->total_size; i++) {
    index = i;
    J1T(Rc_free, free_array, index);
    J1T(Rc_inuse, inuse_array, index);
    if (Rc_free) {
      bf_sys_assert(!Rc_inuse);
    } else {
      bf_sys_assert(Rc_inuse);
    }
  }

  /* Verify that the free blocks are in their appropriate log2_indexes */
  uint32_t free_index = 0;
  index = 0;
  JLF(Pinuse, allocator->inuse_list, index);
  while (Pinuse) {
    power2_allocator_free_list_assert(
        allocator, free_index, (uint32_t)index - free_index);
    free_index = index + *Pinuse;
    JLN(Pinuse, allocator->inuse_list, index);
  }

  if (free_index != allocator->total_size) {
    power2_allocator_free_list_assert(
        allocator, free_index, allocator->total_size - free_index);
  }

  index = allocator->total_size;
  J1F(Rc_free, free_array, index);
  bf_sys_assert(!Rc_free);
  J1F(Rc_inuse, inuse_array, index);
  bf_sys_assert(!Rc_inuse);
  J1FA(Rc_word, free_array);
  (void)Rc_word;
  J1FA(Rc_word, inuse_array);
  (void)Rc_word;
}

int power2_alloc_utest(void) {
  power2_allocator_t *a1 = NULL, *a2 = NULL;
  uint32_t index = 0;
  uint32_t r = 0, c = 0, s = 0;
  int rc = 0;

  a1 = power2_allocator_create(1024, 2);
  bf_sys_assert(a1);

  r = 1;
  index = power2_allocator_alloc(a1, r);
  bf_sys_assert(index == 1023);
  bf_sys_assert(ctz(index) >= log2_uint32_ceil(r));
  power2_allocator_assert(a1);

  r = 3;
  index = power2_allocator_alloc(a1, r);
  bf_sys_assert(index == 1020);
  bf_sys_assert(ctz(index) >= log2_uint32_ceil(r));
  power2_allocator_assert(a1);

  r = 50;
  index = power2_allocator_alloc(a1, r);
  bf_sys_assert(index == 960);
  bf_sys_assert(ctz(index) >= log2_uint32_ceil(r));
  power2_allocator_assert(a1);

  rc = power2_allocator_reserve(a1, 980, 20);
  // Fail
  bf_sys_assert(rc);
  power2_allocator_assert(a1);

  rc = power2_allocator_reserve(a1, 946, 10);
  bf_sys_assert(!rc);
  power2_allocator_assert(a1);

  r = 50;
  index = power2_allocator_alloc(a1, r);
  bf_sys_assert(index == 896);
  bf_sys_assert(ctz(index) >= log2_uint32_ceil(r));
  power2_allocator_assert(a1);

  rc = power2_allocator_reserve(a1, 850, 5);
  bf_sys_assert(!rc);
  power2_allocator_assert(a1);

  r = 50;
  index = power2_allocator_alloc(a1, r);
  bf_sys_assert(index == 768);
  bf_sys_assert(ctz(index) >= log2_uint32_ceil(r));
  power2_allocator_assert(a1);

  power2_allocator_release(a1, 960);
  power2_allocator_assert(a1);

  power2_allocator_release(a1, 896);
  power2_allocator_assert(a1);

  power2_allocator_release(a1, 946);
  power2_allocator_assert(a1);

  r = 100;
  index = power2_allocator_alloc(a1, r);
  bf_sys_assert(index == 896);
  bf_sys_assert(ctz(index) >= log2_uint32_ceil(r));
  power2_allocator_assert(a1);

  a2 = power2_allocator_create(128, 16);
  bf_sys_assert(a2);

  c = 1;
  s = 1;
  r = s * c;
  index = power2_allocator_alloc_multiple(a2, s, c);
  bf_sys_assert(index == 127);
  bf_sys_assert(ctz(index) >= log2_uint32_ceil(r));
  power2_allocator_assert(a2);

  c = 2;
  s = 127;
  r = s * c;
  index = power2_allocator_alloc_multiple(a2, s, c);
  bf_sys_assert(index == 0);
  bf_sys_assert(ctz(index) >= log2_uint32_ceil(r));
  power2_allocator_assert(a2);

  c = 1;
  s = 70;
  r = s * c;
  index = power2_allocator_alloc_multiple(a2, s, c);
  bf_sys_assert(index == 256);
  bf_sys_assert(ctz(index) >= log2_uint32_ceil(r));
  power2_allocator_assert(a2);

  power2_allocator_release_multiple(a2, 0, 2);
  power2_allocator_assert(a2);

  c = 1;
  s = 50;
  r = s * c;
  index = power2_allocator_alloc_multiple(a2, s, c);
  bf_sys_assert(index == 64);
  bf_sys_assert(ctz(index) >= log2_uint32_ceil(r));
  power2_allocator_assert(a2);

  c = 1;
  s = 63;
  r = s * c;
  index = power2_allocator_alloc_multiple(a2, s, c);
  bf_sys_assert(index == 0);
  bf_sys_assert(ctz(index) >= log2_uint32_ceil(r));
  power2_allocator_assert(a2);

  power2_allocator_release_multiple(a2, 64, 1);
  power2_allocator_assert(a2);

  power2_allocator_release_multiple(a2, 0, 1);
  power2_allocator_assert(a2);

  c = 2;
  s = 65;
  r = s * c;
  index = power2_allocator_alloc_multiple(a2, s, c);
  bf_sys_assert(index == 0);
  bf_sys_assert(ctz(index) >= log2_uint32_ceil(r));
  power2_allocator_assert(a2);

  power2_allocator_destroy(a1);
  power2_allocator_destroy(a2);
  return 0;
}
