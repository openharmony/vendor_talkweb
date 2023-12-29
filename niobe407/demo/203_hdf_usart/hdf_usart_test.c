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
#include "samgr_lite.h"
#include "ohos_run.h"

#define HDF_USART_STACK_SIZE 0x1000
#define HDF_USART_TASK_NAME "hdf_usart_test_task"
#define HDF_USART_TASK_PRIORITY 25
#define UART_PORT_NUM (5)
#define DEFAULT_BAUD_RATE (115200)
#define TX_BUFFER_SIZE (256)
#define RX_BUFFER_SIZE (256)
#define DELAY_TIME_MS (100)

uint8_t txbuf[80] = "this is usart test function\n";
uint8_t rxbuf[80] = {0};
uint8_t len = 0;
uint8_t tmp;
static void UartTestConfiguration(DevHandle handle)
{
    int32_t ret;
    uint32_t baudRate;
    ret = UartGetBaud(handle, &baudRate);
    if (ret != 0) {
        HDF_LOGE("UartGetBaud: failed, ret %d\n", ret);
    }
    baudRate = DEFAULT_BAUD_RATE;
    ret = UartSetBaud(handle, baudRate);
    if (ret != 0) {
        HDF_LOGE("UartGetBaud: failed, ret %d\n", ret);
    }
    struct UartAttribute attribute = {0};
    ret = UartGetAttribute(handle, &attribute);
    if (ret != 0) {
        HDF_LOGE("UartGetAttribute: failed, ret %d\n", ret);
    }
    attribute.dataBits = UART_ATTR_DATABIT_8;
    attribute.fifoTxEn = 1;
    attribute.fifoRxEn = 1;
    attribute.parity = UART_ATTR_PARITY_NONE;
    attribute.stopBits = UART_ATTR_STOPBIT_1;
    attribute.cts = 0;
    attribute.rts = 0;
    ret = UartSetAttribute(handle, &attribute);
    if (ret != 0) {
        HDF_LOGE("UartSetAttribute: failed, ret %d\n", ret);
    }
}

static void UartTestCommunication(DevHandle handle)
{
    uint8_t txbuf[TX_BUFFER_SIZE] = "Hello, UART!";
    uint8_t rxbuf[RX_BUFFER_SIZE] = {0};
    int32_t ret;
    size_t len;

    len = strlen((char *)txbuf);
    ret = UartWrite(handle, txbuf, len);
    if (ret != 0) {
        HDF_LOGE("UartWrite: failed, ret %d\n", ret);
        return;
    }
    while (1) {
        ret = UartRead(handle, rxbuf, len);
        if (ret < 0) {
            HDF_LOGE("UartRead: failed, ret %d\n", ret);
            return;
        } else if (ret > 0) {
            HDF_LOGI("UartRead: content length is %d is :%s\n", ret, rxbuf);
        }
        osDelay(DELAY_TIME_MS);
        return;
    }
}

static void* HdfUsartTestEntry(void* arg)
{
    (void *)arg;
    DevHandle handle = UartOpen(UART_PORT_NUM);
    if (handle == NULL) {
        HDF_LOGE("UartOpen %u: failed!\n", UART_PORT_NUM);
        return NULL;
    }
    
    UartTestConfiguration(handle);
    UartTestCommunication(handle);
    
    UartClose(handle);
    return NULL;
}

static void StartHdfUsartTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_USART_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_USART_STACK_SIZE;
    attr.priority = HDF_USART_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfUsartTestEntry, NULL, &attr) == NULL) {
        printf("Failed to create thread1!\n");
    }
}

OHOS_APP_RUN(StartHdfUsartTest);

