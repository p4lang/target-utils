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

/**************************************************************************//**
 *
 *
 *
 *
 *****************************************************************************/
#include <target-utils/AIM/aim_config.h>
#include <target-utils/AIM/aim_pvs_file.h>
#include <target-utils/AIM/aim_string.h>
#include <target-utils/AIM/aim_utils.h>
#include <target-utils/AIM/aim_memory.h>
#include "aim_util.h"

#if AIM_CONFIG_PVS_INCLUDE_TTY == 1
#include <unistd.h>
#endif

AIM_OBJECT_ID_DEFINE(aim_pvs_file_oid, "aim_pvs_file");

/**
 * Destructor for our objects.
 */
static void aim_pvs_file_destroy__(aim_object_t* object);


/**
 * Object subtypes
 */
typedef enum aim_pvs_file_subtypes_e {
    AIM_PVS_FILE_SUBTYPE_NONE = 0,
    AIM_PVS_FILE_SUBTYPE_STDOUT = 1,
    AIM_PVS_FILE_SUBTYPE_STDERR = 2,
    AIM_PVS_FILE_SUBTYPE_FOPEN = 3
} aim_pvs_file_subtype_t;

/**
 * Output to FILE*
 */
static int
aim_vprint_fp__(aim_pvs_t* pvs, const char* fmt, va_list vargs)
{
    int rv = 0;

    /**
     * Special checks for stdout/stderr
     */
    if(pvs->object.cookie == NULL) {
        if(pvs->object.subtype == AIM_PVS_FILE_SUBTYPE_STDOUT) {
            pvs->object.cookie = stdout;
        }
        if(pvs->object.subtype == AIM_PVS_FILE_SUBTYPE_STDERR) {
            pvs->object.cookie = stderr;
        }
    }

    if(pvs->object.cookie) {
        FILE* fp = (FILE*) pvs->object.cookie;
        if(fp) {
            rv = vfprintf(fp, fmt, vargs);
            fflush(fp);
        }
    }
    return rv;
}

/**
 * Create a file PVS.
 */
aim_pvs_t*
aim_pvs_fopen(const char* path, const char* mode)
{
    FILE* fp = fopen(path, mode);
    aim_pvs_t* rv = NULL;

    if(fp) {
        rv = (aim_pvs_t*) aim_object_create(sizeof(*rv),
                                            aim_pvs_file_oid, AIM_PVS_FILE_SUBTYPE_FOPEN,
                                            fp, aim_pvs_file_destroy__);
        rv->vprintf = aim_vprint_fp__;
        rv->enabled = 1;
        rv->description = aim_fstrdup("file:%s", path);
    }
    return rv;
}

/**
 * Close and destroy a file PVS.
 */
static void
aim_pvs_file_destroy__(aim_object_t* object)
{
    aim_pvs_t* pvs = (aim_pvs_t*)object;

    /*
     * We should only destroy subtype FOPEN.
     * The others are static and permanent.
     */
    if(object->subtype == AIM_PVS_FILE_SUBTYPE_FOPEN) {
        if(object->cookie) {
            FILE* fp = (FILE*)object->cookie;
            fclose(fp);
        }
        if(pvs->description) {
            aim_free(pvs->description);
        }
        object = NULL;
    }
}


static int
isatty__(aim_pvs_t* pvs)
{
#if AIM_CONFIG_PVS_INCLUDE_TTY == 1
    if(pvs->object.subtype == AIM_PVS_FILE_SUBTYPE_STDOUT) {
        return isatty(1);
    }
    if(pvs->object.subtype == AIM_PVS_FILE_SUBTYPE_STDERR) {
        return isatty(2);
    }
#endif
    return 0;
}

/**
 * Standard builtin types are based on the FILE pvs.
 */
aim_pvs_t aim_pvs_stdout = {
    AIM_STATIC_OBJECT_INIT(aim_pvs_file_oid, AIM_PVS_FILE_SUBTYPE_STDOUT,
                           NULL, NULL),
    "{stdout}",
    aim_vprint_fp__,
    1,
    0,
    isatty__,
};

aim_pvs_t aim_pvs_stderr = {
    AIM_STATIC_OBJECT_INIT(aim_pvs_file_oid, AIM_PVS_FILE_SUBTYPE_STDERR,
                           NULL, NULL),
    "{stderr}",
    aim_vprint_fp__,
    1,
    0,
    isatty__,
};

aim_pvs_t aim_pvs_none = {
    AIM_STATIC_OBJECT_INIT(aim_pvs_file_oid, AIM_PVS_FILE_SUBTYPE_NONE,
                           NULL, NULL),
    "{none}",
    aim_vprint_fp__,
    1,
    0
};

aim_pvs_t aim_pvs_fp = {
    AIM_STATIC_OBJECT_INIT(aim_pvs_file_oid, AIM_PVS_FILE_SUBTYPE_FOPEN,
                           NULL, NULL),
    "{none}",
    aim_vprint_fp__,
    1,
    0,
    isatty__,
};

