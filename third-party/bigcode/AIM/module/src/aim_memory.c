/****************************************************************
 *
 *        Copyright 2013, Big Switch Networks, Inc.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 ****************************************************************/

/******************************************************************************
 *
 *  /module/src/aim_memory.c
 *
 *  AIM Memory Allocation
 *
 *****************************************************************************/
#include <target-utils/AIM/aim_config.h>
#include <target-utils/AIM/aim.h>
#include "aim_int.h"

void *
aim_malloc(size_t size)
{
    void *ptr = AIM_MALLOC(size);
    if (size > 0 && ptr == NULL) {
        AIM_DIE("memory allocation failed");
    }
    return ptr;
}

void *
aim_zmalloc(size_t size)
{
    void *ptr = AIM_CALLOC(1, size);
    if (size > 0 && ptr == NULL) {
        AIM_DIE("memory allocation failed");
    }
    return ptr;
}

void *
aim_realloc(void *ptr, size_t size)
{
    ptr = AIM_REALLOC(ptr, size);
    if (size > 0 && ptr == NULL) {
        AIM_DIE("memory allocation failed");
    }
    return ptr;
}

void *
aim_memdup(void *src, size_t size)
{
    return aim_memndup(src, size, size);
}

void *
aim_memndup(void *src, size_t src_size, size_t alloc_size)
{
    void *rv;
    if (alloc_size < src_size) {
        alloc_size = src_size;
    }
    rv = aim_malloc(alloc_size);
    AIM_MEMCPY(rv, src, src_size);
    return rv;
}

void
aim_free(void *data)
{
    AIM_FREE(data);
}
