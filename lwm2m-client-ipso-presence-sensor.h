/**
 * @file
 * LightWeightM2M LWM2M Presence Sensor object.
 *
 * @author Imagination Technologies
 *
 * @copyright Copyright (c) 2016, Imagination Technologies Limited and/or its affiliated group
 * companies and/or licensors.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to
 *    endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LWM2M_CLIENT_IPSO_PRESENCE_SENSOR_H_
#define LWM2M_CLIENT_IPSO_PRESENCE_SENSOR_H_

#include "awa/static.h"

/***************************************************************************************************
 * Macros
 **************************************************************************************************/

#define IPSO_PRESENCE_OBJECT                    3302
#define IPSO_DIGITAL_INPUT_STATE                5500
#define IPSO_DIGITAL_INPUT_COUNTER              5501
#define IPSO_DIGITAL_INPUT_COUNTER_RESET        5505
#define IPSO_SENSOR_TYPE                        5751
#define IPSO_BUSY_TO_CLEAR_DELAY                5903
#define IPSO_CLEAR_TO_BUSY_DELAY                5904

#define PRESENCE_SENSORS                        1

/***************************************************************************************************
 * Functions
 **************************************************************************************************/

int DefinePresenceSensorObject(AwaStaticClient *awaClient);
int PresenceSensor_IncrementCounter(AwaStaticClient *awaClient, AwaObjectInstanceID objectInstanceID);

#endif /* LWM2M_CLIENT_IPSO_PRESENCE_SENSOR_H_ */
