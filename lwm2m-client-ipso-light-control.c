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

#include "awa/static.h"
#include "lwm2m-client-ipso-light-control.h"

/***************************************************************************************************
 * Definitions
 **************************************************************************************************/

#define IPSO_LIGHT_CONTROL_OBJECT                        3311
#define IPSO_LIGHT_CONTROL_ON_OFF                        5850
#define IPSO_LIGHT_CONTROL_DIMMER                        5851
#define IPSO_LIGHT_CONTROL_COLOUR                        5706
#define IPSO_LIGHT_CONTROL_UNITS                         5701
#define IPSO_LIGHT_CONTROL_ON_TIME                       5852
#define IPSO_LIGHT_CONTROL_CUMULATIVE_ACTIVE_POWER       5805
#define IPSO_LIGHT_CONTROL_POWER_FACTOR                  5820

#define LIGHT_CONTROLS                                   2

/***************************************************************************************************
 * Typedefs
 **************************************************************************************************/

typedef struct
{
    AwaBoolean OnOff;
    AwaInteger Dimmer;
    char Colour[64];
    char Units[64];
    AwaInteger OnTime;
    AwaFloat CumulativeActivePower;
    AwaFloat PowerFactor;
    LightControlCallBack callback;
    void *context;
} IPSOLightControl;

/***************************************************************************************************
 * Globals
 **************************************************************************************************/

static IPSOLightControl lightControls[LIGHT_CONTROLS];

/***************************************************************************************************
 * Implementation
 **************************************************************************************************/

static AwaResult lightControlHandler(AwaStaticClient *client, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID,
     AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void **dataPointer, size_t *dataSize, bool *changed)
{
    AwaResult result = AwaResult_InternalError;
    bool callCallback = false;
    if (!((objectID == IPSO_LIGHT_CONTROL_OBJECT) && (objectInstanceID >= 0) && (objectInstanceID < LIGHT_CONTROLS)))
    {
        printf("Incorrect object data\n");
        return result;
    }

    switch (operation)
    {
        case AwaOperation_DeleteObjectInstance:
            result = AwaResult_SuccessDeleted;
            memset(&lightControls[objectInstanceID], 0, sizeof(lightControls[objectInstanceID]));
            break;

        case AwaOperation_CreateObjectInstance:
            result = AwaResult_SuccessCreated;
            memset(&lightControls[objectInstanceID], 0, sizeof(lightControls[objectInstanceID]));
            break;

        case AwaOperation_CreateResource:
            result = AwaResult_SuccessCreated;
            break;

        case AwaOperation_Read:
            result = AwaResult_SuccessContent;
            switch (resourceID)
            {
                case IPSO_LIGHT_CONTROL_ON_OFF:
                    *dataPointer = &lightControls[objectInstanceID].OnOff;
                    *dataSize = sizeof(lightControls[objectInstanceID].OnOff) ;
                    break;

                case IPSO_LIGHT_CONTROL_DIMMER:
                    *dataPointer = &lightControls[objectInstanceID].Dimmer;
                    *dataSize = sizeof(lightControls[objectInstanceID].Dimmer) ;
                    break;

                case IPSO_LIGHT_CONTROL_COLOUR:
                    *dataPointer = lightControls[objectInstanceID].Colour;
                    *dataSize = sizeof(lightControls[objectInstanceID].Colour) ;
                    break;

                case IPSO_LIGHT_CONTROL_UNITS:
                    *dataPointer = lightControls[objectInstanceID].Units;
                    *dataSize = sizeof(lightControls[objectInstanceID].Units) ;
                    break;

                case IPSO_LIGHT_CONTROL_ON_TIME:
                    *dataPointer = &lightControls[objectInstanceID].OnTime;
                    *dataSize = sizeof(lightControls[objectInstanceID].OnTime) ;
                    break;

                case IPSO_LIGHT_CONTROL_CUMULATIVE_ACTIVE_POWER:
                    *dataPointer = &lightControls[objectInstanceID].CumulativeActivePower;
                    *dataSize = sizeof(lightControls[objectInstanceID].CumulativeActivePower) ;
                    break;

                case IPSO_LIGHT_CONTROL_POWER_FACTOR:
                    *dataPointer = &lightControls[objectInstanceID].PowerFactor;
                    *dataSize = sizeof(lightControls[objectInstanceID].PowerFactor) ;
                    break;

                default:
                    printf("\n Invalid resource ID for LightControl read operation");
                    result = AwaResult_InternalError;
                    break;
            }
            break;

        case AwaOperation_Write:
            *changed = true;
            result = AwaResult_SuccessChanged;
            switch (resourceID)
            {
                case IPSO_LIGHT_CONTROL_ON_OFF:
                    lightControls[objectInstanceID].OnOff = *((AwaBoolean *)*dataPointer);
                    callCallback = true;
                    break;

                case IPSO_LIGHT_CONTROL_DIMMER:
                    lightControls[objectInstanceID].Dimmer = *((AwaInteger *)*dataPointer);
                    callCallback = true;
                    break;

                case IPSO_LIGHT_CONTROL_COLOUR:
                    if(*dataSize < sizeof(lightControls[objectInstanceID].Colour))
                    {
                        memcpy(lightControls[objectInstanceID].Colour, *dataPointer, *dataSize);
                        lightControls[objectInstanceID].Colour[*dataSize] = '\0';
                        callCallback = true;
                    }
                    else
                    {
                        result = AwaResult_BadRequest;
                    }
                    break;

                case IPSO_LIGHT_CONTROL_UNITS:
                    if(*dataSize < sizeof(lightControls[objectInstanceID].Units))
                    {
                        memcpy(lightControls[objectInstanceID].Units, *dataPointer, *dataSize);
                        lightControls[objectInstanceID].Units[*dataSize] = '\0';
                    }
                    else
                    {
                        result = AwaResult_BadRequest;
                    }
                    break;

                case IPSO_LIGHT_CONTROL_ON_TIME:
                    lightControls[objectInstanceID].OnTime = *((AwaInteger *)*dataPointer);
                    break;

                case IPSO_LIGHT_CONTROL_CUMULATIVE_ACTIVE_POWER:
                    lightControls[objectInstanceID].CumulativeActivePower = *((AwaFloat *)*dataPointer);
                    break;

                case IPSO_LIGHT_CONTROL_POWER_FACTOR:
                    lightControls[objectInstanceID].PowerFactor = *((AwaFloat *)*dataPointer);
                    break;

                default:
                    printf("\n Invalid resource ID for LightControl write operation");
                    result = AwaResult_InternalError;
                    break;
            }
            if (lightControls[objectInstanceID].callback != NULL && callCallback)
            {
                lightControls[objectInstanceID].callback(lightControls[objectInstanceID].context,
                    lightControls[objectInstanceID].OnOff, lightControls[objectInstanceID].Dimmer,
                    lightControls[objectInstanceID].Colour);
            }
            break;
        default:
            printf("LightControl - unknown operation\n");
            break;
    }
    return result;
}

int DefineLightControlObject(AwaStaticClient *awaClient)
{
    AwaError error;
    error = AwaStaticClient_DefineObjectWithHandler(awaClient, "LightControl", IPSO_LIGHT_CONTROL_OBJECT, 0, LIGHT_CONTROLS, lightControlHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to register light control object\n");
        return -1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "On/Off", IPSO_LIGHT_CONTROL_OBJECT, IPSO_LIGHT_CONTROL_ON_OFF, AwaResourceType_Boolean, 1, 1, AwaResourceOperations_ReadWrite, lightControlHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define On/Off resource\n");
        return -1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "Dimmer", IPSO_LIGHT_CONTROL_OBJECT, IPSO_LIGHT_CONTROL_DIMMER, AwaResourceType_Integer, 0, 1, AwaResourceOperations_ReadWrite, lightControlHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define Dimmer resource\n");
        return -1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "Colour", IPSO_LIGHT_CONTROL_OBJECT, IPSO_LIGHT_CONTROL_COLOUR, AwaResourceType_String, 0, 1, AwaResourceOperations_ReadWrite, lightControlHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define Colour resource\n");
        return -1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "Units", IPSO_LIGHT_CONTROL_OBJECT, IPSO_LIGHT_CONTROL_UNITS, AwaResourceType_String, 1, 1, AwaResourceOperations_ReadOnly, lightControlHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define Units resource\n");
        return -1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "OnTime", IPSO_LIGHT_CONTROL_OBJECT, IPSO_LIGHT_CONTROL_ON_TIME, AwaResourceType_Integer, 0, 1, AwaResourceOperations_ReadWrite, lightControlHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define OnTime resource\n");
        return -1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "CumulativeActivePower", IPSO_LIGHT_CONTROL_OBJECT, IPSO_LIGHT_CONTROL_CUMULATIVE_ACTIVE_POWER, AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly, lightControlHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define CumulativeActivePower resource\n");
        return -1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "PowerFactor", IPSO_LIGHT_CONTROL_OBJECT , IPSO_LIGHT_CONTROL_POWER_FACTOR, AwaResourceType_Float, 0, 1, AwaResourceOperations_ReadOnly, lightControlHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define PowerFactor resource\n");
        return -1;
    }

    return 0;
}

int LightControl_AddLightControl(AwaStaticClient *awaClient, ObjectInstanceIDType objectInstanceID,
    LightControlCallBack callback, void *callbackContext)
{
    if ((awaClient == NULL) || (objectInstanceID < 0) || (objectInstanceID >= LIGHT_CONTROLS))
    {
        printf("Invalid arguments passed to %s", __func__);
        return -1;
    }

    AwaStaticClient_CreateObjectInstance(awaClient, IPSO_LIGHT_CONTROL_OBJECT, objectInstanceID);
    AwaStaticClient_CreateResource(awaClient, IPSO_LIGHT_CONTROL_OBJECT, objectInstanceID, IPSO_LIGHT_CONTROL_COLOUR);
    AwaStaticClient_CreateResource(awaClient, IPSO_LIGHT_CONTROL_OBJECT, objectInstanceID, IPSO_LIGHT_CONTROL_ON_TIME);
    lightControls[objectInstanceID].OnOff = false;
    snprintf(lightControls[objectInstanceID].Colour, sizeof(lightControls[objectInstanceID].Colour), "Red%d", objectInstanceID+1);
    lightControls[objectInstanceID].callback = callback;
    lightControls[objectInstanceID].context = callbackContext;
    if (callback != NULL)
    {
        lightControls[objectInstanceID].callback(lightControls[objectInstanceID].context,
            lightControls[objectInstanceID].OnOff, lightControls[objectInstanceID].Dimmer,
            lightControls[objectInstanceID].Colour);
    }
    return 0;
}

int LightControl_IncrementOnTime(AwaStaticClient *awaClient, ObjectInstanceIDType objectInstanceID, AwaInteger seconds)
{
    if ((awaClient == NULL) || (objectInstanceID < 0) || (objectInstanceID >= LIGHT_CONTROLS))
    {
        printf("Invalid arguments passed to %s", __func__);
        return -1;
    }

    if(lightControls[objectInstanceID].OnOff != true)
    {
        return -1;
    }

    lightControls[objectInstanceID].OnTime += seconds;
    return 0;
}
