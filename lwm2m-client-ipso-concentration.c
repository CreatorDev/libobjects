/**
 * @file
 * LightWeightM2M LWM2M Concentration object.
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
#include "lwm2m-client-ipso-concentration.h"
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
} IPSOConcentration;

/***************************************************************************************************
 * Globals
 **************************************************************************************************/

static IPSOConcentration ConcentrationStorage = {
  .Units = "ppm",
  .ApplicatioType = "concentration"
};

/***************************************************************************************************
 * Implementation
 **************************************************************************************************/

static AwaResult ConcentrationObject_Handler(AwaStaticClient *client, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID,
     AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void **dataPointer, size_t *dataSize, bool *changed)
{
    AwaResult result = AwaResult_InternalError;
    if ((objectID != IPSO_CONCENTRATION_OBJECT) || (objectInstanceID != 0))
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
            if (resourceID == IPSO_CONCENTRATION_RESET_MIN_AND_MAX_MEASURED_VALUES)
            {
                ConcentrationStorage.MinMeasuredValue = ConcentrationStorage.Value;
                ConcentrationStorage.MaxMeasuredValue = ConcentrationStorage.Value;
                AwaStaticClient_ResourceChanged(client, IPSO_CONCENTRATION_OBJECT, objectInstanceID, IPSO_CONCENTRATION_MAX_MEASURED_VALUE);
                AwaStaticClient_ResourceChanged(client, IPSO_CONCENTRATION_OBJECT, objectInstanceID, IPSO_CONCENTRATION_MIN_MEASURED_VALUE);
                result = AwaResult_Success;
            }
            else
            {
                printf("\n Invalid resource ID for Concentration execute operation");
                result = AwaResult_InternalError;
            }
            break;

        default:
            printf("Concentration - unknown operation (%d)\n", operation);
            result = AwaResult_SuccessContent;
            break;
    }
    return result;
}

AwaError ConcentrationObject_DefineObjectsAndResources(AwaStaticClient *awaClient, AwaFloat minRange, AwaFloat maxRange, char* applicatioType)
{
    ConcentrationStorage.MaxRangeValue = maxRange;
    ConcentrationStorage.MinRangeValue = minRange;
    strncpy(&ConcentrationStorage.ApplicatioType[0], applicatioType, APPLICATION_TYPE_MAX_LEN);
    AwaError error;

    error = AwaStaticClient_DefineObject(awaClient, IPSO_CONCENTRATION_OBJECT, "Concentration", 0, 1);
    if (error != AwaError_Success)
    {
        printf("Failed to register Concentration IPSO object\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_SENSOR_VALUE, "Sensor Value", AwaResourceType_Float,
        1, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_SENSOR_VALUE, &ConcentrationStorage.Value,
        sizeof(ConcentrationStorage.Value), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Sensor Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_UNITS, "Units", AwaResourceType_String, 0, 1,
        AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_UNITS, ConcentrationStorage.Units,
        strlen(ConcentrationStorage.Units), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Units' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_MIN_MEASURED_VALUE, "Min Measured Value",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_MIN_MEASURED_VALUE,
        &ConcentrationStorage.MinMeasuredValue, sizeof(ConcentrationStorage.MinMeasuredValue), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Min Measured Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_MAX_MEASURED_VALUE, "Max Measured Value",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_MAX_MEASURED_VALUE, &ConcentrationStorage.MaxMeasuredValue,
        sizeof(ConcentrationStorage.MaxMeasuredValue), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Max Measured Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_MIN_RANGE_VALUE, "Min Range Value",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_MIN_RANGE_VALUE, &ConcentrationStorage.MinRangeValue,
        sizeof(ConcentrationStorage.MinRangeValue), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Min Range Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_MAX_RANGE_VALUE, "Max Range Value",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_MAX_RANGE_VALUE, &ConcentrationStorage.MaxRangeValue,
        sizeof(ConcentrationStorage.MaxRangeValue), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Max Range Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_RESET_MIN_AND_MAX_MEASURED_VALUES,
        "Reset Min and Max Measured Values", AwaResourceType_Opaque, 0, 1, AwaResourceOperations_Execute);
    AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_RESET_MIN_AND_MAX_MEASURED_VALUES,
        ConcentrationObject_Handler);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Reset Min and Max Measured Values' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_APPLICATION_TYPE, "Application type",
        AwaResourceType_String, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_CONCENTRATION_OBJECT, IPSO_CONCENTRATION_APPLICATION_TYPE, &ConcentrationStorage.ApplicatioType,
        APPLICATION_TYPE_MAX_LEN, 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Application type' resource\n");
        return error;
    }

    int instance = 0;
    error = AwaStaticClient_CreateObjectInstance(awaClient, IPSO_CONCENTRATION_OBJECT, instance);
    if (error != AwaError_Success) {
        // if the object instance already exists,
        // the object already contains a maximum number of object instances,
        // or if no object is defined for the specified object ID.
        printf("Failed to CreateObjectInstance\n");
        return error;
    }

    //Since it's mandatory resource, you don't need to create it explicit. Awa will handle this for you
    //error = AwaStaticClient_CreateResource(awaClient, IPSO_CONCENTRATION_OBJECT, instance, IPSO_CONCENTRATION_SENSOR_VALUE);
    error = AwaStaticClient_CreateResource(awaClient, IPSO_CONCENTRATION_OBJECT, instance, IPSO_CONCENTRATION_UNITS);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Units'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_CONCENTRATION_OBJECT, instance, IPSO_CONCENTRATION_MAX_RANGE_VALUE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Max Range Value'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_CONCENTRATION_OBJECT, instance, IPSO_CONCENTRATION_MIN_RANGE_VALUE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Min Range Value'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_CONCENTRATION_OBJECT, instance, IPSO_CONCENTRATION_MIN_MEASURED_VALUE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Min Measured Value'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_CONCENTRATION_OBJECT, instance, IPSO_CONCENTRATION_MAX_MEASURED_VALUE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Max Measured Value'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_CONCENTRATION_OBJECT, instance, IPSO_CONCENTRATION_RESET_MIN_AND_MAX_MEASURED_VALUES);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Reset Min and Max Measured Values'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_CONCENTRATION_OBJECT, instance, IPSO_CONCENTRATION_APPLICATION_TYPE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Applicatio Type'\n");
        return error;
    }
    return AwaError_Success;
}

AwaError ConcentrationObject_SetConcentration(AwaStaticClient *awaClient, AwaFloat concentration)
{
    if (awaClient != NULL)
    {
        int objectInstanceID = 0;
        AwaError error;
        ConcentrationStorage.Value = concentration;
        error = AwaStaticClient_ResourceChanged(awaClient, IPSO_CONCENTRATION_OBJECT, objectInstanceID, IPSO_CONCENTRATION_SENSOR_VALUE);

        if (error == AwaError_Success && concentration < ConcentrationStorage.MinMeasuredValue) {
          ConcentrationStorage.MinMeasuredValue = concentration;
          error = AwaStaticClient_ResourceChanged(awaClient, IPSO_CONCENTRATION_OBJECT, objectInstanceID, IPSO_CONCENTRATION_MIN_MEASURED_VALUE);
        }

        if (error == AwaError_Success && concentration > ConcentrationStorage.MaxMeasuredValue) {
          ConcentrationStorage.MaxMeasuredValue = concentration;
          AwaStaticClient_ResourceChanged(awaClient, IPSO_CONCENTRATION_OBJECT, objectInstanceID, IPSO_CONCENTRATION_MAX_MEASURED_VALUE);
        }
        return error;
    }
    else
    {
        printf("Invalid arguments passed to %s", __func__);
        return AwaError_Unspecified;
    }
}
