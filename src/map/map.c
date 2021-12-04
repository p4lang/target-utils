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
#include <target-utils/map/map.h>
#include "map_log.h"
#include <Judy.h>
#include <target-sys/bf_sal/bf_sys_intf.h>

bf_map_sts_t bf_map_init(bf_map_t *map) {
  *map = NULL;
  return BF_MAP_OK;
}

bf_map_sts_t bf_map_add(bf_map_t *map, unsigned long key, void *data) {
  PWord_t Pvalue;
  JLI(Pvalue, (*map), key);
  if (PJERR == Pvalue) {
    bf_sys_dbgchk((PJERR != Pvalue));
    return BF_MAP_ERR;
  } else if (0 != *Pvalue) {
    return BF_MAP_KEY_EXISTS;
  }
  *Pvalue = (Word_t)data;
  return BF_MAP_OK;
}

bf_map_sts_t bf_map_rmv(bf_map_t *map, unsigned long key) {
  int Rc_int = 0;
  JLD(Rc_int, (*map), key);
  if (JERR == Rc_int) {
    bf_sys_dbgchk(JERR != Rc_int);
    return BF_MAP_ERR;
  } else if (0 == Rc_int) {
    return BF_MAP_NO_KEY;
  }
  return BF_MAP_OK;
}

bf_map_sts_t bf_map_get(bf_map_t *map, unsigned long key, void **data) {
  PWord_t Pvalue;
  JLG(Pvalue, (*map), key);
  if (PJERR == Pvalue) {
    bf_sys_dbgchk(PJERR != Pvalue);
    return BF_MAP_ERR;
  } else if (NULL == Pvalue) {
    return BF_MAP_NO_KEY;
  }
  *data = (void *)(*Pvalue);
  return BF_MAP_OK;
}

bf_map_sts_t bf_map_get_rmv(bf_map_t *map, unsigned long key, void **data) {
  bf_map_sts_t sts = BF_MAP_OK;

  sts = bf_map_get(map, key, data);
  if (BF_MAP_OK == sts) {
    sts = bf_map_rmv(map, key);
  }
  return sts;
}

/* Gets the first data item from the map and sets the key to the index
 * where the data item was found.
 */

bf_map_sts_t bf_map_get_first(bf_map_t *map, unsigned long *key, void **data) {
  PWord_t Pvalue;
  unsigned long loc_key = 0;

  JLF(Pvalue, (*map), loc_key);

  if (PJERR == Pvalue) {
    bf_sys_dbgchk(PJERR != Pvalue);
    return BF_MAP_ERR;
  } else if (Pvalue == NULL) {
    return BF_MAP_NO_KEY;
  }

  *data = (void *)(*Pvalue);
  *key = loc_key;

  return BF_MAP_OK;
}

/* Gets the first data item in the map, and removes it from the map */

bf_map_sts_t bf_map_get_first_rmv(bf_map_t *map,
                                  unsigned long *key,
                                  void **data) {
  bf_map_sts_t sts = BF_MAP_OK;

  sts = bf_map_get_first(map, key, data);
  if (BF_MAP_OK == sts) {
    sts = bf_map_rmv(map, *key);
  }
  return sts;
}

/* Gets the next data item from the map from the passed in key and fills in
 * the key where the next data item was found.
 */

bf_map_sts_t bf_map_get_next(bf_map_t *map, unsigned long *key, void **data) {
  PWord_t Pvalue;
  unsigned long loc_key = *key;

  JLN(Pvalue, (*map), loc_key);

  if (PJERR == Pvalue) {
    bf_sys_dbgchk(PJERR != Pvalue);
    return BF_MAP_ERR;
  } else if (Pvalue == NULL) {
    return BF_MAP_NO_KEY;
  }

  *data = (void *)(*Pvalue);
  *key = loc_key;

  return BF_MAP_OK;
}

void bf_map_destroy(bf_map_t *map) {
  int Rc_word = 0;

  JLFA(Rc_word, (*map));
  (void)Rc_word;
  *map = NULL;
}

uint32_t bf_map_count(bf_map_t *map) {
  Word_t count;
  JLC(count, (*map), 0, -1);
  return (uint32_t)count;
}
