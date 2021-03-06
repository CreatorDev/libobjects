/**
 * @file
 * LightWeightM2M LWM2M Humidity object.
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
#include <awa/static.h>
#include "lwm2m-client-ipso-humidity.h"

/***************************************************************************************************
 * Typedefs
 **************************************************************************************************/

typedef struct
{
    AwaFloat Value;
    char Units[13];
    AwaFloat MinMeasuredValue;
    AwaFloat MaxMeasuredValue;
    AwaFloat MinRangeValue;
    AwaFloat MaxRangeValue;
} IPSOHumidity;

/***************************************************************************************************
 * Globals
 **************************************************************************************************/

static IPSOHumidity HumidityStorage = {
  .Units = "%" 
};

/***************************************************************************************************
 * Implementation
 **************************************************************************************************/

static AwaResult HumidityObject_Handler(AwaStaticClient *client, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID,
     AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void **dataPointer, size_t *dataSize, bool *changed)
{
    AwaResult result = AwaResult_InternalError;
    if ((objectID != IPSO_HUMIDITY_OBJECT) || (objectInstanceID != 0))
    {
        printf("Incorrect object data\n");
        return result;
    }

    switch (operation)
    {
        case AwaOperation_CreateResource:
            result = AwaResult_SuccessCreated;
            break;

        case AwaOperation_Execute:
            if (resourceID == IPSO_HUMIDITY_RESET_MIN_AND_MAX_MEASURED_VALUES)
            {
                HumidityStorage.MinMeasuredValue = HumidityStorage.Value;
                HumidityStorage.MaxMeasuredValue = HumidityStorage.Value;
                AwaStaticClient_ResourceChanged(client, IPSO_HUMIDITY_OBJECT, objectInstanceID, IPSO_HUMIDITY_MAX_MEASURED_VALUE);
                AwaStaticClient_ResourceChanged(client, IPSO_HUMIDITY_OBJECT, objectInstanceID, IPSO_HUMIDITY_MIN_MEASURED_VALUE);
                result = AwaResult_Success;
            }
            else
            {
                printf("\n Invalid resource ID for Humidity execute operation");
                result = AwaResult_InternalError;
            }
            break;

        default:
            printf("Humidity - unknown operation (%d)\n", operation);
            result = AwaResult_SuccessContent;
            break;
    }
    return result;
}

AwaError HumidityObject_DefineObjectsAndResources(AwaStaticClient *awaClient, AwaFloat minRange, AwaFloat maxRange)
{
    HumidityStorage.MaxRangeValue = maxRange;
    HumidityStorage.MinRangeValue = minRange;

    AwaError error;

    error = AwaStaticClient_DefineObject(awaClient, IPSO_HUMIDITY_OBJECT, "Humidity", 0, 1);
    if (error != AwaError_Success)
    {
        printf("Failed to register Humidity IPSO object\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_SENSOR_VALUE, "Sensor Value", AwaResourceType_Float,
        1, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_SENSOR_VALUE, &HumidityStorage.Value,
        sizeof(HumidityStorage.Value), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Sensor Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_UNITS, "Units", AwaResourceType_String, 0, 1,
        AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_UNITS, HumidityStorage.Units,
        strlen(HumidityStorage.Units), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Units' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_MIN_MEASURED_VALUE, "Min Measured Value",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_MIN_MEASURED_VALUE,
        &HumidityStorage.MinMeasuredValue, sizeof(HumidityStorage.MinMeasuredValue), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Min Measured Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_MAX_MEASURED_VALUE, "Max Measured Value",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_MAX_MEASURED_VALUE, &HumidityStorage.MaxMeasuredValue,
        sizeof(HumidityStorage.MaxMeasuredValue), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Max Measured Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_MIN_RANGE_VALUE, "Min Range Value",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_MIN_RANGE_VALUE, &HumidityStorage.MinRangeValue,
        sizeof(HumidityStorage.MinRangeValue), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Min Range Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_MAX_RANGE_VALUE, "Max Range Value",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_MAX_RANGE_VALUE, &HumidityStorage.MaxRangeValue,
        sizeof(HumidityStorage.MaxRangeValue), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Max Range Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_RESET_MIN_AND_MAX_MEASURED_VALUES,
        "Reset Min and Max Measured Values", AwaResourceType_Opaque, 0, 1, AwaResourceOperations_Execute);
    AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_HUMIDITY_OBJECT, IPSO_HUMIDITY_RESET_MIN_AND_MAX_MEASURED_VALUES,
        HumidityObject_Handler);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Reset Min and Max Measured Values' resource\n");
        return error;
    }

    int instance = 0;
    error = AwaStaticClient_CreateObjectInstance(awaClient, IPSO_HUMIDITY_OBJECT, instance);
    if (error != AwaError_Success) {
        // if the object instance already exists,
        // the object already contains a maximum number of object instances,
        // or if no object is defined for the specified object ID.
        printf("Failed to CreateObjectInstance\n");
        return error;
    }

    //Since it's mandatory resource, you don't need to create it explicit. Awa will handle this for you
    //error = AwaStaticClient_CreateResource(awaClient, IPSO_HUMIDITY_OBJECT, instance, IPSO_HUMIDITY_SENSOR_VALUE);
    error = AwaStaticClient_CreateResource(awaClient, IPSO_HUMIDITY_OBJECT, instance, IPSO_HUMIDITY_UNITS);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Units'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_HUMIDITY_OBJECT, instance, IPSO_HUMIDITY_MAX_RANGE_VALUE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Max Range Value'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_HUMIDITY_OBJECT, instance, IPSO_HUMIDITY_MIN_RANGE_VALUE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Min Range Value'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_HUMIDITY_OBJECT, instance, IPSO_HUMIDITY_MIN_MEASURED_VALUE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Min Measured Value'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_HUMIDITY_OBJECT, instance, IPSO_HUMIDITY_MAX_MEASURED_VALUE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Max Measured Value'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_HUMIDITY_OBJECT, instance, IPSO_HUMIDITY_RESET_MIN_AND_MAX_MEASURED_VALUES);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Reset Min and Max Measured Values'\n");
        return error;
    }
    return AwaError_Success;
}

AwaError HumidityObject_SetHumidity(AwaStaticClient *awaClient, AwaFloat humidity)
{
    if (awaClient != NULL)
    {
        int objectInstanceID = 0;
        AwaError error;
        HumidityStorage.Value = humidity;
        error = AwaStaticClient_ResourceChanged(awaClient, IPSO_HUMIDITY_OBJECT, objectInstanceID, IPSO_HUMIDITY_SENSOR_VALUE);

        if (error == AwaError_Success && humidity < HumidityStorage.MinMeasuredValue) {
          HumidityStorage.MinMeasuredValue = humidity;
          error = AwaStaticClient_ResourceChanged(awaClient, IPSO_HUMIDITY_OBJECT, objectInstanceID, IPSO_HUMIDITY_MIN_MEASURED_VALUE);
        }

        if (error == AwaError_Success && humidity > HumidityStorage.MaxMeasuredValue) {
          HumidityStorage.MaxMeasuredValue = humidity;
          AwaStaticClient_ResourceChanged(awaClient, IPSO_HUMIDITY_OBJECT, objectInstanceID, IPSO_HUMIDITY_MAX_MEASURED_VALUE);
        }
        return error;
    }
    else
    {
        printf("Invalid arguments passed to %s", __func__);
        return AwaError_Unspecified;
    }
}
