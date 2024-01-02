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
#include <string.h>
#include <unistd.h>

#include "ohos_run.h"
#include "cmsis_os2.h"

#define THREAD_STACK_SIZE (1024 * 4)
#define FIRST_THREAD_NAME "firstThread"
#define SECOND_THREAD_NAME "secondThread"
#define THIRD_THREAD_NAME "thirdThread"
#define FIRST_THREAD_PRIORITY 26
#define SECOND_THREAD_PRIORITY 25
#define THIRD_THREAD_PRIORITY 24

osMutexId_t mutex_id;

static void FirstThread(void)
{
    osDelay(100U);
    while (1) {
        osMutexAcquire(mutex_id, osWaitForever);
        printf("FirstThread is Acquire.\r\n");
        osDelay(1000U);
        osMutexRelease(mutex_id);
        return;
    }
}

static void TwoThread(void)
{
    osDelay(1000U);
    while (1) {
        printf("TwoThread is Acquire.\r\n");
        osDelay(1000U);
        return;
    }
}

static void ThreeThread(void)
{
    while (1) {
        osMutexAcquire(mutex_id, osWaitForever);
        printf("ThreeThread is Acquire.\r\n");
        osDelay(3000U);
        osMutexRelease(mutex_id);
        return;
    }
}

static void OsMutexExample(void)
{
    osThreadAttr_t attr;

    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = THREAD_STACK_SIZE;

    attr.name = FIRST_THREAD_NAME;
    attr.priority = FIRST_THREAD_PRIORITY;
    if (osThreadNew((osThreadFunc_t)FirstThread, NULL, &attr) == NULL) {
        printf("create FirstThread failed!\n");
    }

    attr.name = SECOND_THREAD_NAME;
    attr.priority = SECOND_THREAD_PRIORITY;
    if (osThreadNew((osThreadFunc_t)TwoThread, NULL, &attr) == NULL) {
        printf("create TwoThread failed!\n");
    }

    attr.name = THIRD_THREAD_NAME;
    attr.priority = THIRD_THREAD_PRIORITY;
    if (osThreadNew((osThreadFunc_t)ThreeThread, NULL, &attr) == NULL) {
        printf("create ThreeThread failed!\n");
    }

    mutex_id = osMutexNew(NULL);
    if (mutex_id == NULL) {
        printf("create Mutex failed!\n");
    }
}

OHOS_APP_RUN(OsMutexExample);
