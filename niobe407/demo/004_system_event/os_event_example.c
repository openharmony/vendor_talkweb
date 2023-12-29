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

#define FLAGS_MSK1 0x00000001U
#define TASK_DELAY 1000
#define STACK_SIZE 4096

osEventFlagsId_t g_event_flags_id; // event flags id

static void OsThreadEventSender(void *argument)
{
    (void *)argument;
    osEventFlagsId_t flags;
    printf("Start OsThreadEventSender.\n");
    while (1) {
        flags = osEventFlagsSet(g_event_flags_id, FLAGS_MSK1);
        printf("Send Flags is %d\n", flags);
        osThreadYield();
        osDelay(TASK_DELAY);
        return;
    }
}

static void OsThreadEventReceiver(void *argument)
{
    (void *)argument;
    printf("Start OsThreadEventSender.\n");
    while (1) {
        uint32_t flags;
        flags = osEventFlagsWait(g_event_flags_id, FLAGS_MSK1, osFlagsWaitAny, osWaitForever);
        printf("Receive Flags is %u\n", flags);
        return;
    }
}

static void OsEventExample(void)
{
    printf("Start OsEventExample.\n");
    g_event_flags_id = osEventFlagsNew(NULL);
    if (g_event_flags_id == NULL) {
        printf("Failed to create EventFlags!\n");
        return;
    }

    osThreadAttr_t attr;

    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = STACK_SIZE;
    attr.priority = osPriorityNormal;

    attr.name = "Thread_EventSender";
    if (osThreadNew(OsThreadEventSender, NULL, &attr) == NULL) {
        printf("Failed to create Thread_EventSender!\n");
        return;
    }
    
    attr.name = "Thread_EventReceiver";
    if (osThreadNew(OsThreadEventReceiver, NULL, &attr) == NULL) {
        printf("Failed to create Thread_EventReceiver!\n");
        return;
    }
}

OHOS_APP_RUN(OsEventExample);
