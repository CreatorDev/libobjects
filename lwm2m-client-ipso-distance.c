/**
 * @file
 * LightWeightM2M LWM2M Distance object.
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
#include "lwm2m-client-ipso-distance.h"
/***************************************************************************************************
 * Typedefs
 **************************************************************************************************/
#define APPLICATION_TYPE_MAX_LEN    100

typedef struct
{
    AwaFloat Value;
    char Units[13];
    AwaFloat MinMeasuredValue;
    AwaFloat MaxMeasuredValue;
    AwaFloat MinRangeValue;
    AwaFloat MaxRangeValue;
    char ApplicatioType[APPLICATION_TYPE_MAX_LEN];
} IPSODistance;

/***************************************************************************************************
 * Globals
 **************************************************************************************************/

static IPSODistance DistanceStorage = {
  .Units = "meters",
  .ApplicatioType = "distance"
};

/***************************************************************************************************
 * Implementation
 **************************************************************************************************/

static AwaResult DistanceObject_Handler(AwaStaticClient *client, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID,
     AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void **dataPointer, size_t *dataSize, bool *changed)
{
    AwaResult result = AwaResult_InternalError;
    if ((objectID != IPSO_DISTANCE_OBJECT) || (objectInstanceID != 0))
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
            if (resourceID == IPSO_DISTANCE_RESET_MIN_AND_MAX_MEASURED_VALUES)
            {
                DistanceStorage.MinMeasuredValue = DistanceStorage.Value;
                DistanceStorage.MaxMeasuredValue = DistanceStorage.Value;
                AwaStaticClient_ResourceChanged(client, IPSO_DISTANCE_OBJECT, objectInstanceID, IPSO_DISTANCE_MAX_MEASURED_VALUE);
                AwaStaticClient_ResourceChanged(client, IPSO_DISTANCE_OBJECT, objectInstanceID, IPSO_DISTANCE_MIN_MEASURED_VALUE);
                result = AwaResult_Success;
            }
            else
            {
                printf("\n Invalid resource ID for Distance execute operation");
                result = AwaResult_InternalError;
            }
            break;

        default:
            printf("Distance - unknown operation (%d)\n", operation);
            result = AwaResult_SuccessContent;
            break;
    }
    return result;
}

AwaError DistanceObject_DefineObjectsAndResources(AwaStaticClient *awaClient, AwaFloat minRange, AwaFloat maxRange, char* applicatioType)
{
    DistanceStorage.MaxRangeValue = maxRange;
    DistanceStorage.MinRangeValue = minRange;
    strncpy(&DistanceStorage.ApplicatioType[0], applicatioType, APPLICATION_TYPE_MAX_LEN);
    AwaError error;

    error = AwaStaticClient_DefineObject(awaClient, IPSO_DISTANCE_OBJECT, "Distance", 0, 1);
    if (error != AwaError_Success)
    {
        printf("Failed to register Distance IPSO object\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_SENSOR_VALUE, "Sensor Value", AwaResourceType_Float,
        1, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_SENSOR_VALUE, &DistanceStorage.Value,
        sizeof(DistanceStorage.Value), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Sensor Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_UNITS, "Units", AwaResourceType_String, 0, 1,
        AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_UNITS, DistanceStorage.Units,
        strlen(DistanceStorage.Units), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Units' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_MIN_MEASURED_VALUE, "Min Measured Value",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_MIN_MEASURED_VALUE,
        &DistanceStorage.MinMeasuredValue, sizeof(DistanceStorage.MinMeasuredValue), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Min Measured Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_MAX_MEASURED_VALUE, "Max Measured Value",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_MAX_MEASURED_VALUE, &DistanceStorage.MaxMeasuredValue,
        sizeof(DistanceStorage.MaxMeasuredValue), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Max Measured Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_MIN_RANGE_VALUE, "Min Range Value",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_MIN_RANGE_VALUE, &DistanceStorage.MinRangeValue,
        sizeof(DistanceStorage.MinRangeValue), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Min Range Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_MAX_RANGE_VALUE, "Max Range Value",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_MAX_RANGE_VALUE, &DistanceStorage.MaxRangeValue,
        sizeof(DistanceStorage.MaxRangeValue), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Max Range Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_RESET_MIN_AND_MAX_MEASURED_VALUES,
        "Reset Min and Max Measured Values", AwaResourceType_Opaque, 0, 1, AwaResourceOperations_Execute);
    AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_RESET_MIN_AND_MAX_MEASURED_VALUES,
        DistanceObject_Handler);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Reset Min and Max Measured Values' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_APPLICATION_TYPE, "Application type",
        AwaResourceType_String, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_DISTANCE_OBJECT, IPSO_DISTANCE_APPLICATION_TYPE, &DistanceStorage.ApplicatioType,
        APPLICATION_TYPE_MAX_LEN, 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Application type' resource\n");
        return error;
    }

    int instance = 0;
    error = AwaStaticClient_CreateObjectInstance(awaClient, IPSO_DISTANCE_OBJECT, instance);
    if (error != AwaError_Success) {
        // if the object instance already exists,
        // the object already contains a maximum number of object instances,
        // or if no object is defined for the specified object ID.
        printf("Failed to CreateObjectInstance\n");
        return error;
    }

    //Since it's mandatory resource, you don't need to create it explicit. Awa will handle this for you
    //error = AwaStaticClient_CreateResource(awaClient, IPSO_DISTANCE_OBJECT, instance, IPSO_DISTANCE_SENSOR_VALUE);
    error = AwaStaticClient_CreateResource(awaClient, IPSO_DISTANCE_OBJECT, instance, IPSO_DISTANCE_UNITS);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Units'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_DISTANCE_OBJECT, instance, IPSO_DISTANCE_MAX_RANGE_VALUE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Max Range Value'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_DISTANCE_OBJECT, instance, IPSO_DISTANCE_MIN_RANGE_VALUE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Min Range Value'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_DISTANCE_OBJECT, instance, IPSO_DISTANCE_MIN_MEASURED_VALUE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Min Measured Value'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_DISTANCE_OBJECT, instance, IPSO_DISTANCE_MAX_MEASURED_VALUE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Max Measured Value'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_DISTANCE_OBJECT, instance, IPSO_DISTANCE_RESET_MIN_AND_MAX_MEASURED_VALUES);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Reset Min and Max Measured Values'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_DISTANCE_OBJECT, instance, IPSO_DISTANCE_APPLICATION_TYPE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Applicatio Type'\n");
        return error;
    }
    return AwaError_Success;
}

AwaError DistanceObject_SetDistance(AwaStaticClient *awaClient, AwaFloat distance)
{
    if (awaClient != NULL)
    {
        int objectInstanceID = 0;
        AwaError error;
        DistanceStorage.Value = distance;
        error = AwaStaticClient_ResourceChanged(awaClient, IPSO_DISTANCE_OBJECT, objectInstanceID, IPSO_DISTANCE_SENSOR_VALUE);

        if (error == AwaError_Success && distance < DistanceStorage.MinMeasuredValue) {
          DistanceStorage.MinMeasuredValue = distance;
          error = AwaStaticClient_ResourceChanged(awaClient, IPSO_DISTANCE_OBJECT, objectInstanceID, IPSO_DISTANCE_MIN_MEASURED_VALUE);
        }

        if (error == AwaError_Success && distance > DistanceStorage.MaxMeasuredValue) {
          DistanceStorage.MaxMeasuredValue = distance;
          AwaStaticClient_ResourceChanged(awaClient, IPSO_DISTANCE_OBJECT, objectInstanceID, IPSO_DISTANCE_MAX_MEASURED_VALUE);
        }
        return error;
    }
    else
    {
        printf("Invalid arguments passed to %s", __func__);
        return AwaError_Unspecified;
    }
}
