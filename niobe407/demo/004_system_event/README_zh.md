# Niobe407开发板OpenHarmony内核编程开发——事件标志
本示例将演示如何在Niobe407开发板上使用cmsis 2.0 接口使用事件标志同步线程


## EventFlags API分析


## osEventFlagsNew()

```c
    /// Create and Initialize an Event Flags object.
    /// \param[in]     attr          event flags attributes; NULL: default values.
    /// \return event flags ID for reference by other functions or NULL in case of error.
osEventFlagsId_t osEventFlagsNew(const osEventFlagsAttr_t *attr)
```
**描述：**

osEventFlagsNew函数创建了一个新的事件标志对象，用于跨线程发送事件，并返回事件标志对象标识符的指针，或者在出现错误时返回NULL。可以在RTOS启动(调用 osKernelStart)之前安全地调用该函数，但不能在内核初始化 (调用 osKernelInitialize)之前调用该函数。
> **注意** :不能在中断服务调用该函数

**参数：**

|名字|描述|
|:--|:------|
| attr |事件标志属性;空:默认值.  |

## osEventFlagsSet()

```c
   /// Set the specified Event Flags.
   /// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
   /// \param[in]     flags         specifies the flags that shall be set.
   /// \return event flags after setting or error code if highest bit set.
uint32_t osEventFlagsSet(osEventFlagsId_t ef_id,uint32_t flags)
```
**描述：**
osEventFlagsSet函数在一个由参数ef_id指定的事件标记对象中设置由参数flags指定的事件标记。

> **注意** :不能在中断服务调用该函数


**参数：**

|名字|描述|
|:--|:------|
| ef_id | 事件标志由osEventFlagsNew获得的ID.  |
| flags | 指定设置的标志.  |
| return | 返回设置的事件标志，或者如果高位设置值则返回错误码.  |

## osEventFlagsWait()

```c
       /// Wait for one or more Event Flags to become signaled.
        /// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
        /// \param[in]     flags         specifies the flags to wait for.
        /// \param[in]     options       specifies flags options (osFlagsXxxx).
        /// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
        /// \return event flags before clearing or error code if highest bit set.
uint32_t osEventFlagsWait(osEventFlagsId_t ef_id,uint32_t flags,uint32_t options,uint32_t timeout)
```
**描述：**
osEventFlagsWait函数挂起当前运行线程，直到设置了由参数ef_id指定的事件对象中的任何或所有由参数flags指定的事件标志。当这些事件标志被设置，函数立即返回。否则，线程将被置于阻塞状态。

> **注意** :如果参数timeout设置为0，可以从中断服务例程调用


**参数：**

|名字|描述|
|:--|:------|
| ef_id | 事件标志由osEventFlagsNew获得的ID.  |
| flags | 指定要等待的标志.  |
| options | 指定标记选项.  |
| timeout | 超时时间，0表示不超时  |
| return | 返回设置的事件标志，或者如果高位设置值则返回错误码.  |

## 软件设计

## 软件设计

**主要代码分析**

在OS_Event_example函数中，通过osEventFlagsNew()函数创建了事件标记ID g_event_flags_id，OS_Thread_EventReceiver()函数中通过osEventFlagsWait()函数一直将线程置于阻塞状态，等待事件标记。在OS_Thread_EventSender()函数中通过osEventFlagsSet()函数每隔1S设置的标志，实现任务间的同步。

```c
/**
 *发送事件
 *\param[in] argument 发送参数
 */
void OS_Thread_EventSender(void *argument)
{
    osEventFlagsId_t flags;
    (void)argument;
    while (1)
    {
        /// Set the specified Event Flags.
        /// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
        /// \param[in]     flags         specifies the flags that shall be set.
        /// \return event flags after setting or error code if highest bit set.
        flags = osEventFlagsSet(g_event_flags_id, FLAGS_MSK1);

        printf("Sender Flags is %d\n", flags);
        //挂起线程，让位其他线程调度
        osThreadYield();
        osDelay(100);
    }
}

/**
 *  接收事件
 * \param[in] argument 发送参数
*/
void OS_Thread_EventReceiver(void *argument)
{
    (void)argument;
    uint32_t flags;

    while (1)
    {
        /// Wait for one or more Event Flags to become signaled.
        /// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
        /// \param[in]     flags         specifies the flags to wait for.
        /// \param[in]     options       specifies flags options (osFlagsXxxx).
        /// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
        /// \return event flags before clearing or error code if highest bit set.
        flags = osEventFlagsWait(g_event_flags_id, FLAGS_MSK1, osFlagsWaitAny, osWaitForever);
        printf("Receive Flags is %d\n", flags);
    }
}

/**
 * 事件测试Example
*/
static void OS_Event_example(void)
{
    //  ==== Event Flags Management Functions ====
    /// Create and Initialize an Event Flags object.
    /// \param[in]     attr          event flags attributes; NULL: default values.
    /// \return event flags ID for reference by other functions or NULL in case of error.
    g_event_flags_id = osEventFlagsNew(NULL);
    if (g_event_flags_id == NULL)
    {
        printf("Failed to create EventFlags!\n");
        return;
    }

    osThreadAttr_t attr;

    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 25;

    attr.name = "Thread_EventSender";
    /// Create a thread and add it to Active Threads.
    /// \param[in]     func          thread function.
    /// \param[in]     argument      pointer that is passed to the thread function as start argument.
    /// \param[in]     attr          thread attributes; NULL: default values.
    /// \return thread ID for reference by other functions or NULL in case of error
    //osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr);
    if (osThreadNew(OS_Thread_EventSender, NULL, &attr) == NULL)
    {
        printf("Failed to create Thread_EventSender!\n");
        return;
    }
    
    attr.name = "Thread_EventReceiver";
    if (osThreadNew(OS_Thread_EventReceiver, NULL, &attr) == NULL)
    {
        printf("Failed to create Thread_EventReceiver!\n");
        return;
    }
}

```

## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `004_system_event`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志，会每隔1S输出一次日志。
```
Start OS_Event_example.
Start OS_Thread_EventSender.
Send Flags is 1
Start OS_Thread_EventSender.
Receive Flags is 1
Send Flags is 1
Receive Flags is 1
Send Flags is 1
Receive Flags is 1
Send Flags is 1
Receive Flags is 1
Send Flags is 1
Receive Flags is 1
Send Flags is 1
Receive Flags is 1
```
