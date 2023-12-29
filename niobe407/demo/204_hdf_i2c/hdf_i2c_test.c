/*
 * Copyright (c) 2022 Talkweb Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <hdf_log.h>
#include <uart_if.h>
#include "cmsis_os2.h"
#include "i2c_if.h"
#include "los_compiler.h"
#include "los_task.h"
#include "ohos_run.h"
#include "samgr_lite.h"

#define HDF_I2C_READ 1
#define HDF_I2C_WRITE 0
#define RJGT102_ADDR 0x68

#define RJGT_CMD_REGISTER_ADDR 0xB0
#define RJGT_MEM_REGISTER_ADDR 0xC0
#define RJGT_OBJ_REGISTER_ADDR 0xB2
#define RJGT_UUID_MEM_ADDR 0x90
#define RJGT_STATUS_REGISTER_ADDR 0xB3

#define RJGT_CMD_WRITE_UUID 0xAA
#define RJGT_CMD_READ_MEM 0x0F

#define DELAY_TICKS 100
#define STACK_SIZE 4096
#define TASK_PRIO 2

static osThreadId_t g_i2cThreadId = NULL;
static DevHandle g_i2cHandle = NULL;

static int I2cWriteData(DevHandle handle, unsigned int devAddr, unsigned char *buffer, unsigned int length)
{
    struct I2cMsg msg = {0};

    msg.addr = devAddr;
    msg.buf = buffer;
    msg.len = length;
    msg.flags = HDF_I2C_WRITE;

    if (I2cTransfer(handle, &msg, 1) != 1) {
        HDF_LOGE("[%s]: I2cWriteData fail \r\n", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int I2cReadData(DevHandle handle, unsigned int devAddr, unsigned char *buffer, unsigned int length)
{
    I2cWriteData(handle, devAddr, buffer, 1);

    struct I2cMsg msg = {0};
    msg.addr = devAddr;
    msg.buf = buffer;
    msg.len = length;
    msg.flags = HDF_I2C_READ;

    if (I2cTransfer(handle, &msg, 1) != 1) {
        HDF_LOGE("[%s]: I2cWriteData fail \r\n", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static void WriteRJGTCmdReg(unsigned char regVal)
{
    unsigned char buffer[2] = {0};

    buffer[0] = RJGT_CMD_REGISTER_ADDR;
    buffer[1] = regVal;
    I2cWriteData(g_i2cHandle, RJGT102_ADDR, buffer, sizeof(buffer));
}

static void WriteRJGTMemBuf(unsigned char *buf, unsigned int len)
{
    unsigned char buffer[len + 1];

    buffer[0] = RJGT_MEM_REGISTER_ADDR;
    memcpy_s(&buffer[1], len, buf, len);
    I2cWriteData(g_i2cHandle, RJGT102_ADDR, buffer, sizeof(buffer));
}

static void ReadRJGTMemBuf(unsigned char *buf, unsigned int len)
{
    buf[0] = RJGT_MEM_REGISTER_ADDR;
    I2cReadData(g_i2cHandle, RJGT102_ADDR, buf, len);
}

static unsigned char GetRJGTWriteStateReg(void)
{
    unsigned char buffer[1] = {RJGT_STATUS_REGISTER_ADDR};

    I2cReadData(g_i2cHandle, RJGT102_ADDR, buffer, sizeof(buffer));
    return buffer[0];
}

static void WriteRJGTObjReg(unsigned char regVal)
{
    unsigned char buffer[2] = {0};

    buffer[0] = RJGT_OBJ_REGISTER_ADDR;
    buffer[1] = regVal;
    I2cWriteData(g_i2cHandle, RJGT102_ADDR, buffer, sizeof(buffer));
}

static bool Write8ByteUUID(unsigned char *buf, unsigned int len)
{
    WriteRJGTCmdReg(0x00);

    WriteRJGTMemBuf(buf, len);

    WriteRJGTCmdReg(RJGT_CMD_WRITE_UUID);
    osDelay(DELAY_TICKS);

    int ret = GetRJGTWriteStateReg();
    if (ret != 0x01) {
        HDF_LOGE("Write8ByteUUID fail[%02X]\r\n", ret);
        return false;
    }
    return true;
}

static bool Read8ByteUUID(unsigned char *buf, unsigned int len)
{
    WriteRJGTCmdReg(0x00);

    WriteRJGTObjReg(RJGT_UUID_MEM_ADDR);

    WriteRJGTCmdReg(RJGT_CMD_READ_MEM);
    osDelay(DELAY_TICKS);

    int ret = GetRJGTWriteStateReg();
    if (ret != 0X01) {
        HDF_LOGE("Read8ByteUUID fail[%02X]\r\n", ret);
        return false;
    }

    ReadRJGTMemBuf(buf, len);
    return true;
}

static void ExitThread(void)
{
    if (g_i2cHandle != NULL) {
        I2cClose(g_i2cHandle);
    }

    osThreadTerminate(g_i2cThreadId);
    g_i2cThreadId = NULL;
}

static void *HdfI2cTestEntry(void *arg)
{
    (void *)arg;
    unsigned char ret = 0;
    unsigned char busId = 3;

    g_i2cHandle = I2cOpen(busId);
    if (g_i2cHandle == NULL) {
        HDF_LOGE("%s: Open I2c:%u fail!\r\n", __func__, busId);
        ExitThread();
        return;
    }

    osDelay(DELAY_TICKS);
    unsigned char uuidBuf[8] = {0x11, 0x22, 0x33, 0x44, 0xaa, 0xbb, 0xcc, 0xdd};
    if (Write8ByteUUID(uuidBuf, sizeof(uuidBuf)) != true) {
        ExitThread();
        return;
    }

    osDelay(DELAY_TICKS);
    memset_s(uuidBuf, sizeof(uuidBuf), 0, sizeof(uuidBuf));
    if (Read8ByteUUID(uuidBuf, sizeof(uuidBuf)) != true) {
        ExitThread();
        return;
    }

    while (1) {
        osDelay(DELAY_TICKS);
        return;
    }
}

static void StartHdfI2cTest(void)
{
    UINT32 uwRet;
    UINT32 taskID;
    TSK_INIT_PARAM_S stTask = {0};

    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)HdfI2cTestEntry;
    stTask.uwStackSize = STACK_SIZE;
    stTask.pcName = "HdfI2cTestEntry";
    stTask.usTaskPrio = TASK_PRIO;
    uwRet = LOS_TaskCreate(&taskID, &stTask);
    if (uwRet != LOS_OK) {
        HDF_LOGE("Task1 create failed\n");
    }
}

OHOS_APP_RUN(StartHdfI2cTest);
