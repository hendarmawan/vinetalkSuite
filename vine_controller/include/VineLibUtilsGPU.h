/*
 * Copyright 2018 Foundation for Research and Technology - Hellas
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0 [1] [1]
 *
 * Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *  implied.
 * See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * Links:
 *  ------
 * [1] http://www.apache.org/licenses/LICENSE-2.0 [1] 
*/
#ifndef VINE_LIB_GPU_UTILS_HEADER
#define VINE_LIB_GPU_UTILS_HEADER

#include <vector>
#include "vine_pipe.h"
#include "VineLibMgr.h"
#ifdef LIBRARY_BUILD
extern bool Host2GPU(vine_task_msg_s *vine_task, std::vector<void *> &ioHD);

/* Cuda Memcpy from Device to host*/
extern bool GPU2Host(vine_task_msg_s *vine_task, std::vector<void *> &ioDH);

/* Free Device memory */
extern bool GPUMemFree(std::vector<void *> &io);
#endif
#endif
