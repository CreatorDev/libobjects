/**
 * @file
 * LightWeightM2M LWM2M Digital Output object.
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
#include "lwm2m-client-ipso-digital-output.h"

/***************************************************************************************************
* Typedefs
**************************************************************************************************/
typedef struct
{
   AwaBoolean State[2];
   AwaBoolean Polarity[2];
   char ApplicationType[30];
   DigitalOutputCallback ValueChangeCallback;
} IPSODigitalOutput;

/***************************************************************************************************
* Globals
**************************************************************************************************/

static IPSODigitalOutput DigitalOutputStorage = {
 .State = {0, 0},
 .Polarity = {0, 0},
 .ApplicationType = "Digital output"
};

/***************************************************************************************************
* Implementation
**************************************************************************************************/
AwaResult DigitalOutput_Handler(AwaStaticClient *client, AwaOperation operation, AwaObjectID objectID,
  AwaObjectInstanceID objectInstanceID, AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID,
  void **dataPointer, size_t *dataSize, bool *changed)
{
    printf("UPDATE: objectID=%d, AwaObjectInstanceID=%d, resourceID=%d, resourceInstanceID=%d\n", objectID, objectInstanceID, resourceID, resourceInstanceID);
   AwaResult result = AwaResult_InternalError;
   if (objectID != IPSO_DIGITAL_OUTPUT_OBJECT)
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
               case IPSO_DIGITAL_OUTPUT_STATE:
                   *dataPointer = &DigitalOutputStorage.State[resourceInstanceID];
                   *dataSize = sizeof(AwaBoolean);
                   break;

               case IPSO_DIGITAL_OUTPUT_POLARITY:
                   *dataPointer = &DigitalOutputStorage.Polarity[resourceInstanceID];
                   *dataSize = sizeof(AwaBoolean);
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
               case IPSO_DIGITAL_OUTPUT_STATE:
                   DigitalOutputStorage.State[resourceInstanceID] = **((AwaBoolean **)dataPointer);
                   *changed = true;
                   if (DigitalOutputStorage.ValueChangeCallback != NULL) {
                       DigitalOutputStorage.ValueChangeCallback(
                         DIGITAL_OUTPUT_CALLBACK_TYPE_STATE,
                         resourceInstanceID,
                         DigitalOutputStorage.State[resourceInstanceID]);
                   }
                   break;

               case IPSO_DIGITAL_OUTPUT_POLARITY:
                   DigitalOutputStorage.Polarity[resourceInstanceID] = **((AwaBoolean **)dataPointer);
                   *changed = true;
                   if (DigitalOutputStorage.ValueChangeCallback != NULL) {
                       DigitalOutputStorage.ValueChangeCallback(
                         DIGITAL_OUTPUT_CALLBACK_TYPE_POLARITY,
                         resourceInstanceID,
                         DigitalOutputStorage.State[resourceInstanceID]);
                   }
                   break;

               default:
                   printf("\n Invalid resource ID for SetPoint write operation");
                   result = AwaResult_InternalError;
                   break;
           }
           break;

       default:
           printf("Digital Output - unknown operation\n");
           break;
   }
   return result;
}

AwaError DigitalOutput_DefineObject(AwaStaticClient *awaClient, DigitalOutputCallback callback) {

   AwaError error;
   DigitalOutputStorage.ValueChangeCallback = callback;

   error = AwaStaticClient_DefineObject(awaClient, IPSO_DIGITAL_OUTPUT_OBJECT, "Digital Output", 2, 2);
   if (error != AwaError_Success)
   {
       printf("Failed to register Digital Output IPSO object\n");
       return error;
   }

   error = AwaStaticClient_DefineResource(awaClient, IPSO_DIGITAL_OUTPUT_OBJECT, IPSO_DIGITAL_OUTPUT_STATE, "State",
      AwaResourceType_Boolean, 1, 1, AwaResourceOperations_ReadWrite);
   AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_DIGITAL_OUTPUT_OBJECT, IPSO_DIGITAL_OUTPUT_STATE,
     DigitalOutput_Handler);
   if (error != AwaError_Success)
   {
       printf("Failed to define 'State' resource\n");
       return error;
   }

   error = AwaStaticClient_DefineResource(awaClient, IPSO_DIGITAL_OUTPUT_OBJECT, IPSO_DIGITAL_OUTPUT_POLARITY,
     "Polarity", AwaResourceType_String, 1, 1, AwaResourceOperations_ReadWrite);
   AwaStaticClient_SetResourceOperationHandler(awaClient, IPSO_DIGITAL_OUTPUT_OBJECT, IPSO_DIGITAL_OUTPUT_POLARITY,
      DigitalOutput_Handler);
   if (error != AwaError_Success)
   {
       printf("Failed to define 'Polarity' resource\n");
       return error;
   }

   error = AwaStaticClient_DefineResource(awaClient, IPSO_DIGITAL_OUTPUT_OBJECT, IPSO_DIGITAL_OUTPUT_APPLICATION_TYPE,
     "Application Type", AwaResourceType_String, 1, 1, AwaResourceOperations_ReadOnly);
   AwaStaticClient_SetResourceStorageWithPointer(awaClient, IPSO_DIGITAL_OUTPUT_OBJECT, IPSO_DIGITAL_OUTPUT_APPLICATION_TYPE,
      DigitalOutputStorage.ApplicationType, strlen(DigitalOutputStorage.ApplicationType), 0);
   if (error != AwaError_Success)
   {
       printf("Failed to define 'Application Type' resource\n");
       return error;
   }


   int instance = 0;
   error = AwaStaticClient_CreateObjectInstance(awaClient, IPSO_DIGITAL_OUTPUT_OBJECT, instance);
   if (error != AwaError_Success) {
       // if the object instance already exists,
       // the object already contains a maximum number of object instances,
       // or if no object is defined for the specified object ID.
       printf("Failed to CreateObjectInstance\n");
       return error;
   }

   return AwaError_Success;
}

AwaBoolean DigitalOutput_GetState(int instanceId)
{
   return DigitalOutputStorage.State[instanceId];
}
