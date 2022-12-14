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
//  bf_id.c
//
//
//  Created on 6/20/14.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <target-sys/bf_sal/bf_sys_intf.h>
#include <target-utils/id/id.h>
#include <Judy.h>

//#define BF_ID_ALLOCATOR_TEST 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct bf_id_allocator_int {
  Pvoid_t PJ1Array;
  uint32_t size;
  bool zero_based;
} bf_id_allocator_int;

/**
Create the ID allocator
@param initial_size the initial size of allocator
*/
bf_id_allocator *bf_id_allocator_new(unsigned int initial_size,
                                     bool zero_based) {
  bf_id_allocator_int *allocator =
      (bf_id_allocator_int *)bf_sys_malloc(sizeof(bf_id_allocator_int) * 1);
  allocator->zero_based = zero_based;
  allocator->size = initial_size;
  /* Initialize the Judy array */
  allocator->PJ1Array = (Pvoid_t)NULL;

  return (bf_id_allocator)allocator;
}
/**
Delete the ID allocator region
@param allocator allocator allocated with create
*/
void bf_id_allocator_destroy(bf_id_allocator *allocator) {
  Word_t Rc_word;
  /* Free the Judy array */
  J1FA(Rc_word, ((bf_id_allocator_int *)allocator)->PJ1Array);
  (void)Rc_word;
  bf_sys_free(allocator);
}

/**
Allocate count contiguous ids (max 32)
@param allocator allocator created with create
@param count number of contiguous ids to allocate
*/
int bf_id_allocator_allocate_contiguous(bf_id_allocator *a, uint8_t count) {
  bf_id_allocator_int *allocator = (bf_id_allocator_int *)a;
  unsigned int i;
  int Rc_int;
  Word_t first_empty = 0;
  Word_t next_set = 0;
  Word_t Index = 0;

  bf_sys_assert(allocator != NULL);

  Index = 0;

  /* Get the first empty slot : Thanks Judy :-) */
  if (Index == 0) {
    J1FE(Rc_int, allocator->PJ1Array, Index);
  } else {
    J1NE(Rc_int, allocator->PJ1Array, Index);
  }

  if (Rc_int == 0) {
    return -1;
  }

  if (Index >= allocator->size) {
    /* If free index returned exceeds the size of the allocator,
     * then need to wrap around.
     */

    Index = 0;
    J1FE(Rc_int, allocator->PJ1Array, Index);
  }

  first_empty = Index;

  if (first_empty >= allocator->size) {
    /* Nothing available */
    return -1;
  }

  if (count == 1) {
    J1S(Rc_int, allocator->PJ1Array, first_empty);

    if (allocator->zero_based == true) {
      return first_empty;
    }

    return first_empty + 1;
  }

  /* Get next bit set */
  J1N(Rc_int, allocator->PJ1Array, Index);

  if (Rc_int == 0) {
    /* No set Bits, which means the Judy array is empty from the
     * "first_empty" position.
     */
    next_set = allocator->size;
  } else {
    next_set = Index;
  }

  while (next_set - first_empty < count) {
    Index = next_set;
    J1FE(Rc_int, allocator->PJ1Array, Index);

    if (Rc_int == 0) {
      return -1;
    }
    first_empty = Index;

    J1N(Rc_int, allocator->PJ1Array, Index);

    if (Rc_int == 0) {
      /* Check if there are enough */
      if (allocator->size - Index < count) {
        return -1;
      }
    } else {
      next_set = Index;
    }
  }

  if (first_empty > allocator->size) {
    return -1;
  }

  /* Set the bit(s) now in the Judy array */
  for (i = 0; i < count; i++) {
    J1S(Rc_int, allocator->PJ1Array, first_empty + i);
  }

  if (allocator->zero_based == true) {
    return first_empty;
  }

  return (first_empty + 1);
}

/**
Allocate an id
@param allocator allocator created with create
*/
unsigned int bf_id_allocator_allocate(bf_id_allocator *allocator) {
  return bf_id_allocator_allocate_contiguous(allocator, 1);
}

/**
Free an allocated id
@param allocator allocator created with create
@param id id to be freed up
*/
void bf_id_allocator_release(bf_id_allocator *a, unsigned int id) {
  bf_id_allocator_int *allocator = (bf_id_allocator_int *)a;
  int Rc_int;

  bf_sys_assert(allocator != NULL);

  if (allocator->zero_based != true) {
    bf_sys_assert(id > 0);
    id = id - 1;
  }

  /* JUdy Unset */
  J1U(Rc_int, allocator->PJ1Array, id);

  if (Rc_int == 1) {
    /* Get around set but not used compile error :D
     * CRINGE.
     */
  }
}

/**
Set an allocated id
@param allocator allocator created with create
@param id id to set
*/
void bf_id_allocator_set(bf_id_allocator *a, unsigned int id) {
  bf_id_allocator_int *allocator = (bf_id_allocator_int *)a;
  int Rc_int;

  bf_sys_assert(allocator != NULL);

  if (allocator->zero_based != true) {
    /* For non-zero based allocator, an id of 0 is forbidden */
    bf_sys_assert(id > 0);
    id = id - 1;
  }

  /* Judy Set */
  J1S(Rc_int, allocator->PJ1Array, id);

  if (Rc_int == 1) {
    /* Get around set but not used compile error :D
     * CRINGE.
     */
  }
}

/**
  Checks if an id is allocated or not
  @param allocator allocator created with create
  @param id id to check
  */
int bf_id_allocator_is_set(bf_id_allocator *a, unsigned int id) {
  bf_id_allocator_int *allocator = (bf_id_allocator_int *)a;
  int Rc_int;

  bf_sys_assert(allocator != NULL);
  if (allocator->zero_based != true) {
    if (id < 1) {
      return 0;
    }
    id = id - 1;
  }
  J1T(Rc_int, allocator->PJ1Array, id);

  return (Rc_int == 1 ? 1 : 0);
}

/**
  Finds first allocated id
  */
int bf_id_allocator_get_first(bf_id_allocator *a) {
  bf_id_allocator_int *allocator = (bf_id_allocator_int *)a;
  int Rc_int;
  Word_t wd_id = 0;
  unsigned int id = 0;

  if (allocator == NULL) {
    return -1;
  }
  bf_sys_assert(allocator != NULL);
  J1F(Rc_int, allocator->PJ1Array, wd_id);
  if (Rc_int) {
    if (allocator->zero_based != true) {
      id = wd_id + 1;
    }
  } else {
    id = -1;
  }

  return id;
}

/**
  Finds next allocated id
  */
int bf_id_allocator_get_next(bf_id_allocator *a, unsigned int id) {
  bf_id_allocator_int *allocator = (bf_id_allocator_int *)a;
  int Rc_int;
  Word_t curr_id = id;
  unsigned int id_next = 0;

  if (allocator == NULL) {
    return -1;
  }
  bf_sys_assert(allocator != NULL);
  if (allocator->zero_based != true) {
    if (curr_id > 0) {
      curr_id = curr_id - 1;
    } else {
      return -1;
    }
  }
  J1N(Rc_int, allocator->PJ1Array, curr_id);
  if (Rc_int) {
    if (allocator->zero_based != true) {
      id_next = curr_id + 1;
    } else {
      id_next = curr_id;
    }
  } else {
    id_next = -1;
  }

  return id_next;
}

/**
Copy an id-allocator from a src allocator to a dst allocator
@param src_allocator Source allocator
@param dst_allocator Destination allocator
*/
void bf_id_allocator_copy(bf_id_allocator *dst_allocator,
                          bf_id_allocator *src_allocator) {
  (void)dst_allocator;
  (void)src_allocator;
  return;
}

#ifdef BF_ID_ALLOCATOR_TEST

#define MAX_ID_TEST (16 * 1024)
#define INITIAL_WORDS 4

int id_main(int argc, char **argv) {
  unsigned int i;
  unsigned int iter;
  bf_id_allocator *allocator = bf_id_allocator_new(INITIAL_WORDS * 32);

  for (i = 0; i < 40; i++) bf_id_allocator_allocate(allocator);

  printf("words are 0x%x, 0x%x\n",
         (uint32_t)allocator->data[0],
         (uint32_t)allocator->data[1]);

  for (i = 0; i < 40; i++) bf_id_allocator_release(allocator, i + 1);

  for (i = 0; i < MAX_ID_TEST; i++) {
    unsigned int id = bf_id_allocator_allocate(allocator);
    bf_sys_assert(id == i + 1);
  }

  for (i = 0; i < MAX_ID_TEST; i++) bf_id_allocator_release(allocator, i);

  for (iter = 0; iter < MAX_ID_TEST; iter++) {
    for (i = 0; i < 1000; i++) bf_id_allocator_allocate(allocator);

    for (i = 0; i < 1000; i++) bf_id_allocator_release(allocator, i);
  }

#define NUM_BLOCKS 20
#define BLOCK_SIZE 8
  for (i = 0; i < NUM_BLOCKS; i++) {
    unsigned int id =
        bf_id_allocator_allocate_contiguous(allocator, BLOCK_SIZE);
    printf("id = %d 0x%x\n", id, (uint32_t)allocator->data[i / 4]);
  }
  for (i = 1; i <= NUM_BLOCKS * BLOCK_SIZE; i++) {
    bf_id_allocator_release(allocator, i);
  }

  bf_id_allocator_destroy(allocator);
  return 0;
}

#endif

#ifdef __cplusplus
}
#endif
