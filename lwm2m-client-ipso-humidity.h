/**
 * @file
 * LightWeightM2M LWM2M humidITY object.
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

#ifndef LWM2M_CLIENT_IPSO_HUMIDITY_H_
#define LWM2M_CLIENT_IPSO_HUMIDITY_H_

#include "awa/static.h"

/***************************************************************************************************
 * Macros
 **************************************************************************************************/

#define IPSO_HUMIDITY_OBJECT                             3304
#define IPSO_HUMIDITY_SENSOR_VALUE                       5700
#define IPSO_HUMIDITY_UNITS                              5701
#define IPSO_HUMIDITY_MIN_MEASURED_VALUE                 5601
#define IPSO_HUMIDITY_MAX_MEASURED_VALUE                 5602
#define IPSO_HUMIDITY_MIN_RANGE_VALUE                    5603
#define IPSO_HUMIDITY_MAX_RANGE_VALUE                    5604
#define IPSO_HUMIDITY_RESET_MIN_AND_MAX_MEASURED_VALUES  5605


/***************************************************************************************************
 * Functions
 **************************************************************************************************/

/**
 * @brief Define single IPSO Humidity object (with ID 3004) and all resources bound to it. It also takes min and max measurable humidities as a parameters.
 *  Range defined in this way is hardware specific and should be choosen from tech specification of used device.
 * @param awaClient Reference to AWA client handle
 * @param minRange Minimal humidity which can be measured and represented by this object
 * @param maxRange Maximal humidity which can be measured and represented by this object
 * @return If operation was sucessful then AwaError_Success is returned, to map other values please refer to AwaError type.
 */
AwaError HumidityObject_DefineObjectsAndResources(AwaStaticClient *awaClient, AwaFloat minRange, AwaFloat maxRange);

/**
 * @brief Updates current value of humidity. Call to this method updates also Min/Max measured values.
 * @param awaClient Reference to AWA client handle
 * @param humidity New value for humidity resource (Sensor value)
 * @return If operation was sucessful then AwaError_Success is returned, to map other values please refer to AwaError type.
 */
AwaError HumidityObject_SetHumidity(AwaStaticClient *awaClient, AwaFloat humidity);

#endif /* LWM2M_CLIENT_IPSO_humidITY_H_ */
