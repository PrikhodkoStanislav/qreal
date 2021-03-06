/* Copyright 2016 CyberTech Labs Ltd.
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

#include "ev3Kit/robotModel/parts/ev3Gyroscope.h"

using namespace ev3::robotModel::parts;
using namespace kitBase::robotModel;

Ev3Gyroscope::Ev3Gyroscope(const DeviceInfo &info, const PortInfo &port)
	: kitBase::robotModel::robotParts::ScalarSensor(info, port)
{
}
