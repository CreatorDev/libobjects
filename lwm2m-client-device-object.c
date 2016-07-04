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

#define LWM2M_DEVICE_OBJECT								            3
#define LWM2M_DEVICE_OBJECT_MANUFACTURER                            0
#define LWM2M_DEVICE_OBJECT_MODEL_NUMBER                            1
#define LWM2M_DEVICE_OBJECT_SERIAL_NUMBER                           2
#define LWM2M_DEVICE_OBJECT_FIRMWARE_VERSION                        3
#define LWM2M_DEVICE_OBJECT_REBOOT                                  4
#define LWM2M_DEVICE_OBJECT_FACTORY_RESET                           5
#define LWM2M_DEVICE_OBJECT_AVAILABLE_POWER_SOURCES                 6
#define LWM2M_DEVICE_OBJECT_POWER_SOURCE_VOLTAGE                    7
#define LWM2M_DEVICE_OBJECT_POWER_SOURCE_CURRENT                    8
#define LWM2M_DEVICE_OBJECT_BATTERY_LEVEL                           9
#define LWM2M_DEVICE_OBJECT_MEMORY_FREE                             10
#define LWM2M_DEVICE_OBJECT_ERROR_CODE                              11
#define LWM2M_DEVICE_OBJECT_RESET_ERROR_CODE                        12
#define LWM2M_DEVICE_OBJECT_CURRENT_TIME                            13
#define LWM2M_DEVICE_OBJECT_UTC_OFFSET                              14
#define LWM2M_DEVICE_OBJECT_TIME_ZONE                               15
#define LWM2M_DEVICE_OBJECT_SUPPORTED_BINDING_AND_MODES             16
#define LWM2M_DEVICE_OBJECT_DEVICE_TYPE                             17
#define LWM2M_DEVICE_OBJECT_HARDWARE_VERSION                        18
#define LWM2M_DEVICE_OBJECT_SOFTWARE_VERSION                        19
#define LWM2M_DEVICE_OBJECT_BATTERY_STATUS                          20
#define LWM2M_DEVICE_OBJECT_MEMORY_TOTAL                            21


#define LWM2M_DEVICE_OBJECT_POWER_SOURCES_COUNT         7
#define LWM2M_DEVICE_OBJECT_ERROR_CODE_COUNT            2

typedef struct
{
	char Manufacturer[64];
	char ModelNumber[64];
	char SerialNumber[64];
	char FirmwareVersion[64];
	AwaInteger AvailablePowerSources[LWM2M_DEVICE_OBJECT_POWER_SOURCES_COUNT];
	AwaInteger PowerSourceVoltage[LWM2M_DEVICE_OBJECT_POWER_SOURCES_COUNT];
	AwaInteger PowerSourceCurrent[LWM2M_DEVICE_OBJECT_POWER_SOURCES_COUNT];
	AwaInteger BatteryLevel;
	AwaInteger MemoryFree;
	AwaInteger ErrorCode[LWM2M_DEVICE_OBJECT_ERROR_CODE_COUNT];
	AwaTime CurrentTime;
	char UTCOffset[16];
	char Timezone[16];
	char SupportedBindingandModes[4];
	char DeviceType[64];
	char HardwareVersion[64];
	char SoftwareVersion[64];
	AwaInteger BatteryStatus;
	AwaInteger MemoryTotal;
} DeviceObject;

DeviceObject DeviceObjectStorage = { .SupportedBindingandModes = "U",
                                     .SoftwareVersion = VERSION};

/***************************************************************************************************
 * Implementation - Public
 **************************************************************************************************/

AwaResult executeHandler(AwaStaticClient *client, AwaOperation operation, AwaObjectID objectID, AwaObjectInstanceID objectInstanceID,
    AwaResourceID resourceID, AwaResourceInstanceID resourceInstanceID, void **dataPointer, size_t *dataSize, bool *changed)
{
    AwaResult result = AwaResult_Unspecified;


    printf("EXECUTE handler operation %d\n", operation);

    switch(operation)
    {
        case AwaOperation_CreateResource:
            result = AwaResult_SuccessCreated;
            break;
    }

    return result;
}


int DefineDeviceObject(AwaStaticClient *awaClient)
{
    AwaStaticClient_DefineObject(awaClient, LWM2M_DEVICE_OBJECT, "Device", 1, 1);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_MANUFACTURER,                DeviceObjectStorage.Manufacturer,             sizeof(DeviceObjectStorage.Manufacturer),             0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_MODEL_NUMBER,                DeviceObjectStorage.ModelNumber,              sizeof(DeviceObjectStorage.ModelNumber),              0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_SERIAL_NUMBER,               DeviceObjectStorage.SerialNumber,             sizeof(DeviceObjectStorage.SerialNumber),             0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_FIRMWARE_VERSION,            DeviceObjectStorage.FirmwareVersion,          sizeof(DeviceObjectStorage.FirmwareVersion),          0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_AVAILABLE_POWER_SOURCES,     DeviceObjectStorage.AvailablePowerSources,    sizeof(AwaInteger),                                   0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_POWER_SOURCE_VOLTAGE,        DeviceObjectStorage.PowerSourceVoltage,       sizeof(AwaInteger),                                   0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_POWER_SOURCE_CURRENT,        DeviceObjectStorage.PowerSourceCurrent,       sizeof(AwaInteger),                                   0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_BATTERY_LEVEL,               &DeviceObjectStorage.BatteryLevel,            sizeof(AwaInteger),                                   0 );
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_MEMORY_FREE,                 &DeviceObjectStorage.BatteryStatus,           sizeof(AwaInteger),                                   0 );
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_ERROR_CODE,                  DeviceObjectStorage.ErrorCode,                sizeof(AwaInteger),                                   0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_CURRENT_TIME,                &DeviceObjectStorage.CurrentTime,             sizeof(AwaTime),                                      0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_UTC_OFFSET,                  DeviceObjectStorage.UTCOffset,                sizeof(DeviceObjectStorage.UTCOffset),                0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_TIME_ZONE,                   DeviceObjectStorage.Timezone,                 sizeof(DeviceObjectStorage.Timezone),                 0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_SUPPORTED_BINDING_AND_MODES, DeviceObjectStorage.SupportedBindingandModes, sizeof(DeviceObjectStorage.SupportedBindingandModes), 0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_DEVICE_TYPE,                 DeviceObjectStorage.DeviceType,               sizeof(DeviceObjectStorage.DeviceType),               0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_HARDWARE_VERSION,            DeviceObjectStorage.HardwareVersion,          sizeof(DeviceObjectStorage.DeviceType),               0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_SOFTWARE_VERSION,            DeviceObjectStorage.SoftwareVersion,          sizeof(DeviceObjectStorage.SoftwareVersion),          0);
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_BATTERY_STATUS,              &DeviceObjectStorage.BatteryStatus ,          sizeof(AwaInteger),                                   0 );
    AwaStaticClient_SetResourceStorageWithPointer(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_MEMORY_TOTAL,                &DeviceObjectStorage.MemoryTotal ,            sizeof(AwaInteger),                                   0 );

    AwaStaticClient_SetResourceOperationHandler(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_REBOOT,           executeHandler);
    AwaStaticClient_SetResourceOperationHandler(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_FACTORY_RESET,    executeHandler);
    AwaStaticClient_SetResourceOperationHandler(awaClient, LWM2M_DEVICE_OBJECT, LWM2M_DEVICE_OBJECT_RESET_ERROR_CODE, executeHandler);

    AwaStaticClient_CreateObjectInstance(awaClient, LWM2M_DEVICE_OBJECT, 0);

//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_MANUFACTURER);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_MODEL_NUMBER);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_SERIAL_NUMBER);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_FIRMWARE_VERSION);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_AVAILABLE_POWER_SOURCES);
//	AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_POWER_SOURCE_VOLTAGE);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_POWER_SOURCE_CURRENT);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_BATTERY_LEVEL);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_MEMORY_FREE);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_CURRENT_TIME);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_UTC_OFFSET);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_TIME_ZONE);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_DEVICE_TYPE);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_HARDWARE_VERSION);
    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_SOFTWARE_VERSION);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_BATTERY_STATUS);
//    AwaStaticClient_CreateResource(awaClient, LWM2M_DEVICE_OBJECT, 0, LWM2M_DEVICE_OBJECT_MEMORY_TOTAL);

    return 0;
}
