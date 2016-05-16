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
#include <awa/static.h>
#include "lwm2m-client-ipso-digital-input.h"

/***************************************************************************************************
 * Typedefs
 **************************************************************************************************/

typedef struct
{
    AwaBoolean State;
    AwaInteger Counter;
    AwaBoolean Polarity;
    AwaTime DebouncePeriod;
    AwaInteger EdgeSelection;
    char ApplicationType[128];
    char SensorType[128];
} IPSODigitalInput;

/***************************************************************************************************
 * Globals
 **************************************************************************************************/

static IPSODigitalInput digitalInputs[DIGITAL_INPUTS];

/***************************************************************************************************
 * Implementation
 **************************************************************************************************/

static AwaResult digitalInputHandler(AwaStaticClient *client, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID,
     AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void **dataPointer, size_t *dataSize, bool *changed)
{
    AwaResult result = AwaResult_InternalError;
    if (!((objectID == IPSO_DIGITAL_INPUT_OBJECT) && (objectInstanceID >= 0) && (objectInstanceID < DIGITAL_INPUTS)))
    {
        printf("Incorrect object data\n");
        return result;
    }

    switch (operation)
    {
        case AwaOperation_DeleteObjectInstance:
            result = AwaResult_SuccessDeleted;
            memset(&digitalInputs[objectInstanceID], 0, sizeof(digitalInputs[objectInstanceID]));
            break;

        case AwaOperation_CreateObjectInstance:
            result = AwaResult_SuccessCreated;
            memset(&digitalInputs[objectInstanceID], 0, sizeof(digitalInputs[objectInstanceID]));
            break;

        case AwaOperation_CreateResource:
            result = AwaResult_SuccessCreated;
            break;

        case AwaOperation_Execute:
            if (resourceID == IPSO_DIGITAL_INPUT_COUNTER_RESET)
            {
                digitalInputs[objectInstanceID].Counter = 0;
                AwaStaticClient_ResourceChanged(client, IPSO_DIGITAL_INPUT_OBJECT, objectInstanceID, IPSO_DIGITAL_INPUT_COUNTER);
                result = AwaResult_Success;
            }
            else
            {
                printf("\n Invalid resource ID for DigitalInput execute operation");
                result = AwaResult_InternalError;
            }
            break;

        case AwaOperation_Read:
            result = AwaResult_SuccessContent;
            switch (resourceID)
            {
                case IPSO_DIGITAL_INPUT_STATE:
                    *dataPointer = &digitalInputs[objectInstanceID].State;
                    *dataSize = sizeof(digitalInputs[objectInstanceID].State) ;
                    break;

                case IPSO_DIGITAL_INPUT_COUNTER:
                    *dataPointer = &digitalInputs[objectInstanceID].Counter;
                    *dataSize = sizeof(digitalInputs[objectInstanceID].Counter) ;
                    break;

                case IPSO_DIGITAL_INPUT_POLARITY:
                    *dataPointer = &digitalInputs[objectInstanceID].Polarity;
                    *dataSize = sizeof(digitalInputs[objectInstanceID].Polarity) ;
                    break;

                case IPSO_DIGITAL_INPUT_DEBOUNCE_PERIOD:
                    *dataPointer = &digitalInputs[objectInstanceID].DebouncePeriod;
                    *dataSize = sizeof(digitalInputs[objectInstanceID].DebouncePeriod) ;
                    break;

                case IPSO_DIGITAL_INPUT_EDGE_SELECTION:
                    *dataPointer = &digitalInputs[objectInstanceID].EdgeSelection;
                    *dataSize = sizeof(digitalInputs[objectInstanceID].EdgeSelection) ;
                    break;

                case IPSO_APPLICATION_TYPE:
                    *dataPointer = digitalInputs[objectInstanceID].ApplicationType;
                    *dataSize = strlen(digitalInputs[objectInstanceID].ApplicationType) ;
                    break;

                case IPSO_SENSOR_TYPE:
                    *dataPointer = digitalInputs[objectInstanceID].SensorType;
                    *dataSize = strlen(digitalInputs[objectInstanceID].SensorType) ;
                    break;

                default:
                    printf("\n Invalid resource ID for DigitalInput read operation");
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
                    digitalInputs[objectInstanceID].State = *((AwaBoolean *)*dataPointer);
                    break;

                case IPSO_DIGITAL_INPUT_COUNTER:
                    digitalInputs[objectInstanceID].Counter = *((AwaInteger *)*dataPointer);
                    break;

                case IPSO_DIGITAL_INPUT_POLARITY:
                    digitalInputs[objectInstanceID].Polarity = *((AwaBoolean *)*dataPointer);
                    break;

                case IPSO_DIGITAL_INPUT_DEBOUNCE_PERIOD:
                    digitalInputs[objectInstanceID].DebouncePeriod = *((AwaTime *)*dataPointer);
                    break;

                case IPSO_DIGITAL_INPUT_EDGE_SELECTION:
                    digitalInputs[objectInstanceID].EdgeSelection = *((AwaInteger *)*dataPointer);
                    break;

                case IPSO_APPLICATION_TYPE:
                    if(*dataSize < sizeof(digitalInputs[objectInstanceID].ApplicationType))
                    {
                        memcpy(digitalInputs[objectInstanceID].ApplicationType, *dataPointer, *dataSize);
                        digitalInputs[objectInstanceID].ApplicationType[*dataSize] = '\0';
                    }
                    else
                    {
                        result = AwaResult_BadRequest;
                    }
                    break;

                case IPSO_SENSOR_TYPE:
                    if(*dataSize < sizeof(digitalInputs[objectInstanceID].SensorType))
                    {
                        memcpy(digitalInputs[objectInstanceID].SensorType, *dataPointer, *dataSize);
                        digitalInputs[objectInstanceID].SensorType[*dataSize] = '\0';
                    }
                    else
                    {
                        result = AwaResult_BadRequest;
                    }
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

int DefineDigitalInputObject(AwaStaticClient *awaClient)
{
    AwaError error;
    int i;

    error = AwaStaticClient_DefineObjectWithHandler(awaClient, "DigitalInput", IPSO_DIGITAL_INPUT_OBJECT, 0, DIGITAL_INPUTS, digitalInputHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to register flow access object\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "State", IPSO_DIGITAL_INPUT_OBJECT, IPSO_DIGITAL_INPUT_STATE, AwaResourceType_Boolean, 1, 1, AwaResourceOperations_ReadOnly, digitalInputHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define State resource\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "Counter", IPSO_DIGITAL_INPUT_OBJECT, IPSO_DIGITAL_INPUT_COUNTER, AwaResourceType_Integer, 0, 1, AwaResourceOperations_ReadOnly, digitalInputHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define Counter resource\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "Polarity", IPSO_DIGITAL_INPUT_OBJECT, IPSO_DIGITAL_INPUT_POLARITY, AwaResourceType_Boolean, 0, 1, AwaResourceOperations_ReadWrite, digitalInputHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define Polarity resource\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "DebouncePeriod", IPSO_DIGITAL_INPUT_OBJECT, IPSO_DIGITAL_INPUT_DEBOUNCE_PERIOD, AwaResourceType_Time, 0, 1, AwaResourceOperations_ReadWrite, digitalInputHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define DebouncePeriod resource\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "EdgeSelection", IPSO_DIGITAL_INPUT_OBJECT, IPSO_DIGITAL_INPUT_EDGE_SELECTION, AwaResourceType_Integer, 0, 1, AwaResourceOperations_ReadWrite, digitalInputHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define EdgeSelection resource\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "ApplicationType", IPSO_DIGITAL_INPUT_OBJECT, IPSO_APPLICATION_TYPE, AwaResourceType_String, 0, 1, AwaResourceOperations_ReadOnly, digitalInputHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define ApplicationType resource\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "SensorType", IPSO_DIGITAL_INPUT_OBJECT , IPSO_SENSOR_TYPE, AwaResourceType_String, 0, 1, AwaResourceOperations_ReadOnly, digitalInputHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define SensorType resource\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "CounterReset", IPSO_DIGITAL_INPUT_OBJECT , IPSO_DIGITAL_INPUT_COUNTER_RESET, AwaResourceType_Opaque, 0, 1, AwaResourceOperations_Execute, digitalInputHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define CounterReset resource\n");
        return 1;
    }

    for (i = 0; i < DIGITAL_INPUTS; i++)
    {
        AwaStaticClient_CreateObjectInstance(awaClient, IPSO_DIGITAL_INPUT_OBJECT, i);
        AwaStaticClient_CreateResource(awaClient, IPSO_DIGITAL_INPUT_OBJECT, i, IPSO_DIGITAL_INPUT_COUNTER);
    }
    return 0;
}


int DigitalInput_IncrementCounter(AwaStaticClient *awaClient, AwaObjectInstanceID objectInstanceID)
{
    if ((awaClient != NULL) && (objectInstanceID >= 0) && (objectInstanceID < DIGITAL_INPUTS))
    {
        digitalInputs[objectInstanceID].Counter += 1;
        AwaStaticClient_ResourceChanged(awaClient, IPSO_DIGITAL_INPUT_OBJECT, objectInstanceID, IPSO_DIGITAL_INPUT_COUNTER);
        return 0;
    }
    else
    {
        printf("Invalid arguments passed to %s", __func__);
        return 1;
    }
}
