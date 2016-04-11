/**
 * @file
 * LightWeightM2M LWM2M Digital Input object.
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

/***************************************************************************************************
 * Definitions
 **************************************************************************************************/

#define IPSO_DIGITAL_INPUT_OBJECT				3200
#define IPSO_DIGITAL_INPUT_STATE				5500
#define IPSO_DIGITAL_INPUT_COUNTER				5501
#define IPSO_DIGITAL_INPUT_POLARITY				5502
#define IPSO_DIGITAL_INPUT_DEBOUNCE_PERIOD		5503
#define IPSO_DIGITAL_INPUT_EDGE_SELECTION		5504
#define IPSO_DIGITAL_INPUT_COUNTER_RESET		5505

#define IPSO_APPICATION_TYPE					5750
#define IPSO_SENSOR_TYPE						5751

#define DIGITAL_INPUTS							2

#define MAX_STR_SIZE							128

#define REGISTER_DIGITAL_INPUT_RESOURCE(context, name, id, type, operations) \
	REGISTER_RESOURCE(context, name, IPSO_DIGITAL_INPUT_OBJECT, id, type,    \
		MultipleInstancesEnum_Single, MandatoryEnum_Optional, operations,    \
		&DigitalInputResourceOperationHandlers)

#define CREATE_DIGITAL_INPUT_OPTIONAL_RESOURCE(context, objectInstanceId, resourcId) \
	CREATE_OPTIONAL_RESOURCE(context, IPSO_DIGITAL_INPUT_OBJECT, objectInstanceId, resourcId)

/***************************************************************************************************
 * Typedefs
 **************************************************************************************************/

typedef struct
{
	bool State;
	int64_t Counter;
	bool Polarity;
	int64_t DebouncePeriod;
	int64_t EdgeSelection;
	char ApplicationType[MAX_STR_SIZE];
	char SensoryType[MAX_STR_SIZE];
} IPSODigitalInput;

/***************************************************************************************************
 * Prototypes
 **************************************************************************************************/

static int DigitalInput_ResourceReadHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID, uint8_t *destBuffer, int destBufferLen);

static int DigitalInput_ResourceGetLengthHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID);

static int DigitalInput_ResourceWriteHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID, uint8_t *srcBuffer, int srcBufferLen, bool *changed);

static int DigitalInput_ResourceExecuteHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID, uint8_t *srcBuffer,
	int srcBufferLen);

static int DigitalInput_ResourceCreateHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID);

static int DigitalInput_ObjectCreateInstanceHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID);

static int DigitalInput_ObjectDeleteHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID);

/***************************************************************************************************
 * Globals
 **************************************************************************************************/

static ObjectOperationHandlers DigitalInputObjectOperationHandlers =
{
	.CreateInstance = DigitalInput_ObjectCreateInstanceHandler,
	.Delete = DigitalInput_ObjectDeleteHandler,
};

static ResourceOperationHandlers DigitalInputResourceOperationHandlers =
{
	.Read = DigitalInput_ResourceReadHandler,
	.GetLength = DigitalInput_ResourceGetLengthHandler,
	.Write = DigitalInput_ResourceWriteHandler,
	.CreateOptionalResource = DigitalInput_ResourceCreateHandler,
	.Execute = DigitalInput_ResourceExecuteHandler,
};

static IPSODigitalInput digitalInputs[DIGITAL_INPUTS];

/***************************************************************************************************
 * Implementation
 **************************************************************************************************/

static int DigitalInput_ObjectCreateInstanceHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID)
{
	if(objectInstanceID >= DIGITAL_INPUTS)
	{
		Lwm2m_Error("DigitalInput_ResourceCreateHandler instance number %d out of range (max %d)",
			objectInstanceID, DIGITAL_INPUTS - 1);
		return -1;
	}
	return objectInstanceID;
}

static int DigitalInput_ResourceCreateHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID)
{
	return 0;
}

static int DigitalInput_ObjectDeleteHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID)
{
	if (objectID != IPSO_DIGITAL_INPUT_OBJECT)
	{
		Lwm2m_Error("DigitalInput_ObjectDeleteHandler Invalid OIR: %d/%d/%d\n", objectID,
			objectInstanceID, resourceID);
		return -1;
	}

	if(objectInstanceID >= DIGITAL_INPUTS)
	{
		Lwm2m_Error("DigitalInput_ObjectDeleteHandler instance number %d out of range (max %d)",
			objectInstanceID, DIGITAL_INPUTS - 1);
		return -1;
	}

	if (resourceID == -1)
	{
		memset(&digitalInputs[objectInstanceID], 0, sizeof(IPSODigitalInput));
	}
	else
	{
		//TODO
	}

	return 0;
}

static int DigitalInput_ResourceReadHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID, uint8_t *destBuffer, int destBufferLen)
{
	int result = 0;

	switch (resourceID)
	{
		case IPSO_DIGITAL_INPUT_STATE:
			result = sizeof(digitalInputs[objectInstanceID].State);
			memcpy(destBuffer, &digitalInputs[objectInstanceID].State, result);
			break;

		case IPSO_DIGITAL_INPUT_COUNTER:
			result = sizeof(digitalInputs[objectInstanceID].Counter);
			memcpy(destBuffer, &digitalInputs[objectInstanceID].Counter, result);
			break;

		case IPSO_DIGITAL_INPUT_POLARITY:
			result = sizeof(digitalInputs[objectInstanceID].State);
			memcpy(destBuffer, &digitalInputs[objectInstanceID].State, result);
			break;

		case IPSO_DIGITAL_INPUT_DEBOUNCE_PERIOD:
			result = sizeof(digitalInputs[objectInstanceID].Counter);
			memcpy(destBuffer, &digitalInputs[objectInstanceID].Counter, result);
			break;

		case IPSO_DIGITAL_INPUT_EDGE_SELECTION:
			result = sizeof(digitalInputs[objectInstanceID].State);
			memcpy(destBuffer, &digitalInputs[objectInstanceID].State, result);
			break;

		case IPSO_APPICATION_TYPE:
			result = strlen(digitalInputs[objectInstanceID].ApplicationType) + 1;
			memcpy(destBuffer, digitalInputs[objectInstanceID].ApplicationType, result);
			break;

		case IPSO_SENSOR_TYPE:
			result = strlen(digitalInputs[objectInstanceID].SensoryType) + 1;
			memcpy(destBuffer, digitalInputs[objectInstanceID].SensoryType, result);
			break;

		default:
			result = -1;
			break;
	}

	return result;
}

static int DigitalInput_ResourceGetLengthHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID)
{
	int result = 0;

	switch (resourceID)
	{
		case IPSO_DIGITAL_INPUT_STATE:
			result = sizeof(digitalInputs[objectInstanceID].State);
			break;

		case IPSO_DIGITAL_INPUT_COUNTER:
			result = sizeof(digitalInputs[objectInstanceID].Counter);
			break;

		case IPSO_DIGITAL_INPUT_POLARITY:
			result = sizeof(digitalInputs[objectInstanceID].State);
			break;

		case IPSO_DIGITAL_INPUT_DEBOUNCE_PERIOD:
			result = sizeof(digitalInputs[objectInstanceID].Counter);
			break;

		case IPSO_DIGITAL_INPUT_EDGE_SELECTION:
			result = sizeof(digitalInputs[objectInstanceID].State);
			break;

		case IPSO_APPICATION_TYPE:
			result = strlen(digitalInputs[objectInstanceID].ApplicationType) + 1;
			break;

		case IPSO_SENSOR_TYPE:
			result = strlen(digitalInputs[objectInstanceID].SensoryType) + 1;
			break;

		default:
			result = -1;
			break;
	}

	return result;
}

static int DigitalInput_ResourceWriteHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID, uint8_t *srcBuffer, int srcBufferLen, bool *changed)
{
	int result;

	switch(resourceID)
	{
		case IPSO_DIGITAL_INPUT_STATE:
			result = srcBufferLen;
			memcpy(&digitalInputs[objectInstanceID].State, srcBuffer, result);
			break;

		case IPSO_DIGITAL_INPUT_COUNTER:
			result = srcBufferLen;
			memcpy(&digitalInputs[objectInstanceID].Counter, srcBuffer, result);
			Lwm2m_Debug("Button %d counter incremented to %d.\n", objectInstanceID + 1,
				(int)digitalInputs[objectInstanceID].Counter);
			break;

		case IPSO_DIGITAL_INPUT_POLARITY:
			result = srcBufferLen;
			memcpy(&digitalInputs[objectInstanceID].State, srcBuffer, result);
			break;

		case IPSO_DIGITAL_INPUT_DEBOUNCE_PERIOD:
			result = srcBufferLen;
			memcpy(&digitalInputs[objectInstanceID].Counter, srcBuffer, result);
			break;

		case IPSO_DIGITAL_INPUT_EDGE_SELECTION:
			result = srcBufferLen;
			memcpy(&digitalInputs[objectInstanceID].State, srcBuffer, result);
			break;

		case IPSO_APPICATION_TYPE:
			result = srcBufferLen;
			if(result < sizeof(digitalInputs[objectInstanceID].ApplicationType))
			{
				memcpy(digitalInputs[objectInstanceID].ApplicationType, srcBuffer, result);
			}
			else
			{
				Lwm2m_Error("DigitalInput_ResourceWriteHandler Application Type string too long: "
					"%d", result);
				result = -1;
			}
			break;

		case IPSO_SENSOR_TYPE:
			result = srcBufferLen;
			if(result < sizeof(digitalInputs[objectInstanceID].SensoryType))
			{
				memcpy(digitalInputs[objectInstanceID].SensoryType, srcBuffer, result);
			}
			else
			{
				Lwm2m_Error("DigitalInput_ResourceWriteHandler Sensor Type string too long: %d",
					result);
				result = -1;
			}
			break;

		default:
			result = -1;
			break;
	}

	if(result > 0)
		*changed = true;

	return result;
}

static int DigitalInput_ResourceExecuteHandler(void *context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID, uint8_t *srcBuffer,
	int srcBufferLen)
{
	if(resourceID == IPSO_DIGITAL_INPUT_COUNTER_RESET)
	{
		int64_t zero = 0;
		if (Lwm2mCore_SetResourceInstanceValue(context, objectID, objectInstanceID,
			IPSO_DIGITAL_INPUT_COUNTER, 0, &zero, sizeof(zero)) == -1)
		{
			Lwm2m_Error("Failed to set Counter to %" PRId64 "\n", zero);
			return -1;
		}
	}
	else
		return -1;

	return 0;
}

/***************************************************************************************************
 * Implementation - Public
 **************************************************************************************************/

int DigitalInput_RegisterDigitalInputObject(Lwm2mContextType * context)
{

	REGISTER_OBJECT(context, "DigitalInput", IPSO_DIGITAL_INPUT_OBJECT, \
		MultipleInstancesEnum_Multiple, MandatoryEnum_Optional, \
		&DigitalInputObjectOperationHandlers);

	REGISTER_DIGITAL_INPUT_RESOURCE(context, "State", IPSO_DIGITAL_INPUT_STATE, \
		ResourceTypeEnum_TypeBoolean, Operations_R);
	REGISTER_DIGITAL_INPUT_RESOURCE(context, "Counter", IPSO_DIGITAL_INPUT_COUNTER, \
		ResourceTypeEnum_TypeInteger, Operations_R);
	REGISTER_DIGITAL_INPUT_RESOURCE(context, "Polarity", IPSO_DIGITAL_INPUT_POLARITY, \
		ResourceTypeEnum_TypeBoolean, Operations_RW);
	REGISTER_DIGITAL_INPUT_RESOURCE(context, "DebouncePeriod", IPSO_DIGITAL_INPUT_DEBOUNCE_PERIOD, \
		ResourceTypeEnum_TypeInteger, Operations_RW);
	REGISTER_DIGITAL_INPUT_RESOURCE(context, "EdgeSelection", IPSO_DIGITAL_INPUT_EDGE_SELECTION, \
		ResourceTypeEnum_TypeInteger, Operations_RW);
	REGISTER_DIGITAL_INPUT_RESOURCE(context, "CounterReset", IPSO_DIGITAL_INPUT_COUNTER_RESET, \
		ResourceTypeEnum_TypeNone, Operations_E);
	REGISTER_DIGITAL_INPUT_RESOURCE(context, "ApplicationType", IPSO_APPICATION_TYPE, \
		ResourceTypeEnum_TypeString, Operations_R);
	REGISTER_DIGITAL_INPUT_RESOURCE(context, "SensorType", IPSO_SENSOR_TYPE, \
		ResourceTypeEnum_TypeString, Operations_R);

	return 0;
}

int DigitalInput_AddDigitialInput(Lwm2mContextType *context, ObjectInstanceIDType objectInstanceID)
{
	if(objectInstanceID < DIGITAL_INPUTS)
	{
		CREATE_OBJECT_INSTANCE(context, IPSO_DIGITAL_INPUT_OBJECT, objectInstanceID);
		CREATE_DIGITAL_INPUT_OPTIONAL_RESOURCE(context, objectInstanceID, \
			IPSO_DIGITAL_INPUT_COUNTER);
		CREATE_DIGITAL_INPUT_OPTIONAL_RESOURCE(context, objectInstanceID, \
			IPSO_DIGITAL_INPUT_COUNTER_RESET);
		CREATE_DIGITAL_INPUT_OPTIONAL_RESOURCE(context, objectInstanceID, IPSO_SENSOR_TYPE);

		memset(&digitalInputs[objectInstanceID], 0, sizeof(IPSODigitalInput));
		snprintf(digitalInputs[objectInstanceID].SensoryType, MAX_STR_SIZE, "Button%d",
			objectInstanceID + 1);
	}
	else
	{
		Lwm2m_Error("%d instance of Digital Input exceeds max instances %d\n", objectInstanceID,
			DIGITAL_INPUTS);
		return -1;
	}
	return 0;
}

int DigitalInput_IncrementCounter(Lwm2mContextType *context, ObjectInstanceIDType objectInstanceID)
{
	int64_t counter = digitalInputs[objectInstanceID].Counter + 1;
	if (Lwm2mCore_SetResourceInstanceValue(context, IPSO_DIGITAL_INPUT_OBJECT, objectInstanceID,
		IPSO_DIGITAL_INPUT_COUNTER, 0, &counter, sizeof(counter)) == -1)
	{
		Lwm2m_Error("Failed to increment Counter resource\n");
		return -1;
	}
	return 0;
}
