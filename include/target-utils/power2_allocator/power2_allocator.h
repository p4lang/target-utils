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
#ifndef _POWER2_ALLOCATOR_H_
#define _POWER2_ALLOCATOR_H_

typedef struct power2_allocator_s {
  uint32_t max_size;  // Maximum size of a block
  uint32_t total_size;
  uint32_t no_free_lists;  // log2(max_size) + 1
  /* Free list is an array indexed by the log2 of the free size.
   * Each array element is a Judy list of free indexes which align to that
   * log2 size. The key is the index and value stores the free size starting
   * at that index.
   * If 0-4 (size 5) is free at index 0, then it'll be in the free_lists[3]
   * with index 0 and size 5.
   * If 128-129 are free, then it'll be in the free_lists[1] with index 128
   * and size 2
   *   Subsequently if 130 becomes free, then the original entry in
   * free_lists[1]
   *   will be removed and an entry in free_lists[2] will be added with
   *   index 128 and size 3
   */
  void **free_lists;
  /* Inuse list stores the size in use for a given index.
   * Used in release path to figure out the size to free
   */
  void *inuse_list;
} power2_allocator_t;

/** \brief power2_allocator_create
  *        Create a power2 allocator and initialize count number of
  *        size entries
  *
  * \param size Default size of the resource. This should be a power of 2.
  *             Requests to allocate indexes with size > default size is an
  *error
  * \param count The number of "size" resources available.
  * \return Returns the pointer to an allocator to use for future calls.
  *         Null in case of failure
  */
power2_allocator_t *power2_allocator_create(uint32_t size, uint32_t count);

/** \brief power2_allocator_destroy
  *        Destroy the state allocated for power2_allocator
  *
  * \param allocator The pointer to the power2 allocator returned by
  *power2_allocator_create
  * \return void
  */
void power2_allocator_destroy(power2_allocator_t *allocator);

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
int power2_allocator_alloc(power2_allocator_t *allocator, uint32_t size);

/** \brief power2_allocator_release
  *        Release the resource that was allocated at given index
  *
  * \param allocator The power2 allocator
  * \param index Index to release
  * \return Status of the operation. 0 for SUCCESS.
  *         non-zero in case of errors
  */
int power2_allocator_release(power2_allocator_t *allocator, uint32_t index);

/** \brief power2_allocator_get_index_size
  *         Given an index, return the size allocated at that index.
  *
  * \param allocator The power2 allocator
  * \param index The index to query
  * \return The size of the allocation, -1 if the index is not valid.
  */
uint32_t power2_allocator_get_index_size(power2_allocator_t *allocator,
                                         uint32_t alloc_index);

/** \brief power2_allocator_reserve
  *         Reserve a block of indexes as inuse in the power2 allocator
  *
  * \param allocator The power2 allocator
  * \param index The index to reserve
  * \param size The size to reserve
  * \return Status of the operation.
  *     0 in case of success.
  *     1 in case any indexes within the index, index+size are already in use
  *     -1 in case of any other errors
  */
int power2_allocator_reserve(power2_allocator_t *allocator,
                             uint32_t reserve_index,
                             uint32_t reserve_size);

/** \brief power2_allocator_make_copy
  *         Make a copy of the allocator and return the copy
  *
  * \param src The pointer to the power2 allocator that needs to be copied
  * \return Returns pointer to an allocator that has same state has the passed
  *in
  *          allocator.
  *         Null in case of failure
  */
power2_allocator_t *power2_allocator_make_copy(power2_allocator_t *src);

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
                                    uint32_t count);

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
                                      uint32_t count);

/** \brief power2_allocator_alloc_count_by_size
  *        Get the number of allocated elements of a given size.
  *
  * \param allocator The power2 allocator.
  * \param size The size of the element.
  * \return The number of elements allocated of the specified size.
  */
int power2_allocator_alloc_count_by_size(power2_allocator_t *allocator,
                                         uint32_t size);

int power2_allocator_usage(power2_allocator_t *allocator);

uint32_t power2_allocator_alloc_count(power2_allocator_t *allocator);

int power2_allocator_first_alloc(power2_allocator_t *allocator);
int power2_allocator_next_alloc(power2_allocator_t *allocator, int idx);

int power2_alloc_utest(void);

#endif  // _POWER2_ALLOCATOR_H_
