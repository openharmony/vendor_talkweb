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

#define SLEEP_TIME 500000
#define THREAD_STACK_SIZE (1024 * 4)
#define THREAD1_NAME "thread1"
#define THREAD2_NAME "thread2"
#define THREAD1_PRIORITY 25

static void ThreadEntry1(void)
{
    int sum = 0;
    while (1) {
        printf("This is Niobe407 Thread1----%d\r\n", sum++);
        usleep(SLEEP_TIME);
        return;
    }
}

static void ThreadEntry2(void)
{
    int sum = 0;
    while (1) {
        printf("This is Niobe407 Thread2----%d\r\n", sum++);
        usleep(SLEEP_TIME);
        return;
    }
}

static void OsThreadExample(void)
{
    osThreadAttr_t attr;

    attr.name = THREAD1_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = THREAD_STACK_SIZE;
    attr.priority = THREAD1_PRIORITY;

    if (osThreadNew((osThreadFunc_t)ThreadEntry1, NULL, &attr) == NULL) {
        printf("Failed to create thread1!\n");
    }

    attr.name = THREAD2_NAME;

    if (osThreadNew((osThreadFunc_t)ThreadEntry2, NULL, &attr) == NULL) {
        printf("Failed to create thread2!\n");
    }
}

OHOS_APP_RUN(OsThreadExample);
