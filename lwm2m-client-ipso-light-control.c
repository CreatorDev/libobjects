/**
 * @file
 * LightWeightM2M LWM2M Light Control object.
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

/***************************************************************************************************
 * Includes
 **************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "lwm2m_core.h"
#include "coap_abstraction.h"
#include "lwm2m-client-ipso-light-control.h"

/***************************************************************************************************
 * Definitions
 **************************************************************************************************/

#define IPSO_LIGHT_CONTROL_OBJECT						3311
#define IPSO_LIGHT_CONTROL_ON_OFF						5850
#define IPSO_LIGHT_CONTROL_DIMMER						5851
#define IPSO_LIGHT_CONTROL_COLOUR						5706
#define IPSO_LIGHT_CONTROL_UNITS						5701
#define IPSO_LIGHT_CONTROL_ON_TIME						5852
#define IPSO_LIGHT_CONTROL_CUMULATIVE_ACTIVE_POWER		5805
#define IPSO_LIGHT_CONTROL_POWER_FACTOR					5820

#define LIGHT_CONTROLS									2

#define MAX_STR_SIZE									64

#define REGISTER_LIGHT_CONTROL_RESOURCE(context, name, id, type, minInstances, operations) \
	REGISTER_RESOURCE(context, name, IPSO_LIGHT_CONTROL_OBJECT, id, type, \
		MultipleInstancesEnum_Single, minInstances, operations, \
		&LightControlResourceOperationHandlers)

#define CREATE_LIGHT_CONTROL_OPTIONAL_RESOURCE(context, objectInstanceId, resourcId) \
	CREATE_OPTIONAL_RESOURCE(context, IPSO_LIGHT_CONTROL_OBJECT, objectInstanceId, resourcId)

/***************************************************************************************************
 * Typedefs
 **************************************************************************************************/

typedef struct
{
	bool OnOff;
	int64_t Dimmer;
	char Colour[MAX_STR_SIZE];
	char Units[MAX_STR_SIZE];
	int64_t OnTime;
	float CumulativeActivePower;
	float PowerFactor;
	LightControlCallBack callback;
	void * context;
} IPSOLightControl;

/***************************************************************************************************
 * Prototypes
 **************************************************************************************************/

static int LightControl_ResourceReadHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID, uint8_t * destBuffer, int destBufferLen);

static int LightControl_ResourceGetLengthHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID);

static int LightControl_ResourceWriteHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID, uint8_t * srcBuffer, int srcBufferLen,
	bool * changed);


static int LightControl_ResourceCreateHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID);

static int LightControl_ObjectCreateInstanceHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID);

static int LightControl_ObjectDeleteHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID);

/***************************************************************************************************
 * Globals
 **************************************************************************************************/

static ObjectOperationHandlers LightControlObjectOperationHandlers =
{
	.CreateInstance = LightControl_ObjectCreateInstanceHandler,
	.Delete = LightControl_ObjectDeleteHandler,
};

static ResourceOperationHandlers LightControlResourceOperationHandlers =
{
	.Read = LightControl_ResourceReadHandler,
	.GetLength = LightControl_ResourceGetLengthHandler,
	.Write = LightControl_ResourceWriteHandler,
	.CreateOptionalResource = LightControl_ResourceCreateHandler,
	.Execute = NULL,
};

static IPSOLightControl LightControls[LIGHT_CONTROLS];

/***************************************************************************************************
 * Implementation
 **************************************************************************************************/

static int LightControl_ObjectCreateInstanceHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID)
{
	if(objectInstanceID >= LIGHT_CONTROLS)
	{
		Lwm2m_Error("LightControl_ResourceCreateHandler instance number %d out of range (max %d)",
			objectInstanceID, LIGHT_CONTROLS - 1);
		return -1;
	}

	return objectInstanceID;
}

static int LightControl_ResourceCreateHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID)
{
	return 0;
}

static int LightControl_ObjectDeleteHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID)
{
	if (objectID != IPSO_LIGHT_CONTROL_OBJECT)
	{
		Lwm2m_Error("LightControl_ObjectDeleteHandler Invalid OIR: %d/%d/%d\n", objectID,
			objectInstanceID, resourceID);
		return -1;
	}

	if(objectInstanceID >= LIGHT_CONTROLS)
	{
		Lwm2m_Error("LightControl_ObjectDeleteHandler instance number %d out of range (max %d)",
			objectInstanceID, LIGHT_CONTROLS - 1);
		return -1;
	}

	if (resourceID == -1)
	{
		memset(&LightControls[objectInstanceID], 0, sizeof(IPSOLightControl));
	}
	else
	{
		//TODO
	}

	return 0;
}

static int LightControl_ResourceReadHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID, uint8_t * destBuffer, int destBufferLen)
{
	int result = 0;

	switch (resourceID)
	{
		case IPSO_LIGHT_CONTROL_ON_OFF:
			result = sizeof(LightControls[objectInstanceID].OnOff);
			memcpy(destBuffer, &LightControls[objectInstanceID].OnOff, result);
			break;

		case IPSO_LIGHT_CONTROL_DIMMER:
			result = sizeof(LightControls[objectInstanceID].Dimmer);
			memcpy(destBuffer, &LightControls[objectInstanceID].Dimmer, result);
			break;

		case IPSO_LIGHT_CONTROL_COLOUR:
			result = strlen(LightControls[objectInstanceID].Colour) + 1;
			memcpy(destBuffer, LightControls[objectInstanceID].Colour, result);
			break;

		case IPSO_LIGHT_CONTROL_UNITS:
			result = strlen(LightControls[objectInstanceID].Units) + 1;
			memcpy(destBuffer, LightControls[objectInstanceID].Units, result);
			break;

		case IPSO_LIGHT_CONTROL_ON_TIME:
			result = sizeof(LightControls[objectInstanceID].OnTime);
			memcpy(destBuffer, &LightControls[objectInstanceID].OnTime, result);
			break;

		case IPSO_LIGHT_CONTROL_CUMULATIVE_ACTIVE_POWER:
			result = sizeof(LightControls[objectInstanceID].CumulativeActivePower);
			memcpy(destBuffer, &LightControls[objectInstanceID].CumulativeActivePower, result);
			break;

		case IPSO_LIGHT_CONTROL_POWER_FACTOR:
			result = sizeof(LightControls[objectInstanceID].PowerFactor);
			memcpy(destBuffer, &LightControls[objectInstanceID].PowerFactor, result);
			break;

		default:
			result = -1;
			break;
	}

	return result;
}

static int LightControl_ResourceGetLengthHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID)
{
	int result = 0;

	switch (resourceID)
	{
		case IPSO_LIGHT_CONTROL_ON_OFF:
			result = sizeof(LightControls[objectInstanceID].OnOff);
			break;

		case IPSO_LIGHT_CONTROL_DIMMER:
			result = sizeof(LightControls[objectInstanceID].Dimmer);
			break;

		case IPSO_LIGHT_CONTROL_COLOUR:
			result = strlen(LightControls[objectInstanceID].Colour) + 1;
			break;

		case IPSO_LIGHT_CONTROL_UNITS:
			result = strlen(LightControls[objectInstanceID].Units) + 1;
			break;

		case IPSO_LIGHT_CONTROL_ON_TIME:
			result = sizeof(LightControls[objectInstanceID].OnTime);
			break;

		case IPSO_LIGHT_CONTROL_CUMULATIVE_ACTIVE_POWER:
			result = sizeof(LightControls[objectInstanceID].CumulativeActivePower);
			break;

		case IPSO_LIGHT_CONTROL_POWER_FACTOR:
			result = sizeof(LightControls[objectInstanceID].PowerFactor);
			break;

		default:
			result = -1;
			break;
	}

	return result;
}

static int LightControl_ResourceWriteHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID, uint8_t * srcBuffer, int srcBufferLen, bool * changed)
{
	int result;
	bool CallCallback = false;

	switch(resourceID)
	{
		case IPSO_LIGHT_CONTROL_ON_OFF:
			result = srcBufferLen;
			memcpy(&LightControls[objectInstanceID].OnOff, srcBuffer, result);
			CallCallback = true;
			break;

		case IPSO_LIGHT_CONTROL_DIMMER:
			result = srcBufferLen;
			memcpy(&LightControls[objectInstanceID].Dimmer, srcBuffer, result);
			CallCallback = true;
			break;

		case IPSO_LIGHT_CONTROL_COLOUR:
			result = srcBufferLen;
			if(result < sizeof(LightControls[objectInstanceID].Colour))
			{
				memcpy(LightControls[objectInstanceID].Colour, srcBuffer, result);
			}
			else
			{
				Lwm2m_Error("LightControl_ResourceWriteHandler Colour Type string too long: %d",
					result);
				result = -1;
			}
			CallCallback = true;
			break;

		case IPSO_LIGHT_CONTROL_UNITS:
			result = srcBufferLen;
			if(result < sizeof(LightControls[objectInstanceID].Units))
			{
				memcpy(LightControls[objectInstanceID].Units, srcBuffer, result);
			}
			else
			{
				Lwm2m_Error("LightControl_ResourceWriteHandler Units Type string too long: %d",
					result);
				result = -1;
			}
			break;

		case IPSO_LIGHT_CONTROL_ON_TIME:
			result = srcBufferLen;
			memcpy(&LightControls[objectInstanceID].OnTime, srcBuffer, result);
			break;

		case IPSO_LIGHT_CONTROL_CUMULATIVE_ACTIVE_POWER:
			result = srcBufferLen;
			memcpy(&LightControls[objectInstanceID].CumulativeActivePower, srcBuffer, result);
			break;

		case IPSO_LIGHT_CONTROL_POWER_FACTOR:
			result = srcBufferLen;
			memcpy(&LightControls[objectInstanceID].PowerFactor, srcBuffer, result);
			break;
		default:

			result = -1;
			break;
	}

	if (LightControls[objectInstanceID].callback != NULL && CallCallback)
	{
		LightControls[objectInstanceID].callback(LightControls[objectInstanceID].context,
			LightControls[objectInstanceID].OnOff, LightControls[objectInstanceID].Dimmer,
			LightControls[objectInstanceID].Colour);
	}


	if(result > 0)
		*changed = true;

	return result;
}

/***************************************************************************************************
 * Implementation - Public
 **************************************************************************************************/

int LightControl_RegisterLightControlObject(Lwm2mContextType * context)
{
	REGISTER_OBJECT(context, "LightControl", IPSO_LIGHT_CONTROL_OBJECT,                      \
		MultipleInstancesEnum_Multiple, MandatoryEnum_Optional,                              \
		&LightControlObjectOperationHandlers);

	REGISTER_LIGHT_CONTROL_RESOURCE(context, "On/Off", IPSO_LIGHT_CONTROL_ON_OFF,            \
		ResourceTypeEnum_TypeBoolean, MandatoryEnum_Mandatory, Operations_RW);
	REGISTER_LIGHT_CONTROL_RESOURCE(context, "Dimmer", IPSO_LIGHT_CONTROL_DIMMER,            \
		ResourceTypeEnum_TypeInteger, MandatoryEnum_Optional, Operations_RW);
	REGISTER_LIGHT_CONTROL_RESOURCE(context, "Colour", IPSO_LIGHT_CONTROL_COLOUR,            \
		ResourceTypeEnum_TypeString, MandatoryEnum_Optional, Operations_RW);
	REGISTER_LIGHT_CONTROL_RESOURCE(context, "Units", IPSO_LIGHT_CONTROL_UNITS,              \
		ResourceTypeEnum_TypeString, MandatoryEnum_Mandatory, Operations_R);
	REGISTER_LIGHT_CONTROL_RESOURCE(context, "OnTime", IPSO_LIGHT_CONTROL_ON_TIME,           \
		ResourceTypeEnum_TypeInteger, MandatoryEnum_Optional, Operations_RW);
	REGISTER_LIGHT_CONTROL_RESOURCE(context, "CumulativeActivePower",                        \
		IPSO_LIGHT_CONTROL_CUMULATIVE_ACTIVE_POWER, ResourceTypeEnum_TypeFloat,              \
		MandatoryEnum_Optional, Operations_R);
	REGISTER_LIGHT_CONTROL_RESOURCE(context, "PowerFactor", IPSO_LIGHT_CONTROL_POWER_FACTOR, \
		ResourceTypeEnum_TypeFloat, MandatoryEnum_Optional, Operations_R);

	return 0;
}

int LightControl_AddLightControl(Lwm2mContextType * context, ObjectInstanceIDType objectInstanceID,
	LightControlCallBack callback, void * callbackContext)
{
	if(objectInstanceID <= LIGHT_CONTROLS)
	{
		CREATE_OBJECT_INSTANCE(context, IPSO_LIGHT_CONTROL_OBJECT, objectInstanceID);
		CREATE_LIGHT_CONTROL_OPTIONAL_RESOURCE(context, objectInstanceID, IPSO_LIGHT_CONTROL_ON_OFF);
		CREATE_LIGHT_CONTROL_OPTIONAL_RESOURCE(context, objectInstanceID, IPSO_LIGHT_CONTROL_COLOUR);
		CREATE_LIGHT_CONTROL_OPTIONAL_RESOURCE(context, objectInstanceID, IPSO_LIGHT_CONTROL_ON_TIME);

		memset(&LightControls[objectInstanceID], 0, sizeof(IPSOLightControl));
		snprintf(LightControls[objectInstanceID].Colour, MAX_STR_SIZE, "Red%d", objectInstanceID+1);

		LightControls[objectInstanceID].callback = callback;
		LightControls[objectInstanceID].context = callbackContext;

		bool state = false;

		if (Lwm2mCore_SetResourceInstanceValue(context, IPSO_LIGHT_CONTROL_OBJECT, objectInstanceID,
			IPSO_LIGHT_CONTROL_ON_OFF, 0, &state, sizeof(state)) == -1)
		{
			Lwm2m_Error("Failed to set On/Off resource to %s", state ? "true" : "false");
			return -1;
		}
	}
	else
	{
		Lwm2m_Error("%d instance of Light Control exceeds max instances %d\n", objectInstanceID,
			LIGHT_CONTROLS);
		return -1;
	}
	return 0;
}

int LightControl_IncrementOnTime(Lwm2mContextType * context, ObjectInstanceIDType objectInstanceID,
	int seconds)
{
	//only increment on time if it is on.
	if(LightControls[objectInstanceID].OnOff == true)
	{
		int64_t OnTime = LightControls[objectInstanceID].OnTime + seconds;
		if (Lwm2mCore_SetResourceInstanceValue(context, IPSO_LIGHT_CONTROL_OBJECT, objectInstanceID,
			IPSO_LIGHT_CONTROL_ON_TIME, 0, &OnTime, sizeof(OnTime)) == -1)
		{
			Lwm2m_Error("Failed to increment ON Time resource\n");
			return -1;
		}
	}
	else
		return -1;

	return 0;
}
