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
#include <target-utils/fbitset/fbitset.h>
#include <Judy.h>
#include <assert.h>

/* Initialize a bitset. */
void bf_fbs_init(bf_fbitset_t *bs, unsigned int width) {
  assert(bs);
  assert(width > 0);
  bs->width = width;
  bs->fbs = NULL;
}

/* Set the bit at "position" to "val", return it's previous value. */
bool bf_fbs_set(bf_fbitset_t *bs, int position, int val) {
  assert(bs);
  assert(position >= 0);
  assert((unsigned)position < bs->width);
  assert((val == 1) || (val == 0));

  int Rc_int;
  if (val) {
    J1S(Rc_int, bs->fbs, (Word_t)position);
    return (Rc_int ? false : true);
  } else {
    J1U(Rc_int, bs->fbs, (Word_t)position);
    return (Rc_int ? true : false);
  }
}

/* Return the value of the bit at "position". */
bool bf_fbs_get(bf_fbitset_t *bs, int position) {
  assert(bs);
  assert(position >= 0);
  assert((unsigned)position < bs->width);
  int Rc_int;
  J1T(Rc_int, bs->fbs, (Word_t)position);
  return (Rc_int ? true : false);
}

/* Position is exclusive */
int bf_fbs_prev_clr_contiguous(bf_fbitset_t *bs,
                               int position,
                               unsigned int count) {
  if (position <= 0) {
    return -1;
  }
  position--;
  assert(bs);
  assert(position >= 0);
  assert((unsigned)position < bs->width);
  assert(count);

  if (((unsigned)(position + 1) < count) || ((unsigned)position >= bs->width)) {
    return -1;
  }

  int Rc_int;
  Word_t free_index = position;
  J1LE(Rc_int, bs->fbs, free_index);
  if (!Rc_int) {
    return -1;
  }

  if (count == 1) {
    return free_index;
  }

  Word_t last_free = free_index;
  while (true) {
    J1P(Rc_int, bs->fbs, last_free);
    if (!Rc_int) {
      last_free = 0;
    } else {
      last_free++;
    }

    if (((free_index + 1) - last_free) >= count) {
      return ((free_index + 1) - count);
    }
    J1PE(Rc_int, bs->fbs, last_free);
    if (!Rc_int) {
      return -1;
    }
    free_index = last_free;
  }

  return -1;
}

/* Position is exclusive */
int bf_fbs_first_clr_contiguous(bf_fbitset_t *bs,
                                int position,
                                unsigned int count) {
  position++;
  assert(bs);
  assert(count);
  if (position < 0 || ((unsigned)(position + count) > bs->width)) {
    return -1;
  }
  assert(position >= 0);
  assert((unsigned)(position + count) <= bs->width);

  int Rc_int;
  Word_t free_index = position;
  J1FE(Rc_int, bs->fbs, free_index);
  if (!Rc_int) {
    return -1;
  }

  if (free_index >= bs->width) {
    return -1;
  }

  if (count == 1) {
    return free_index;
  }

  Word_t last_free = free_index;
  while (last_free < bs->width) {
    J1N(Rc_int, bs->fbs, last_free);
    if (!Rc_int) {
      last_free = bs->width - 1;
    } else {
      last_free--;
    }

    if (((last_free + 1) - free_index) >= count) {
      return free_index;
    }
    J1NE(Rc_int, bs->fbs, last_free);
    if (!Rc_int) {
      return -1;
    }
    free_index = last_free;
  }

  return -1;
}

/* Position is exclusive */
int bf_fbs_first_set(bf_fbitset_t *bs, int position) {
  position++;
  assert(bs);
  if (position < 0 || ((unsigned)position > bs->width)) {
    return -1;
  }
  int Rc_int;
  Word_t set_index = position;
  J1F(Rc_int, bs->fbs, set_index);
  if (!Rc_int) {
    return -1;
  }
  return set_index;
}

void bf_fbs_destroy(bf_fbitset_t *bs) {
  Word_t Rc_word;
  (void)Rc_word;
  J1FA(Rc_word, bs->fbs);
}
