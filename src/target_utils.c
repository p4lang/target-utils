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

#ifndef TARGET_UTILS_BLD_VER
#define TARGET_UTILS_BLD_VER "0"
#endif

#ifndef TARGET_UTILS_GIT_VER
#define TARGET_UTILS_GIT_VER "000"
#endif

#define TARGET_UTILS_REL_VER "9.8.0"
#define TARGET_UTILS_VER TARGET_UTILS_REL_VER "-" TARGET_UTILS_BLD_VER

#define TARGET_UTILS_INTERNAL_VER TARGET_UTILS_VER "(" TARGET_UTILS_GIT_VER ")"

const char *target_utils_get_version(void) { return TARGET_UTILS_VER; }
const char *target_utils_get_internal_version(void) {
  return TARGET_UTILS_INTERNAL_VER;
}
