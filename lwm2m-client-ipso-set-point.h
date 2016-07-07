/**
 * @file
 * LightWeightM2M LWM2M Set Point object.
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

#ifndef LWM2M_CLIENT_IPSO_SET_POINT_H_
#define LWM2M_CLIENT_IPSO_SET_POINT_H_

#include "awa/static.h"

/***************************************************************************************************
 * Macros
 **************************************************************************************************/

#define IPSO_SET_POINT_OBJECT                             3308
#define IPSO_SET_POINT_VALUE                              5900
#define IPSO_SET_POINT_COLOUR                             5706
#define IPSO_SET_POINT_UNITS                              5701
#define IPSO_SET_POINT_APPLICATION_TYPE                   5750

/***************************************************************************************************
 * Typedefs
 **************************************************************************************************/
typedef void(*SetPointCallback)(AwaFloat newValue);

/***************************************************************************************************
 * Functions
 **************************************************************************************************/
 /**
  * @brief Define single IPSO Set Point object (with ID 3008) and resources bound to it (excluding Colour).
  * @param awaClient Reference to AWA client handle
  * @param valueChangeCallback callback which will be triggered when new value is recieved from outside
  * @return If operation was sucessful then AwaError_Success is returned, to map other values please refer to AwaError type.
  */
AwaError SetPointObject_DefineObjectsAndResources(AwaStaticClient *awaClient, SetPointCallback valueChangeCallback);

/**
 * @brief Sets new value for "Set point" object, and sends notification about change to server. Call to this method does NOT trigger valueChangeCallback.
 * @param awaClient Reference to AWA client handle
 * @param value new value to be set
 * @return If operation was sucessful then AwaError_Success is returned, to map other values please refer to AwaError type.
 */
AwaError SetPointObject_SetValue(AwaStaticClient *awaClient, AwaFloat value);

 /**
  * @brief Reads current value of "Set Point" object resource "Value".
  * @return Currently stored value in object.
  */
 AwaFloat SetPointObject_GetValue();

#endif /* LWM2M_CLIENT_IPSO_SET_POINT_H_ */
