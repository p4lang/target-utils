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
/**************************************************************************/ /**
  *
  *
  *
  *****************************************************************************/
#ifndef __MAP_LOG_H__
#define __MAP_LOG_H__

#include <target-sys/bf_sal/bf_sys_intf.h>

#define LOG_ERROR(...) \
  bf_sys_log_and_trace(BF_MOD_UTIL, BF_LOG_ERR, __VA_ARGS__)

#endif /* __MAP_LOG_H__ */
