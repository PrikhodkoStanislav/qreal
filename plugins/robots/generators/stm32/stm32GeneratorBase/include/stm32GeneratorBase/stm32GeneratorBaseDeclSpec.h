/* Copyright 2007-2015 QReal Research Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#pragma once

#include <QtCore/qglobal.h>

#ifndef ROBOTS_STM32_GENERATOR_BASE_EXPORT
#  if defined(ROBOTS_STM32_GENERATOR_BASE_LIBRARY)
#    define ROBOTS_STM32_GENERATOR_BASE_EXPORT Q_DECL_EXPORT
#  else
#    define ROBOTS_STM32_GENERATOR_BASE_EXPORT Q_DECL_IMPORT
#  endif
#endif