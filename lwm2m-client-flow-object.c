/**
 * @file
 * LightWeightM2M LWM2M Flow object.
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
#include <inttypes.h>
#include <awa/static.h>
#include "coap_abstraction.h"
#include "hmac.h"
#include "b64.h"

/***************************************************************************************************
 * Definitions
 **************************************************************************************************/

#define FLOWM2M_FLOW_OBJECT							20000
#define FLOWM2M_FLOW_OBJECT_DEVICEID				0
#define FLOWM2M_FLOW_OBJECT_PARENTID				1
#define FLOWM2M_FLOW_OBJECT_DEVICETYPE				2
#define FLOWM2M_FLOW_OBJECT_NAME					3
#define FLOWM2M_FLOW_OBJECT_DESCRIPTION				4
#define FLOWM2M_FLOW_OBJECT_FCAP					5
#define FLOWM2M_FLOW_OBJECT_LICENSEEID				6
#define FLOWM2M_FLOW_OBJECT_LICENSEECHALLENGE		7
#define FLOWM2M_FLOW_OBJECT_HASHITERATIONS			8
#define FLOWM2M_FLOW_OBJECT_LICENSEEHASH			9
#define FLOWM2M_FLOW_OBJECT_STATUS					10

#define MAX_STRING_SIZE								64
#define MAX_KEY_SIZE								64



/***************************************************************************************************
 * Globals
 **************************************************************************************************/

#define FLOW_INSTANCES 1
static char licenseeSecret[64] = "getATTtDsNBpBRnMsN7GoQ==";


/***************************************************************************************************
 * Implementation - Public
 **************************************************************************************************/
typedef struct
{
	int8_t DeviceID[16];
	size_t DeviceIDSize;
	int8_t ParentID[16];
	size_t ParentIDSize;
	char DeviceType[64];
	char Name[64];
	char Description[64];
	char FCAP[64];
	AwaInteger LicenseeID;
	uint8_t LicenseeChallenge[64];
	size_t LicenseeChallengeSize;
	AwaInteger HashIterations;
	uint8_t LicenseeHash[SHA256_HASH_LENGTH];
	size_t LicenseeHashSize;
	AwaInteger Status;
} FlowObject;

static FlowObject flow[FLOW_INSTANCES] = {0};

static bool CalculateLicenseeHash(char *licenseeSecret, uint8_t hash[SHA256_HASH_LENGTH], const char *challenge, int challengeLength, int iterations)
{
	int i;
	uint8_t key[MAX_KEY_SIZE];
	int keyLen = b64Decode(key, sizeof(key), licenseeSecret, strlen(licenseeSecret));

	if (keyLen == -1)
	{
		return false;
	}

	HmacSha256_ComputeHash(hash, challenge, challengeLength, key, keyLen);
	for (i = 1; i < iterations; i++)
	{
		HmacSha256_ComputeHash(hash, hash, SHA256_HASH_LENGTH, key, keyLen);
	}
	return true;
}

AwaResult handler(AwaStaticClient *client, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID,
	AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void **dataPointer, size_t *dataSize, bool *changed)
{
	AwaResult result = AwaResult_InternalError;

	if (!((objectID == FLOWM2M_FLOW_OBJECT) && (objectInstanceID >= 0) && (objectInstanceID < FLOW_INSTANCES)))
	{
		printf("incorrect flow object data\n");
		return result;
	}

	switch (operation)
	{
        case AwaOperation_DeleteObjectInstance:
            result = AwaResult_SuccessDeleted;
            memset(&flow[objectInstanceID], 0, sizeof(FlowObject));
            break;

		case AwaOperation_CreateObjectInstance:
			result = AwaResult_SuccessCreated;
			memset(&flow[objectInstanceID], 0, sizeof(FlowObject));
			break;

		case AwaOperation_CreateResource:
			result = AwaResult_SuccessCreated;
			break;

		case AwaOperation_Read:
			switch (resourceID)
			{
				case FLOWM2M_FLOW_OBJECT_DEVICEID:
					*dataPointer = flow[objectInstanceID].DeviceID;
					*dataSize = flow[objectInstanceID].DeviceIDSize;
					break;

				case FLOWM2M_FLOW_OBJECT_PARENTID:
					*dataPointer = flow[objectInstanceID].ParentID;
					*dataSize = flow[objectInstanceID].ParentIDSize;
					break;

				case FLOWM2M_FLOW_OBJECT_DEVICETYPE:
					*dataPointer = flow[objectInstanceID].DeviceType;
					*dataSize = strlen(flow[objectInstanceID].DeviceType) ;
					break;

				case FLOWM2M_FLOW_OBJECT_NAME:
					*dataPointer = flow[objectInstanceID].Name;
					*dataSize = strlen(flow[objectInstanceID].Name);
					break;

				case FLOWM2M_FLOW_OBJECT_DESCRIPTION:
					*dataPointer = flow[objectInstanceID].Description;
					*dataSize = strlen(flow[objectInstanceID].Description);
					break;

				case FLOWM2M_FLOW_OBJECT_FCAP:
					*dataPointer = flow[objectInstanceID].FCAP;
					*dataSize = strlen(flow[objectInstanceID].FCAP);
					break;

				case FLOWM2M_FLOW_OBJECT_LICENSEEID:
					*dataPointer = &flow[objectInstanceID].LicenseeID;
					*dataSize = sizeof(flow[objectInstanceID].LicenseeID);
					break;

				case FLOWM2M_FLOW_OBJECT_LICENSEECHALLENGE:
					*dataPointer = flow[objectInstanceID].LicenseeChallenge;
					*dataSize = flow[objectInstanceID].LicenseeChallengeSize;
					break;

				case FLOWM2M_FLOW_OBJECT_HASHITERATIONS:
					*dataPointer = &flow[objectInstanceID].HashIterations;
					*dataSize = sizeof(flow[objectInstanceID].HashIterations);
					break;

				case FLOWM2M_FLOW_OBJECT_LICENSEEHASH:
					*dataPointer = flow[objectInstanceID].LicenseeHash;
					*dataSize = flow[objectInstanceID].LicenseeHashSize;
					break;

				case FLOWM2M_FLOW_OBJECT_STATUS:
					*dataPointer = &flow[objectInstanceID].Status;
					*dataSize = sizeof(flow[objectInstanceID].Status);
					break;
				default:
					printf("Invalid resource id %d\n", resourceID);
					break;
			}
			result = AwaResult_SuccessContent;
			break;

		case AwaOperation_Write:
			switch (resourceID)
			{
				case FLOWM2M_FLOW_OBJECT_DEVICEID:
					memcpy(flow[objectInstanceID].DeviceID, *dataPointer, *dataSize);
					flow[objectInstanceID].DeviceIDSize = *dataSize;
					break;

				case FLOWM2M_FLOW_OBJECT_PARENTID:
					memcpy(flow[objectInstanceID].ParentID, *dataPointer, *dataSize);
					flow[objectInstanceID].ParentIDSize = *dataSize;
					break;

				case FLOWM2M_FLOW_OBJECT_DEVICETYPE:
					memcpy(flow[objectInstanceID].DeviceType, *dataPointer, *dataSize);
					flow[objectInstanceID].DeviceType[*dataSize] = '\0';
					break;

				case FLOWM2M_FLOW_OBJECT_NAME:
				    memcpy(flow[objectInstanceID].Name, *dataPointer, *dataSize);
				    flow[objectInstanceID].Name[*dataSize] = '\0';
					break;

				case FLOWM2M_FLOW_OBJECT_DESCRIPTION:
					memcpy(flow[objectInstanceID].Description,  *dataPointer, *dataSize);
					flow[objectInstanceID].Description[*dataSize] = '\0';
					break;

				case FLOWM2M_FLOW_OBJECT_FCAP:
				    memcpy(flow[objectInstanceID].FCAP, *dataPointer, *dataSize);
					flow[objectInstanceID].FCAP[*dataSize] = '\0';
					printf("FCAP = %s\n", flow[objectInstanceID].FCAP);
					break;

				case FLOWM2M_FLOW_OBJECT_LICENSEEID:
					flow[objectInstanceID].LicenseeID =  (*(AwaInteger*)(*dataPointer));
					break;

				case FLOWM2M_FLOW_OBJECT_LICENSEECHALLENGE:
					memcpy(flow[objectInstanceID].LicenseeChallenge, *dataPointer, *dataSize);
					flow[objectInstanceID].LicenseeChallengeSize = *dataSize;
					break;

				case FLOWM2M_FLOW_OBJECT_HASHITERATIONS:
					flow[objectInstanceID].HashIterations = *((AwaInteger*)(*dataPointer));
					break;

				case FLOWM2M_FLOW_OBJECT_LICENSEEHASH:
					memcpy(flow[objectInstanceID].LicenseeHash, *dataPointer, *dataSize);
					flow[objectInstanceID].LicenseeHashSize = *dataSize;
					break;

				case FLOWM2M_FLOW_OBJECT_STATUS:
					flow[objectInstanceID].Status = *((AwaInteger*)(*dataPointer));
					printf("Status: %li\n", flow[objectInstanceID].Status);
					*changed = true;
					break;

				default:
					printf("invalid resource id for write operation\n");
					break;
			}
			result = AwaResult_SuccessChanged;
			break;

		default:
			printf("default case for handler, operation - %d\n", operation);
			break;
	}

	if(operation == AwaOperation_Write)
	{
        if(flow[objectInstanceID].HashIterations > 0 && flow[objectInstanceID].LicenseeChallengeSize > 0 && flow[objectInstanceID].LicenseeHashSize == 0
            && (resourceID == FLOWM2M_FLOW_OBJECT_HASHITERATIONS || (resourceID == FLOWM2M_FLOW_OBJECT_LICENSEECHALLENGE)))
        {
            printf("Calculating licensee hash with %li iterations...\n", flow[objectInstanceID].HashIterations);

            if (CalculateLicenseeHash(licenseeSecret, flow[objectInstanceID].LicenseeHash, flow[objectInstanceID].LicenseeChallenge, flow[objectInstanceID].LicenseeChallengeSize, flow[objectInstanceID].HashIterations))
            {
                printf("Calculated hash, writing Licensee Hash resource...\n");
                flow[objectInstanceID].LicenseeHashSize = SHA256_HASH_LENGTH;
                AwaStaticClient_CreateResource(client, FLOWM2M_FLOW_OBJECT, 0, FLOWM2M_FLOW_OBJECT_LICENSEEHASH);
                AwaStaticClient_ResourceChanged(client, FLOWM2M_FLOW_OBJECT, 0, FLOWM2M_FLOW_OBJECT_LICENSEEHASH);
            }
            else
            {
                printf("Licensee secret is invalid\n");
            }
        }
	}

	return result;
}

int DefineFlowObject(AwaStaticClient *awaClient)
{
	AwaError error;

	error = AwaStaticClient_DefineObjectWithHandler(awaClient, "Flow", FLOWM2M_FLOW_OBJECT, 0, FLOW_INSTANCES, handler);
	if (error != AwaError_Success)
	{
		printf("Failed to register flow bject\n");
		return 1;
	}

	error = AwaStaticClient_DefineResourceWithHandler(awaClient, "DeviceID", FLOWM2M_FLOW_OBJECT, FLOWM2M_FLOW_OBJECT_DEVICEID, AwaResourceType_Opaque, 1, 1, AwaResourceOperations_ReadWrite,
	handler);
	if (error != AwaError_Success)
	{
		printf("Failed to define deviceID resource\n");
		return 1;
	}

	error = AwaStaticClient_DefineResourceWithHandler(awaClient, "ParentID",  FLOWM2M_FLOW_OBJECT, FLOWM2M_FLOW_OBJECT_PARENTID, AwaResourceType_Opaque, 0, 1, AwaResourceOperations_ReadWrite,
		handler);
	if (error != AwaError_Success)
	{
		printf("Failed to define parentID resource\n");
		return 1;
	}

	error = AwaStaticClient_DefineResourceWithHandler(awaClient, "DeviceType", FLOWM2M_FLOW_OBJECT, FLOWM2M_FLOW_OBJECT_DEVICETYPE, AwaResourceType_String, 1, 1, AwaResourceOperations_ReadWrite,
		handler);
	if (error != AwaError_Success)
	{
		printf("Failed to define device type resource\n");
		return 1;
	}

	error = AwaStaticClient_DefineResourceWithHandler(awaClient, "Name", FLOWM2M_FLOW_OBJECT, FLOWM2M_FLOW_OBJECT_NAME, AwaResourceType_String, 0, 1, AwaResourceOperations_ReadWrite,
		handler);
	if (error != AwaError_Success)
	{
		printf("Failed to define name resource\n");
		return 1;
	}

	error = AwaStaticClient_DefineResourceWithHandler(awaClient, "Description", FLOWM2M_FLOW_OBJECT, FLOWM2M_FLOW_OBJECT_DESCRIPTION, AwaResourceType_String, 0, 1, AwaResourceOperations_ReadWrite,
		handler);
	if (error != AwaError_Success)
	{
		printf("Failed to define description resource\n");
		return 1;
	}

	error = AwaStaticClient_DefineResourceWithHandler(awaClient, "FCAP", FLOWM2M_FLOW_OBJECT, FLOWM2M_FLOW_OBJECT_FCAP, AwaResourceType_String, 1, 1, AwaResourceOperations_ReadWrite,
		handler);
	if (error != AwaError_Success)
	{
		printf("Failed to define FCAP resource\n");
		return 1;
	}

	error = AwaStaticClient_DefineResourceWithHandler(awaClient, "LicenseeID", FLOWM2M_FLOW_OBJECT, FLOWM2M_FLOW_OBJECT_LICENSEEID, AwaResourceType_Integer, 1, 1, AwaResourceOperations_ReadWrite,
		handler);
	if (error != AwaError_Success)
	{
		printf("Failed to define LicenseeID resource\n");
		return 1;
	}

	error = AwaStaticClient_DefineResourceWithHandler(awaClient, "LicenseeChallenge", FLOWM2M_FLOW_OBJECT, FLOWM2M_FLOW_OBJECT_LICENSEECHALLENGE, AwaResourceType_Opaque, 0, 1, AwaResourceOperations_ReadWrite,
		handler);
	if (error != AwaError_Success)
	{
		printf("Failed to define LicenseeChallenge resource\n");
		return 1;
	}

	error = AwaStaticClient_DefineResourceWithHandler(awaClient, "HashIterations", FLOWM2M_FLOW_OBJECT, FLOWM2M_FLOW_OBJECT_HASHITERATIONS, AwaResourceType_Integer, 0, 1, AwaResourceOperations_ReadWrite,
		handler);
	if (error != AwaError_Success)
	{
		printf("Failed to define HashIterations resource\n");
		return 1;
	}

	error = AwaStaticClient_DefineResourceWithHandler(awaClient, "LicenseeHash", FLOWM2M_FLOW_OBJECT, FLOWM2M_FLOW_OBJECT_LICENSEEHASH, AwaResourceType_Opaque, 0, 1, AwaResourceOperations_ReadWrite,
		handler);
	if (error != AwaError_Success)
	{
		printf("Failed to define LicenseeHash resource\n");
		return 1;
	}

	error = AwaStaticClient_DefineResourceWithHandler(awaClient, "Status", FLOWM2M_FLOW_OBJECT, FLOWM2M_FLOW_OBJECT_STATUS, AwaResourceType_Integer, 0, 1, AwaResourceOperations_ReadWrite,
		handler);
	if (error != AwaError_Success)
	{
		printf("Failed to define Status resource\n");
		return 1;
	}

	return 0;
}
