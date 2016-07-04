/**
 * @file
 * LightWeightM2M LWM2M Presence object.
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
#include "lwm2m-client-ipso-presence.h"

/***************************************************************************************************
 * Typedefs
 **************************************************************************************************/

typedef struct
{
    AwaBoolean State;
    AwaInteger StateCounter;
    char SensorType[8];
    AwaInteger BusyToClearDelay;
    AwaInteger ClearToBusyDelay;
} IPSOPresence;

/***************************************************************************************************
 * Globals
 **************************************************************************************************/

static IPSOPresence PresenceStorage = {
  .SensorType = "IR PIR"
};

/***************************************************************************************************
 * Implementation
 **************************************************************************************************/

static AwaResult PresenceObject_Handler(AwaStaticClient *client, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID,
     AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void **dataPointer, size_t *dataSize, bool *changed)
{
    AwaResult result = AwaResult_InternalError;
    if ((objectID != IPSO_PRESENCE_OBJECT) || (objectInstanceID != 0))
    {
        printf("Incorrect object data\n");
        return result;
    }

    switch (operation)
    {
        case AwaOperation_CreateObjectInstance:
            result = AwaResult_SuccessDeleted;
            memset(&PresenceStorage, 0, sizeof(PresenceStorage));
            strncpy(PresenceStorage.SensorType, "IR PIR", 8);
            PresenceStorage.SensorType[7] = 0;
            result = AwaResult_SuccessCreated;
            break;

        case AwaOperation_CreateResource:
            result = AwaResult_SuccessCreated;
            break;

        case AwaOperation_Execute:
            if (resourceID == IPSO_PRESENCE_DIGITAL_INPUT_COUNTER_RESET)
            {
                PresenceStorage.StateCounter = 0;
                AwaStaticClient_ResourceChanged(client, IPSO_PRESENCE_OBJECT, objectInstanceID, IPSO_PRESENCE_DIGITAL_INPUT_COUNTER);
                result = AwaResult_Success;
            }
            else
            {
                printf("\n Invalid resource ID for Presence execute operation");
                result = AwaResult_InternalError;
            }
            break;

        case AwaOperation_DeleteObjectInstance:
        case AwaOperation_Read:
        case AwaOperation_Write:
        default:
            printf("Presence - unknown operation (%d)\n", operation);
            result = AwaResult_InternalError;
            break;
    }
    return result;
}

AwaError PresenceObject_DefineObjectsAndResources(AwaStaticClient *awaClient, AwaInteger busyToClearDelay, AwaInteger clearToBusyDelay)
{
    PresenceStorage.BusyToClearDelay = busyToClearDelay;
    PresenceStorage.ClearToBusyDelay = clearToBusyDelay;
    AwaError error;

    error = AwaStaticClient_DefineObject(awaClient, IPSO_PRESENCE_OBJECT, "Presence", 0, 1);
    AwaStaticClient_SetObjectOperationHandler(awaClient, IPSO_PRESENCE_OBJECT, PresenceObject_Handler);
    if (error != AwaError_Success)
    {
        printf("Failed to register Presence IPSO object\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_PRESENCE_OBJECT, IPSO_PRESENCE_DIGITAL_INPUT_STATE, "Digital Input State",
        AwaResourceType_Boolean, 1, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_PRESENCE_OBJECT, IPSO_PRESENCE_DIGITAL_INPUT_STATE, &PresenceStorage.State,
        sizeof(PresenceStorage.State), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Digital Input State' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_PRESENCE_OBJECT, IPSO_PRESENCE_DIGITAL_INPUT_COUNTER, "Digital Input Counter",
        AwaResourceType_String, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_PRESENCE_OBJECT, IPSO_PRESENCE_DIGITAL_INPUT_COUNTER, &PresenceStorage.StateCounter,
        sizeof(PresenceStorage.StateCounter), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Digital Input Counter' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_PRESENCE_OBJECT, IPSO_PRESENCE_DIGITAL_INPUT_COUNTER_RESET, "Digital Input Counter Reset",
        AwaResourceType_Opaque, 0, 1, AwaResourceOperations_Execute);
    AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_PRESENCE_OBJECT, IPSO_PRESENCE_DIGITAL_INPUT_COUNTER_RESET, PresenceObject_Handler);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Digital Input Counter Reset' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_PRESENCE_OBJECT, IPSO_PRESENCE_BUSY_TO_CLEAR_DELAY, "Busy to Clear delay",
        AwaResourceType_Integer, 0, 1, AwaResourceOperations_ReadWrite);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_PRESENCE_OBJECT, IPSO_PRESENCE_BUSY_TO_CLEAR_DELAY, &PresenceStorage.BusyToClearDelay,
        sizeof(PresenceStorage.BusyToClearDelay), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Busy to Clear delay' resource\n");
        return error;
    }

    error = AwaStaticClient_DefineResource(awaClient, IPSO_PRESENCE_OBJECT, IPSO_PRESENCE_CLEAR_TO_BUSY_DELAY, "Clear to Busy delay",
        AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_PRESENCE_OBJECT, IPSO_PRESENCE_CLEAR_TO_BUSY_DELAY, &PresenceStorage.ClearToBusyDelay,
        sizeof(PresenceStorage.ClearToBusyDelay), 0);
    if (error != AwaError_Success)
    {
        printf("Failed to define 'Clear to Busy delay' resource\n");
        return error;
    }

    AwaStaticClient_CreateObjectInstance(awaClient, IPSO_PRESENCE_OBJECT, 0);
    return AwaError_Success;
}


AwaError PresenceObject_SetPresence(AwaStaticClient *awaClient, AwaBoolean state)
{
    if (awaClient != NULL)
    {
        AwaError error;
        if (state != PresenceStorage.State) {
            PresenceStorage.State = state;
            error = AwaStaticClient_ResourceChanged(awaClient, IPSO_PRESENCE_OBJECT, 0, IPSO_PRESENCE_DIGITAL_INPUT_STATE);

            if (error == AwaError_Success) {
                PresenceStorage.StateCounter ++;
                error = AwaStaticClient_ResourceChanged(awaClient, IPSO_PRESENCE_OBJECT, 0, IPSO_PRESENCE_DIGITAL_INPUT_COUNTER);
            }
        }
        return AwaError_Success;
    }
    else
    {
        printf("Invalid arguments passed to %s", __func__);
        return AwaError_Unspecified;
    }
}
