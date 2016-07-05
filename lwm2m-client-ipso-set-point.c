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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <awa/static.h>
#include "lwm2m-client-ipso-set-point.h"

/***************************************************************************************************
 * Typedefs
 **************************************************************************************************/

typedef struct
{
    AwaFloat Value;
    char Units[13];
    char ApplicationType[30];
} IPSOSetPoint;

/***************************************************************************************************
 * Globals
 **************************************************************************************************/

static IPSOSetPoint SetPointStorage = {
  .Value = 0,
  .Units = "Celsius deg",
  .ApplicationType = "Temperature Delta Trigger"
};

/***************************************************************************************************
 * Implementation
 **************************************************************************************************/
static AwaResult SetPointObject_Handler(AwaStaticClient *client, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID,
     AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void **dataPointer, size_t *dataSize, bool *changed)
{
    AwaResult result = AwaResult_InternalError;
    if (objectID != IPSO_SET_POINT_OBJECT)
    {
        printf("Incorrect object data\n");
        return result;
    }

    switch (operation)
    {
        case AwaOperation_CreateObjectInstance:
             return AwaResult_SuccessCreated;

        case AwaOperation_CreateResource:
            result = AwaResult_SuccessCreated;
            break;

        case AwaOperation_Read:
            result = AwaResult_SuccessContent;
            switch (resourceID)
            {
                case IPSO_SET_POINT_VALUE:
                    *dataPointer = &SetPointStorage.Value;
                    *dataSize = sizeof(SetPointStorage.Value);
                    break;

                default:
                    printf("\n Invalid resource ID for SetPoint read operation");
                    result = AwaResult_InternalError;
                    break;
            }
            break;

        case AwaOperation_Write:
            result = AwaResult_SuccessChanged;
            switch (resourceID)
            {
                case IPSO_SET_POINT_VALUE:
                    SetPointStorage.Value = **((AwaFloat **)dataPointer);
                    *changed = true;
                    break;

                default:
                    printf("\n Invalid resource ID for SetPoint write operation");
                    result = AwaResult_InternalError;
                    break;
            }
            break;

        default:
            printf("SetPoint - unknown operation\n");
            break;
    }
    return result;
}

AwaError SetPointObject_DefineObjectsAndResources(AwaStaticClient *awaClient)
{
    AwaError error;

    error = AwaStaticClient_DefineObject(awaClient, IPSO_SET_POINT_OBJECT, "Set Point", 0, 1);
    if (error != AwaError_Success)
    {
        printf("Failed to register Set Point IPSO object\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_SET_POINT_OBJECT, IPSO_SET_POINT_VALUE, "Value", AwaResourceType_Float, 1, 1,
        AwaResourceOperations_ReadWrite);
    AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_SET_POINT_OBJECT, IPSO_SET_POINT_VALUE, SetPointObject_Handler);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Value' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_SET_POINT_OBJECT, IPSO_SET_POINT_UNITS, "Units", AwaResourceType_String, 0, 1,
        AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_SET_POINT_OBJECT, IPSO_SET_POINT_UNITS, SetPointStorage.Units,
        strlen(SetPointStorage.Units), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Units' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_SET_POINT_OBJECT, IPSO_SET_POINT_APPLICATION_TYPE, "Application Type",
        AwaResourceType_String, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_SET_POINT_OBJECT, IPSO_SET_POINT_APPLICATION_TYPE, SetPointStorage.ApplicationType,
        strlen(SetPointStorage.ApplicationType), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Application Type' resource\n");
        return error;
    }


    int instance = 0;
    error = AwaStaticClient_CreateObjectInstance(awaClient, IPSO_SET_POINT_OBJECT, instance);
    if (error != AwaError_Success) {
        // if the object instance already exists,
        // the object already contains a maximum number of object instances,
        // or if no object is defined for the specified object ID.
        printf("Failed to CreateObjectInstance\n");
        return error;
    }

    //Since it's mandatory resource, you don't need to create it explicit. Awa will handle this for you
    //error = AwaStaticClient_CreateResource(awaClient, IPSO_SET_POINT_OBJECT, instance, IPSO_SET_POINT_VALUE);
    error = AwaStaticClient_CreateResource(awaClient, IPSO_SET_POINT_OBJECT, instance, IPSO_SET_POINT_UNITS);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Units'\n");
        return error;
    }

    error = AwaStaticClient_CreateResource(awaClient, IPSO_SET_POINT_OBJECT, instance, IPSO_SET_POINT_APPLICATION_TYPE);
    if (error != AwaError_Success)
    {
        printf("Failed to create resource 'Application Type'\n");
        return error;
    }
    return AwaError_Success;
}

AwaError SetPointObject_SetValue(AwaStaticClient *awaClient, AwaFloat value)
{
    if (awaClient != NULL)
    {
        int objectInstanceID = 0;
        AwaError error;
        SetPointStorage.Value = value;
        error = AwaStaticClient_ResourceChanged(awaClient, IPSO_SET_POINT_OBJECT, objectInstanceID, IPSO_SET_POINT_VALUE);
        return error;
    }
    else
    {
        printf("Invalid arguments passed to %s", __func__);
        return AwaError_Unspecified;
    }
}

AwaFloat SetPointObject_GetValue()
{
    return SetPointStorage.Value;
}
