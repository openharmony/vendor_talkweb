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
#include "hdf_log.h"
#include "spi_if.h"
#include "cmsis_os2.h"
#include "samgr_lite.h"
#include "ohos_run.h"

#define HDF_SPI_STACK_SIZE 0x1000
#define HDF_SPI_TASK_NAME "hdf_spi_test_task"
#define HDF_SPI_TASK_PRIORITY 25
uint8_t txBuffer[] = "welcome to OpenHarmony\n";
#define WIP_FLAG       0x01
#define SPI_FLASH_IDx  0x4018
#define Countof(a)      (sizeof(a)/sizeof(*(a)))
#define bufferSize      (Countof(txBuffer) - 1)

uint8_t rxBuffer[bufferSize] = {0};
#define USE_TRANSFER_API 1

static uint8_t BufferCmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
    while (BufferLength--) {
        if (*pBuffer1 != *pBuffer2) {
            return 0;
        }
        pBuffer1++;
        pBuffer2++;
    }
    return 1;
}

#if (USE_TRANSFER_API == 1)
static uint16_t ReadDeviceId(DevHandle spiHandle)
{
    struct SpiMsg msg;
    uint16_t deviceId = 0;
    uint8_t rbuff[5] = { 0 };
    uint8_t wbuff[5] = { 0xAB, 0xFF, 0xFF, 0xFF, 0xFF };
    int32_t ret = 0;
    msg.wbuf = wbuff;
    msg.rbuf = rbuff;
    msg.len = sizeof(wbuff);
    msg.keepCs = 0;
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
    } else {
        deviceId = rbuff[4];
    }

    return deviceId;
}

static uint16_t ReadFlashId(DevHandle spiHandle)
{
    int32_t ret = 0;
    uint16_t flashId = 0;
    uint8_t rbuff1[4] = { 0 };
    uint8_t wbuff1[4] = { 0x9f, 0xFF, 0xFF, 0xFF };
    struct SpiMsg msg1 = {0};
    msg1.wbuf = wbuff1;
    msg1.rbuf = rbuff1;
    msg1.len = sizeof(wbuff1);
    msg1.keepCs = 0;
    msg1.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg1, 1);
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
    } else {
        flashId = (msg1.rbuf[2]<<8)|msg1.rbuf[3];
    }
    return flashId;
}

static void WaitForWriteEnd(DevHandle spiHandle)
{
    uint8_t FLASH_Status = 0;
    /* Send "Read Status Register" instruction */
    uint8_t wbuf[1] = {0x05};
    uint8_t wbuf1[1] = {0xff};
    uint8_t rbuf[1] = {0};
    struct SpiMsg msg = {0};
    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 1;
    msg.delayUs = 0;
    int32_t ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
    }
    
    /* Loop as long as the memory is busy with a write cycle */
    do {
        msg.wbuf = wbuf1;
        msg.rbuf = rbuf;
        msg.len = sizeof(wbuf1);
        msg.keepCs = 1;
        msg.delayUs = 0;

        ret = SpiTransfer(spiHandle, &msg, 1);
        if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
        }
        FLASH_Status = rbuf[0];
    }
    while ((FLASH_Status & WIP_FLAG) == 1); /* Write in progress */
    msg.wbuf = wbuf1;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf1);
    msg.keepCs = 0;
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
    }
}

static void WriteEnable(DevHandle spiHandle)
{
    uint8_t wbuf[1] = {0x06};
    uint8_t rbuf[1] = {0};
    struct SpiMsg msg = {0};
    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 0;
    msg.delayUs = 0;
    int32_t ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
    }
}

static void BufferWrite(DevHandle spiHandle, const uint8_t* buf, uint32_t size)
{
    WriteEnable(spiHandle);
    uint8_t wbuf[4] = {0x02, 0x00, 0x00, 0x00};
    uint8_t rbuf[4] = {0};
    uint8_t *rbuf1 = NULL;
    int32_t ret = 0;

    struct SpiMsg msg = {0};
    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 1;
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
    }

    rbuf1 = (uint8_t*)OsalMemAlloc(size);
    if (rbuf1 == NULL) {
        HDF_LOGE("OsalMemAlloc failed.\n");
        return;
    }
    
    memset_s(rbuf1, size, 0, size);
    msg.wbuf = buf;
    msg.rbuf = rbuf1;
    msg.len = size;
    msg.keepCs = 0;
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
    }

    WaitForWriteEnd(spiHandle);
    
    OsalMemFree(rbuf1);
}

static void BufferRead(DevHandle spiHandle, uint8_t* buf, uint32_t size)
{
    int32_t ret = 0;

    uint8_t wbuf[4] = {0x03, 0x00, 0x00, 0x00};
    uint8_t rbuf[4] = {0};
    struct SpiMsg msg = {0};
    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 1;
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
        return;
    }
    uint8_t *wbuf1 = (uint8_t*)OsalMemAlloc(size);
    if (wbuf1 == NULL) {
        HDF_LOGE("OsalMemAlloc failed.\n");
        return;
    }
    memset_s(wbuf1, size, 0xff, size);
    msg.wbuf = wbuf1;
    msg.rbuf = buf;
    msg.len = size;
    msg.keepCs = 0;
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
        return;
    }

    OsalMemFree(wbuf1);
}

static void SectorErase(DevHandle spiHandle)
{
    WriteEnable(spiHandle);
    WaitForWriteEnd(spiHandle);
    uint8_t wbuf[4] = {0x20, 0x00, 0x00, 0x00};
    uint8_t rbuf[4] = {0};
    struct SpiMsg msg = {0};
    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = sizeof(wbuf);
    msg.keepCs = 0;
    msg.delayUs = 0;
    int32_t ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
        return;
    }
    WaitForWriteEnd(spiHandle);
}
#else
static uint16_t ReadDeviceId(DevHandle spiHandle)
{
    struct SpiMsg msg;
    uint16_t deviceId = 0;
    uint8_t rbuff1[2] = { 0 };
    uint8_t wbuff1[5] = {0x00, 0xAB, 0xff, 0xff, 0xff};
    int32_t ret = 0;

    ret =SpiWrite(spiHandle, wbuff1, 5);
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }
    rbuff1[0] = 0x01;
    ret = SpiRead(spiHandle, rbuff1, 2);
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }

    deviceId = rbuff1[1];

    return deviceId;
}

static uint16_t ReadFlashId(DevHandle spiHandle)
{
    int32_t ret = 0;
    uint16_t flashId = 0;
    uint8_t wbuff[2] = {0x00, 0x9f};
    uint8_t rbuff[4] = {0};
    ret =SpiWrite(spiHandle, wbuff, sizeof(wbuff));
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }
    rbuff[0] = 0x01;
    ret = SpiRead(spiHandle, rbuff, 4);
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }
    flashId = (rbuff[2] << 8)|rbuff[3];
    return flashId;
}

static void WaitForWriteEnd(DevHandle spiHandle)
{
    uint8_t FLASH_Status = 0;
    int32_t ret = 0;
    uint8_t wbuff[2] = {0x00, 0x05};
    uint8_t rbuff[2] = {0};
    ret =SpiWrite(spiHandle, wbuff, sizeof(wbuff));
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }
    do {
        rbuff[0] = 0;
        ret = SpiRead(spiHandle, rbuff, 2);
        if (ret != 0) {
            HDF_LOGE("SpiRead: failed, ret %d\n", ret);
        }
        FLASH_Status = rbuff[1];
    } while ((FLASH_Status & WIP_FLAG) == 1); /* Write in progress */

    uint8_t wbuff1[1] = {0x01};
    ret =SpiWrite(spiHandle, wbuff1, sizeof(wbuff1));
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }
}
static void WriteEnable(DevHandle spiHandle)
{
    uint8_t FLASH_Status = 0;
    int32_t ret = 0;
    uint8_t wbuff[2] = {0x01, 0x06};
    ret =SpiWrite(spiHandle, wbuff, sizeof(wbuff));
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }

    return;
}

static void BufferWrite(DevHandle spiHandle, const uint8_t* buf, uint32_t size)
{
    WriteEnable(spiHandle);
    uint8_t wbuf[5] = {0x01, 0x02, 0x00, 0x00, 0x00};
    uint8_t rbuf[4] = {0};
    uint8_t *wbuf1 = NULL;
    int32_t ret = 0;
    wbuf1 = (uint8_t*)OsalMemAlloc(size + sizeof(wbuf));

    ret = strncpy_s(wbuf1, size + sizeof(wbuf), wbuf, sizeof(wbuf));
    if (ret < 0) {
        HDF_LOGE("strncpy wbuf failed, ret %d\n", ret);
    }
    ret = strncpy_s(wbuf1 + sizeof(wbuf), size, buf, size);
    if (ret < 0) {
        HDF_LOGE("strncpy buf failed, ret %d\n", ret);
    }
    ret = SpiWrite(spiHandle, wbuf1, size + sizeof(wbuf));
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }

    WaitForWriteEnd(spiHandle);
    if (wbuf1!= NULL) {
        OsalMemFree(wbuf1);
    }
}

static void BufferRead(DevHandle spiHandle, uint8_t* buf, uint32_t size)
{
    WriteEnable(spiHandle);
    uint8_t wbuf[5] = {0x00, 0x03, 0x00, 0x00, 0x00};
    uint8_t *rbuf = NULL;
    int32_t ret = 0;
    rbuf = (uint8_t*)OsalMemAlloc(size + 1);

    ret = SpiWrite(spiHandle, wbuf, sizeof(wbuf));
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }
    rbuf[0] = 0x01;
    ret = SpiRead(spiHandle, rbuf, size + 1);
    if (ret != 0) {
        HDF_LOGE("SpiRead: failed, ret %d\n", ret);
    }

    strncpy_s(buf, size, rbuf + 1, size);

    if (rbuf!= NULL) {
        OsalMemFree(rbuf);
    }

    return;
}

static void SectorErase(DevHandle spiHandle)
{
    WriteEnable(spiHandle);
    WaitForWriteEnd(spiHandle);
    uint8_t wbuf[5] = {0x01, 0x20, 0x00, 0x00, 0x00};
    uint8_t rbuf[5] = {0};
    int32_t ret = 0;
    ret = SpiWrite(spiHandle, wbuf, sizeof(wbuf));
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }
    WaitForWriteEnd(spiHandle);
}

#endif

static void* HdfSpiTestEntry(void* arg)
{
    (void)arg;
#ifdef USE_SET_CFG
    int32_t ret;
    struct SpiCfg cfg;
#endif
    uint16_t flashId = 0;
    uint16_t deviceId = 0;
    struct SpiDevInfo spiDevinfo;
    DevHandle spiHandle;
    spiDevinfo.busNum = 0;
    spiDevinfo.csNum = 0;
    spiHandle = SpiOpen(&spiDevinfo);
    if (spiHandle == NULL) {
        HDF_LOGE("SpiOpen: failed\n");
        return NULL;
    }
#ifdef USE_SET_CFG
    ret = SpiGetCfg(spiHandle, &cfg);
    if (ret != 0) {
        HDF_LOGE("SpiGetCfg: failed, ret %d\n", ret);
        goto err;
    }
    HDF_LOGI("speed:%d, bitper:%d, mode:%d, transMode:%d\n", cfg.maxSpeedHz, cfg.bitsPerWord, cfg.mode, \
        cfg.transferMode);
    cfg.maxSpeedHz = 1;
    cfg.bitsPerWord = 8;
    cfg.mode = 0;
    cfg.transferMode = 1;
    ret = SpiSetCfg(spiHandle, &cfg);
    if (ret != 0) {
        HDF_LOGE("SpiSetCfg: failed, ret %d\n", ret);
        goto err;
    }
    SpiClose(spiHandle);
    spiHandle = SpiOpen(&spiDevinfo);
    if (spiHandle == NULL) {
        HDF_LOGE("SpiOpen: failed\n");
        return NULL;
    }
#endif
    deviceId = ReadDeviceId(spiHandle);
    HDF_LOGI("read device id is 0x%02x\n", deviceId);
    flashId = ReadFlashId(spiHandle);
    HDF_LOGI("read flash id is 0x%02x\n", flashId);

    if (flashId == SPI_FLASH_IDx) {
        SectorErase(spiHandle);
        BufferWrite(spiHandle, txBuffer, bufferSize);
        HDF_LOGI("send buffer is %s\n", txBuffer);
        BufferRead(spiHandle, rxBuffer, bufferSize);
        HDF_LOGI("recv send buffer is %s\n", rxBuffer);

        if (BufferCmp(txBuffer, rxBuffer, bufferSize)) {
            HDF_LOGI("hdf spi write read flash success\n");
        } else {
            HDF_LOGI("hdf spi write read flash failed\n");
        }
    }
#ifdef USE_SET_CFG
err:
#endif
    SpiClose(spiHandle);
    return NULL;
}

void StartHdfSpiTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_SPI_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_SPI_STACK_SIZE;
    attr.priority = HDF_SPI_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfSpiTestEntry, NULL, &attr) == NULL) {
        printf("Failed to create thread1!\n");
    }
}

OHOS_APP_RUN(StartHdfSpiTest);
