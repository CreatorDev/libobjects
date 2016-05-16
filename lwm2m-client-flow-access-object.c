/**
 * @file
 * LightWeightM2M LWM2M Flow Access object.
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
#include "coap_abstraction.h"

#include <awa/static.h>

/***************************************************************************************************
 * Definitions
 **************************************************************************************************/

#define FLOWM2M_FLOW_ACCESS_OBJECT                                20001
#define FLOWM2M_FLOW_ACCESS_OBJECT_URL                            0
#define FLOWM2M_FLOW_ACCESS_OBJECT_CUSTOMERKEY                    1
#define FLOWM2M_FLOW_ACCESS_OBJECT_CUSTOMERSECRET                 2
#define FLOWM2M_FLOW_ACCESS_OBJECT_REMEMBERMETOKEN                3
#define FLOWM2M_FLOW_ACCESS_OBJECT_REMEMBERMETOKENEXPIRY          4

#define FLOW_ACCESS_INSTANCES 1


typedef struct
{
    char URL[64];
    char CustomerKey[64];
    char CustomerSecret[64];
    char RememberMeToken[64];
    AwaTime RememberMeTokenExpiry;
} FlowAccessObject;

FlowAccessObject flowAccess[FLOW_ACCESS_INSTANCES];

/***************************************************************************************************
 * Implementation - Public
 **************************************************************************************************/

AwaResult accessHandler(AwaStaticClient *client, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID,
    AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void **dataPointer, size_t *dataSize, bool *changed)
{
    AwaResult result = AwaResult_InternalError;

    if (!((objectID == FLOWM2M_FLOW_ACCESS_OBJECT) && (objectInstanceID >= 0) && (objectInstanceID < FLOW_ACCESS_INSTANCES)))
    {
        printf("Incorrect flow access object data\n");
        return result;
    }

    switch (operation)
    {
        case AwaOperation_DeleteObjectInstance:
            result = AwaResult_SuccessDeleted;
            memset(&flowAccess[objectInstanceID], 0, sizeof(flowAccess[objectInstanceID]));
            break;

        case AwaOperation_CreateObjectInstance:
            result = AwaResult_SuccessCreated;
            memset(&flowAccess[objectInstanceID], 0, sizeof(flowAccess[objectInstanceID]));
            break;

        case AwaOperation_CreateResource:
            result = AwaResult_SuccessCreated;
            break;

        case AwaOperation_Read:
            switch (resourceID)
            {
                case FLOWM2M_FLOW_ACCESS_OBJECT_URL:
                    *dataPointer = flowAccess[objectInstanceID].URL;
                    *dataSize = strlen(flowAccess[objectInstanceID].URL) ;
                    result = AwaResult_SuccessContent;
                    break;

                case FLOWM2M_FLOW_ACCESS_OBJECT_CUSTOMERKEY:
                    *dataPointer = flowAccess[objectInstanceID].CustomerKey;
                    *dataSize = strlen(flowAccess[objectInstanceID].CustomerKey);
                    result = AwaResult_SuccessContent;
                    break;

                case FLOWM2M_FLOW_ACCESS_OBJECT_CUSTOMERSECRET:
                    *dataPointer = flowAccess[objectInstanceID].CustomerSecret;
                    *dataSize = strlen(flowAccess[objectInstanceID].CustomerSecret);
                    result = AwaResult_SuccessContent;
                    break;

                case FLOWM2M_FLOW_ACCESS_OBJECT_REMEMBERMETOKEN:
                    *dataPointer = flowAccess[objectInstanceID].RememberMeToken;
                    *dataSize = strlen(flowAccess[objectInstanceID].RememberMeToken);
                    result = AwaResult_SuccessContent;
                    break;

                case FLOWM2M_FLOW_ACCESS_OBJECT_REMEMBERMETOKENEXPIRY:
                    *dataPointer = &flowAccess[objectInstanceID].RememberMeTokenExpiry;
                    *dataSize = sizeof(flowAccess[objectInstanceID].RememberMeTokenExpiry);
                    result = AwaResult_SuccessContent;
                    break;

                default:
                    printf("\n Invalid res id for flow access - read op");
                    break;
            }
            break;

            case AwaOperation_Write:
                switch (resourceID)
                {
                    case FLOWM2M_FLOW_ACCESS_OBJECT_URL:
                        memcpy(flowAccess[objectInstanceID].URL, *dataPointer, *dataSize);
                        flowAccess[objectInstanceID].URL[*dataSize] = '\0';
                        *changed = true;
                        result = AwaResult_SuccessChanged;
                        break;

                    case FLOWM2M_FLOW_ACCESS_OBJECT_CUSTOMERKEY:
                        memcpy(flowAccess[objectInstanceID].CustomerKey,*dataPointer, *dataSize);
                        flowAccess[objectInstanceID].CustomerKey[*dataSize] = '\0';
                        *changed = true;
                        result = AwaResult_SuccessChanged;
                        break;

                    case FLOWM2M_FLOW_ACCESS_OBJECT_CUSTOMERSECRET:
                        memcpy(flowAccess[objectInstanceID].CustomerSecret, *dataPointer, *dataSize);
                        flowAccess[objectInstanceID].CustomerSecret[*dataSize] = '\0';
                        *changed = true;
                        result = AwaResult_SuccessChanged;
                        break;

                    case FLOWM2M_FLOW_ACCESS_OBJECT_REMEMBERMETOKEN:
                        memcpy(flowAccess[objectInstanceID].RememberMeToken, *dataPointer, *dataSize);
                        flowAccess[objectInstanceID].RememberMeToken[*dataSize] = '\0';
                        *changed = true;
                        result = AwaResult_SuccessChanged;
                        break;

                    case FLOWM2M_FLOW_ACCESS_OBJECT_REMEMBERMETOKENEXPIRY:
                        flowAccess[objectInstanceID].RememberMeTokenExpiry = *((AwaTime *)*dataPointer);
                        *changed = true;
                        result = AwaResult_SuccessChanged;
                        break;

                    default:
                        printf("Invalid res id %d for flow access - write op\n", resourceID);
                        break;
                }
                break;

        default:
            printf("Flow access - unknown operation\n");
            break;
    }
    return result;
}

int DefineFlowAccessObject(AwaStaticClient *awaClient)
{
    AwaError error;

    error = AwaStaticClient_DefineObjectWithHandler(awaClient, "FlowAccess", FLOWM2M_FLOW_ACCESS_OBJECT, 0, FLOW_ACCESS_INSTANCES, accessHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to register flow access object\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "URL", FLOWM2M_FLOW_ACCESS_OBJECT, FLOWM2M_FLOW_ACCESS_OBJECT_URL, AwaResourceType_String, 0, 1, AwaResourceOperations_ReadWrite,
        accessHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define URL resource\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "CustomerKey",  FLOWM2M_FLOW_ACCESS_OBJECT, FLOWM2M_FLOW_ACCESS_OBJECT_CUSTOMERKEY, AwaResourceType_String, 0, 1, AwaResourceOperations_ReadWrite,
        accessHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define CustomerKey resource\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "CustomerSecret", FLOWM2M_FLOW_ACCESS_OBJECT, FLOWM2M_FLOW_ACCESS_OBJECT_CUSTOMERSECRET, AwaResourceType_String, 0, 1, AwaResourceOperations_ReadWrite,
        accessHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define CustomerSecret resource\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "RememberMeToken", FLOWM2M_FLOW_ACCESS_OBJECT, FLOWM2M_FLOW_ACCESS_OBJECT_REMEMBERMETOKEN, AwaResourceType_String, 0, 1, AwaResourceOperations_ReadWrite,
        accessHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define RememberMeToken resource\n");
        return 1;
    }

    error = AwaStaticClient_DefineResourceWithHandler(awaClient, "RememberMeTokenExpiry", FLOWM2M_FLOW_ACCESS_OBJECT, FLOWM2M_FLOW_ACCESS_OBJECT_REMEMBERMETOKENEXPIRY, AwaResourceType_Time, 0, 1, AwaResourceOperations_ReadWrite,
        accessHandler);
    if (error != AwaError_Success)
    {
        printf("Failed to define RememberMeTokenExpiry resource\n");
        return 1;
    }

    return 0;
}
