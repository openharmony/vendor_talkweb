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

#define DELAY_TIME 100
#define ATTR_BITS 0U
#define CB_SIZE_0U 0U
#define STACK_SIZE (1024 * 4)
#define PRIORITY_LEVEL 10
#define QUEUE_SIZE 10
#define DATA_SIZE 50

typedef struct {
    char *buffer;
} MSG_BUF;

MSG_BUF g_msgBuf;
osMessageQueueId_t g_msg_queue;

static void ThreadMsgQueue1(void *arg)
{
    (void)arg;
    g_msgBuf.buffer = "hello niobe407";
    while (1) {
        osMessageQueuePut(g_msg_queue, &g_msgBuf.buffer, 0U, 0U);
        osThreadYield();
        osDelay(DELAY_TIME);
        return;
    }
}

static void ThreadMsgQueue2(void *arg)
{
    (void)arg;
    osStatus_t status;

    while (1) {
        // wait for message
        status = osMessageQueueGet(g_msg_queue, &g_msgBuf.buffer, NULL, 0U);
        if (status == osOK) {
            printf("ThreadMsgQueue2 Get msg: %s\n", g_msgBuf.buffer);
        } else {
            osDelay(DELAY_TIME);
        }
        return;
    }
}

static void MsgExample(void)
{
    g_msg_queue = osMessageQueueNew(QUEUE_SIZE, DATA_SIZE, NULL);
    if (g_msg_queue == NULL) {
        printf("create Message Queue failed!\n");
    }

    osThreadAttr_t attr;

    attr.attr_bits = ATTR_BITS;
    attr.cb_mem = NULL;
    attr.cb_size = CB_SIZE_0U;
    attr.stack_mem = NULL;
    attr.stack_size = STACK_SIZE;
    attr.priority = PRIORITY_LEVEL;

    attr.name = "ThreadMsgQueue1";
    if (osThreadNew(ThreadMsgQueue1, NULL, &attr) == NULL) {
        printf("create ThreadMsgQueue1 failed!\n");
    }

    attr.name = "ThreadMsgQueue2";
    if (osThreadNew(ThreadMsgQueue2, NULL, &attr) == NULL) {
        printf("create ThreadMsgQueue2 failed!\n");
    }
}

OHOS_APP_RUN(MsgExample);
