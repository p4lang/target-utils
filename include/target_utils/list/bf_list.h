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
#ifndef _BF_LIST_H_
#define _BF_LIST_H_

//
// Simple double linked list macros.  Head's prev points to tail.  Tail's
// next is NULL.
//

// Prepend.
#define BF_LIST_DLL_PP(l, e, n, p) \
  do {                             \
    bf_sys_assert(e);              \
    (e)->n = l;                    \
    if (l) {                       \
      (e)->p = (l)->p;             \
      (l)->p = e;                  \
    } else {                       \
      (e)->p = e;                  \
    }                              \
    (l) = e;                       \
  } while (0);
// Get last.
#define BF_LIST_DLL_LAST(l, r, n, p) \
  do {                               \
    if (l) {                         \
      r = (l)->p;                    \
    } else {                         \
      r = NULL;                      \
    }                                \
  } while (0);
// Append.
#define BF_LIST_DLL_AP(l, e, n, p) \
  do {                             \
    bf_sys_assert(e);              \
    (e)->n = NULL;                 \
    if (l) {                       \
      (e)->p = (l)->p;             \
      bf_sys_assert((l)->p);       \
      (l)->p->n = e;               \
      (l)->p = e;                  \
    } else {                       \
      (e)->p = (e);                \
      l = e;                       \
    }                              \
  } while (0);
// Remove.
#define BF_LIST_DLL_REM(l, e, n, p) \
  do {                              \
    bf_sys_assert(e);               \
    if ((e)->p == (e)) {            \
      l = NULL;                     \
    } else if ((e) == (l)) {        \
      bf_sys_assert((e)->n);        \
      (e)->n->p = (e)->p;           \
      (l) = (e)->n;                 \
    } else {                        \
      bf_sys_assert((e)->p);        \
      (e)->p->n = (e)->n;           \
      if ((e)->n) {                 \
        (e)->n->p = (e)->p;         \
      } else {                      \
        bf_sys_assert(l);           \
        (l)->p = (e)->p;            \
      }                             \
    }                               \
    (e)->p = NULL;                  \
    (e)->n = NULL;                  \
  } while (0);
// Concatenate
#define BF_LIST_DLL_CAT(f, s, n, p)       \
  do {                                    \
    if ((s) && (f)) {                     \
      void *BF_LIST_DLL_CAT_TMP = (f)->p; \
      bf_sys_assert((f)->p);              \
      (f)->p->n = (s);                    \
      (f)->p = (s)->p;                    \
      (s)->p = BF_LIST_DLL_CAT_TMP;       \
      (s) = NULL;                         \
    } else if ((s)) {                     \
      (f) = (s);                          \
      (s) = NULL;                         \
    }                                     \
  } while (0);

#endif
