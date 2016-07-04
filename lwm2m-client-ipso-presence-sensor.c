/**
 * @file
 * LightWeightM2M LWM2M Presence Sensor object.
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
#include "lwm2m-client-ipso-presence-sensor.h"

/***************************************************************************************************
 * Typedefs
 **************************************************************************************************/

typedef struct
{
    AwaBoolean State;
    AwaInteger Counter;
    char SensorType[128];
    AwaInteger BusyToClearDelay;
    AwaInteger ClearToBusyDelay;
} IPSOPresenceSensor;

/***************************************************************************************************
 * Globals
 **************************************************************************************************/

static IPSOPresenceSensor presenceSensors[PRESENCE_SENSORS];

/***************************************************************************************************
 * Implementation
 **************************************************************************************************/

static AwaResult PresenceSensorHandler(AwaStaticClient *client, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID,
     AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void **dataPointer, size_t *dataSize, bool *changed)
{
    AwaResult result = AwaResult_InternalError;
    if (!((objectID == IPSO_PRESENCE_OBJECT) && (objectInstanceID >= 0) && (objectInstanceID < PRESENCE_SENSORS)))
    {
        printf("Incorrect object data\n");
        return result;
    }

    switch (operation)
    {
        case AwaOperation_DeleteObjectInstance:
            result = AwaResult_SuccessDeleted;
            memset(&presenceSensors[objectInstanceID], 0, sizeof(presenceSensors[objectInstanceID]));
            break;

        case AwaOperation_CreateObjectInstance:
            result = AwaResult_SuccessCreated;
            memset(&presenceSensors[objectInstanceID], 0, sizeof(presenceSensors[objectInstanceID]));
            break;

        case AwaOperation_CreateResource:
            result = AwaResult_SuccessCreated;
            break;

        case AwaOperation_Execute:
            if (resourceID == IPSO_DIGITAL_INPUT_COUNTER_RESET)
            {
                presenceSensors[objectInstanceID].Counter = 0;
                AwaStaticClient_ResourceChanged(client, IPSO_PRESENCE_OBJECT, objectInstanceID, IPSO_DIGITAL_INPUT_COUNTER);
                result = AwaResult_Success;
            }
            else
            {
                printf("\n Invalid resource ID for PresenceSensor execute operation");
                result = AwaResult_InternalError;
            }
            break;

        case AwaOperation_Read:
            result = AwaResult_SuccessContent;
            switch (resourceID)
            {
                case IPSO_DIGITAL_INPUT_STATE:
                    *dataPointer = &presenceSensors[objectInstanceID].State;
                    *dataSize = sizeof(presenceSensors[objectInstanceID].State);
                    break;

                case IPSO_DIGITAL_INPUT_COUNTER:
                    *dataPointer = &presenceSensors[objectInstanceID].Counter;
                    *dataSize = sizeof(presenceSensors[objectInstanceID].Counter);
                    break;

                case IPSO_SENSOR_TYPE:
                    *dataPointer = presenceSensors[objectInstanceID].SensorType;
                    *dataSize = strlen(presenceSensors[objectInstanceID].SensorType);
                    break;

                case IPSO_BUSY_TO_CLEAR_DELAY:
                    *dataPointer = &presenceSensors[objectInstanceID].BusyToClearDelay;
                    *dataSize = sizeof(presenceSensors[objectInstanceID].BusyToClearDelay);
                    break;

                case IPSO_CLEAR_TO_BUSY_DELAY:
                    *dataPointer = &presenceSensors[objectInstanceID].ClearToBusyDelay;
                    *dataSize = sizeof(presenceSensors[objectInstanceID].ClearToBusyDelay);
                    break;

                default:
                    printf("\n Invalid resource ID for PresenceSensor read operation");
                    result = AwaResult_InternalError;
                    break;
            }
            break;

        case AwaOperation_Write:
            *changed = true;
            result = AwaResult_SuccessChanged;
            switch (resourceID)
            {
                case IPSO_DIGITAL_INPUT_STATE:
                    presenceSensors[objectInstanceID].State = *((AwaBoolean *)*dataPointer);
                    break;

                case IPSO_DIGITAL_INPUT_COUNTER:
                    presenceSensors[objectInstanceID].Counter = *((AwaInteger *)*dataPointer);
                    break;

                case IPSO_SENSOR_TYPE:
                    if(*dataSize < sizeof(presenceSensors[objectInstanceID].SensorType))
                    {
                        memcpy(presenceSensors[objectInstanceID].SensorType, *dataPointer, *dataSize);
                        presenceSensors[objectInstanceID].SensorType[*dataSize] = '\0';
                    }
                    else
                    {
                        result = AwaResult_BadRequest;
                    }
                    break;

                case IPSO_BUSY_TO_CLEAR_DELAY:
                    presenceSensors[objectInstanceID].BusyToClearDelay = *((AwaInteger *)*dataPointer);
                    break;

                case IPSO_CLEAR_TO_BUSY_DELAY:
                    presenceSensors[objectInstanceID].ClearToBusyDelay = *((AwaInteger *)*dataPointer);
                    break;

                default:
                    printf("\n Invalid resource ID for DigitalInput write operation");
                    result = AwaResult_InternalError;
                    break;
            }
            break;
        default:
            printf("DigitalInput - unknown operation\n");
            break;
    }
    return result;
}

int DefinePresenceSensorObject(AwaStaticClient *awaClient)
{
    AwaError error;
    int i;

    error = AwaStaticClient_SetObjectOperationHandler(awaClient, IPSO_PRESENCE_OBJECT, PresenceSensorHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to register flow access object\n");
        return 1;
    }

    error = AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_PRESENCE_OBJECT, IPSO_DIGITAL_INPUT_STATE, PresenceSensorHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define State resource\n");
        return 1;
    }

    error = AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_PRESENCE_OBJECT, IPSO_DIGITAL_INPUT_COUNTER, PresenceSensorHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define Counter resource\n");
        return 1;
    }

    error = AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_PRESENCE_OBJECT , IPSO_SENSOR_TYPE, PresenceSensorHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define SensorType resource\n");
        return 1;
    }

    error = AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_PRESENCE_OBJECT, IPSO_BUSY_TO_CLEAR_DELAY, PresenceSensorHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define BusyToClearDelay resource\n");
        return 1;
    }

    error = AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_PRESENCE_OBJECT, IPSO_CLEAR_TO_BUSY_DELAY, PresenceSensorHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define ClearToBusyDelay resource\n");
        return 1;
    }

    error = AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_PRESENCE_OBJECT , IPSO_DIGITAL_INPUT_COUNTER_RESET, PresenceSensorHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define CounterReset resource\n");
        return 1;
    }

    for (i = 0; i < PRESENCE_SENSORS; i++)
    {
        AwaStaticClient_CreateObjectInstance(awaClient, IPSO_PRESENCE_OBJECT, i);
        AwaStaticClient_CreateResource(awaClient, IPSO_PRESENCE_OBJECT, i, IPSO_DIGITAL_INPUT_COUNTER);
    }
    return 0;
}


int PresenceSensor_IncrementCounter(AwaStaticClient *awaClient, AwaObjectInstanceID objectInstanceID)
{
    if ((awaClient != NULL) && (objectInstanceID >= 0) && (objectInstanceID < PRESENCE_SENSORS))
    {
        presenceSensors[objectInstanceID].Counter += 1;
        AwaStaticClient_ResourceChanged(awaClient, IPSO_PRESENCE_OBJECT, objectInstanceID, IPSO_DIGITAL_INPUT_COUNTER);
        return 0;
    }
    else
    {
        printf("Invalid arguments passed to %s", __func__);
        return 1;
    }
}
