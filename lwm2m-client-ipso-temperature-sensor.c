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

/************************************************************************************************************
 * Includes
 ************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "lwm2m_core.h"
#include "coap_abstraction.h"
#include "common.h"
#include "thermo3-click.h"
#include "lwm2m-client-ipso-temperature-sensor.h"

/************************************************************************************************************
 * Definitions
 ************************************************************************************************************/

#define IPSO_TEMPERATURE_SENSOR_OBJECT      3303
#define IPSO_SENSOR_VALUE                   5700
#define IPSO_SET_POINT_VALUE                5900

#define NUM_OF_SENSORS                      1

#define REGISTER_TEMPERATURE_SENSOR_RESOURCE(context, name, id, type, operations)                   \
	REGISTER_RESOURCE(context, name, IPSO_TEMPERATURE_SENSOR_OBJECT, id, type,                      \
		MultipleInstancesEnum_Single, MandatoryEnum_Optional, operations,                           \
		&TemperatureSensorResourceOperationHandlers)

#define CREATE_TEMPERATURE_SENSOR_OPTIONAL_RESOURCE(context, objectInstanceId, resourcId)           \
	CREATE_OPTIONAL_RESOURCE(context, IPSO_TEMPERATURE_SENSOR_OBJECT, objectInstanceId, resourcId)

/************************************************************************************************************
 * Prototypes
 ************************************************************************************************************/
typedef struct
{
    float SensorValue;
    int64_t SetPointValue;
} IPSOTemperatureSensor;

static int TemperatureSensor_ResourceReadHandler(void * context, ObjectIDType objectID, 
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID, uint8_t * destBuffer, int destBufferLen);

static int TemperatureSensor_ResourceGetLengthHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID);

static int TemperatureSensor_ResourceWriteHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID, uint8_t * srcBuffer, int srcBufferLen, bool * changed);

static int TemperatureSensor_ResourceCreateHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID);

static int TemperatureSensor_ObjectCreateInstanceHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID);

static int TemperatureSensor_ObjectDeleteHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID);

/************************************************************************************************************
 * Global variables
 ************************************************************************************************************/
static ObjectOperationHandlers TemperatureSensorObjectOperationHandlers =
{
    .CreateInstance = TemperatureSensor_ObjectCreateInstanceHandler,
    .Delete = TemperatureSensor_ObjectDeleteHandler,
};

static ResourceOperationHandlers TemperatureSensorResourceOperationHandlers =
{
    .Read = TemperatureSensor_ResourceReadHandler,
    .GetLength = TemperatureSensor_ResourceGetLengthHandler,
    .Write = TemperatureSensor_ResourceWriteHandler,
    .CreateOptionalResource = TemperatureSensor_ResourceCreateHandler,
};

static float CurrentTemperatureValue,PreviousTemperatureValue = 0;

static IPSOTemperatureSensor TemperatureSensors[NUM_OF_SENSORS];

/************************************************************************************************************
 * Implementation
 ************************************************************************************************************/

static int TemperatureSensor_ObjectCreateInstanceHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID)
{
    if(objectInstanceID >= NUM_OF_SENSORS)
    {
        Lwm2m_Error("TemperatureSensor_ResourceCreateHandler instance number %d out of range (max %d)",
        objectInstanceID, NUM_OF_SENSORS - 1);
        return -1;
    }
    return objectInstanceID;
}

static int TemperatureSensor_ResourceCreateHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID)
{
    return 0;
}

static int TemperatureSensor_ObjectDeleteHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID)
{
    if (objectID != IPSO_TEMPERATURE_SENSOR_OBJECT)
    {
        Lwm2m_Error("TemperatureSensor_ObjectDeleteHandler Invalid OIR: %d/%d/%d\n", objectID,
        objectInstanceID, resourceID);
        return -1;
    }

    if(objectInstanceID >= NUM_OF_SENSORS)
    {
        Lwm2m_Error("TemperatureSensor_ObjectDeleteHandler instance number %d out of range (max %d)",
        objectInstanceID, NUM_OF_SENSORS - 1);
        return -1;
    }

    if (resourceID == -1)
    {
        memset(&TemperatureSensors[objectInstanceID], 0, sizeof(IPSOTemperatureSensor));
    }
    return 0;
}

static int TemperatureSensor_ResourceReadHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID, uint8_t * destBuffer, int destBufferLen)
{
    int result = 0;
	switch (resourceID)
	{
		case IPSO_SENSOR_VALUE:
			result = sizeof(TemperatureSensors[objectInstanceID].SensorValue);
			memcpy(destBuffer, &TemperatureSensors[objectInstanceID].SensorValue, result);
			break;
		case IPSO_SET_POINT_VALUE:
			result = sizeof(TemperatureSensors[objectInstanceID].SetPointValue);
			memcpy(destBuffer, &TemperatureSensors[objectInstanceID].SetPointValue, result);
			break;
		default:
			result = -1;
			break;
    }

    return result;
}

static int TemperatureSensor_ResourceGetLengthHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID)
{
    int result = 0;

	switch (resourceID)
	{
		case IPSO_SENSOR_VALUE:
			result = sizeof(TemperatureSensors[objectInstanceID].SensorValue);
        		break;
		case IPSO_SET_POINT_VALUE:
			result = sizeof(TemperatureSensors[objectInstanceID].SetPointValue);
			break;
	}
	return result;
}

static int TemperatureSensor_ResourceWriteHandler(void * context, ObjectIDType objectID,
	ObjectInstanceIDType objectInstanceID, ResourceIDType resourceID,
	ResourceInstanceIDType resourceInstanceID, uint8_t * srcBuffer, int srcBufferLen, bool * changed)
{
    int result;

	switch(resourceID)
	{
		case IPSO_SENSOR_VALUE:
			result = srcBufferLen;
			memcpy(&TemperatureSensors[objectInstanceID].SensorValue, srcBuffer, result);
			Lwm2m_Debug("Temperature of node is  %f  Degree Celsius.\n",
				TemperatureSensors[objectInstanceID].SensorValue);
			break;
		case IPSO_SET_POINT_VALUE:
			result = srcBufferLen;
			memcpy(&TemperatureSensors[objectInstanceID].SetPointValue, srcBuffer, result);
			break;
		default:
			result = -1;
			break;
	}
	if(result > 0)
        	*changed = true;

	return result;
}

/************************************************************************************************************
 * Implementation - Public
 ************************************************************************************************************/

int TemperatureSensor_RegisterTemperatureSensorObject(Lwm2mContextType * context)
{

	REGISTER_OBJECT(context, "TemperatureSensor", IPSO_TEMPERATURE_SENSOR_OBJECT,          \
		MultipleInstancesEnum_Multiple, MandatoryEnum_Optional,                            \
		&TemperatureSensorObjectOperationHandlers);

	REGISTER_TEMPERATURE_SENSOR_RESOURCE(context, "SensorValue", IPSO_SENSOR_VALUE,        \
	  	ResourceTypeEnum_TypeFloat, Operations_R);
	REGISTER_TEMPERATURE_SENSOR_RESOURCE(context, "SetPointValue",IPSO_SET_POINT_VALUE,    \
		ResourceTypeEnum_TypeInteger, Operations_RW);
	
	return 0;
}


int TemperatureSensor_AddSensorValue(Lwm2mContextType *context, ObjectInstanceIDType objectInstanceID)
{
	if(objectInstanceID <  NUM_OF_SENSORS)
	{
		CREATE_OBJECT_INSTANCE(context, IPSO_TEMPERATURE_SENSOR_OBJECT, objectInstanceID);
		CREATE_TEMPERATURE_SENSOR_OPTIONAL_RESOURCE(context, objectInstanceID,              \
			IPSO_SENSOR_VALUE);
		CREATE_TEMPERATURE_SENSOR_OPTIONAL_RESOURCE(context, objectInstanceID,              \
			IPSO_SET_POINT_VALUE);
		
		TemperatureSensors[objectInstanceID].SetPointValue = 1;
	}
	else
	{
		Lwm2m_Error("%d instance of Temperature sensor exceeds max instances %d\n", objectInstanceID,
				 NUM_OF_SENSORS);
		return -1;
	}
	return 0;
}
  
int TemperatureSensor_PeriodicTemperatureCheck(Lwm2mContextType * context, ObjectInstanceIDType objectInstanceID)
{
	if(objectInstanceID < NUM_OF_SENSORS)
	{
		CurrentTemperatureValue = tmp102_read_temp();
			if (PreviousTemperatureValue == 0) {
				PreviousTemperatureValue = CurrentTemperatureValue;
			}
			if (fabs(CurrentTemperatureValue - PreviousTemperatureValue) >= 
				TemperatureSensors[objectInstanceID].SetPointValue )
			{
				Lwm2mCore_SetResourceInstanceValue(context, IPSO_TEMPERATURE_SENSOR_OBJECT, 
					objectInstanceID, IPSO_SENSOR_VALUE, 0, &CurrentTemperatureValue,
					sizeof(TemperatureSensors[objectInstanceID].SensorValue));
				PreviousTemperatureValue = CurrentTemperatureValue;
				return 1;
			} 
	}
	return 0;
}

void TemperatureSensor_WriteTemperature(Lwm2mContextType * context, ObjectInstanceIDType objectInstanceID)
{
	if(objectInstanceID < NUM_OF_SENSORS )
	{
		Lwm2mCore_SetResourceInstanceValue(context, IPSO_TEMPERATURE_SENSOR_OBJECT,
			objectInstanceID, IPSO_SENSOR_VALUE, 0, &PreviousTemperatureValue,
			sizeof(TemperatureSensors[objectInstanceID].SensorValue));       
	}
}  
