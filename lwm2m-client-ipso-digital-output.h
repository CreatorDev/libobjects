/**
 * @file
 * LightWeightM2M LWM2M Digital Output object.
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

#ifndef LWM2M_CLIENT_IPSO_DIGITAL_OUTPUT_H_
#define LWM2M_CLIENT_IPSO_DIGITAL_OUTPUT_H_

#include "awa/static.h"
#include "awa/types.h"

/***************************************************************************************************
* Macros
**************************************************************************************************/

#define IPSO_DIGITAL_OUTPUT_OBJECT               3201
#define IPSO_DIGITAL_OUTPUT_STATE                5550
#define IPSO_DIGITAL_OUTPUT_POLARITY             5551
#define IPSO_DIGITAL_OUTPUT_APPLICATION_TYPE     5750

//constants for callback, informing which value (state/polarity) was changed
#define DIGITAL_OUTPUT_CALLBACK_TYPE_STATE      0
#define DIGITAL_OUTPUT_CALLBACK_TYPE_POLARITY   1

#define DIGITAL_OUTPUTS                          2

/***************************************************************************************************
* Functions
**************************************************************************************************/
typedef void(*DigitalOutputCallback)(int type, int instanceId, int state);

AwaError DigitalOutput_DefineObject(AwaStaticClient *awaClient, DigitalOutputCallback callback);

AwaBoolean DigitalOutput_GetState(int instanceId);

#endif /* LWM2M_CLIENT_IPSO_DIGITAL_OUTPUT_H_ */
