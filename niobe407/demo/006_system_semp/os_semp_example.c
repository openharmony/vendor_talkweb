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

#define SEM_INITIAL_VALUE 3
#define SEM_MAX_VALUE 0
#define ATTR_BITS 0U
#define CB_SIZE_0U 0U
#define STACK_SIZE (1024 * 4)
#define PRIORITY_LEVEL 10

osSemaphoreId_t sem1;

static void ThreadSemp1(void)
{
    while (1) {
        osSemaphoreRelease(sem1);
        printf("ThreadSemp1 Release  Semap \n");
        osDelay(200U);
        return;
    }
}

static void ThreadSemp2(void)
{
    while (1) {
        osSemaphoreAcquire(sem1, osWaitForever);
        printf("ThreadSemp2 get Semap \n");
        osDelay(100U);
        return;
    }
}

static void SempExample(void)
{
    osThreadAttr_t attr;

    attr.attr_bits = ATTR_BITS;
    attr.cb_mem = NULL;
    attr.cb_size = CB_SIZE_0U;
    attr.stack_mem = NULL;
    attr.stack_size = STACK_SIZE;
    attr.priority = PRIORITY_LEVEL;

    attr.name = "ThreadSemp1";
    if (osThreadNew((osThreadFunc_t)ThreadSemp1, NULL, &attr) == NULL) {
        printf("create ThreadSemp1 failed!\n");
    }

    attr.name = "ThreadSemp2";
    if (osThreadNew((osThreadFunc_t)ThreadSemp2, NULL, &attr) == NULL) {
        printf("create ThreadSemp2 failed!\n");
    }
	
    sem1 = osSemaphoreNew(SEM_INITIAL_VALUE, SEM_MAX_VALUE, NULL);
    if (sem1 == NULL) {
        printf("Semp1 create failed!\n");
    }
}

OHOS_APP_RUN(SempExample);