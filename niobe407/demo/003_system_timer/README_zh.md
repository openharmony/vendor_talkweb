# Niobe407开发板OpenHarmony内核编程开发——定时器
本示例将演示如何在Niobe407开发板上使用cmsis 2.0 接口进行定时器开发

## Timer API分析

## osTimerNew()

```c
    /// Create and Initialize a timer.
    /// \param[in]     func          function pointer to callback function.
    /// \param[in]     type          \ref osTimerOnce for one-shot or \ref osTimerPeriodic for periodic behavior.
    /// \param[in]     argument      argument to the timer callback function.
    /// \param[in]     attr          timer attributes; NULL: default values.
    /// \return timer ID for reference by other functions or NULL in case of error.
   osTimerId_t osTimerNew	(osTimerFunc_t func,osTimerType_t type,void *argument,const osTimerAttr_t *attr)
```
**描述：**

函数osTimerNew创建一个一次性或周期性计时器，并将其与一个带参数的回调函数相关联。计时器在osTimerStart启动之前一直处于停止状态。可以在RTOS启动(调用 osKernelStart)之前安全地调用该函数，但不能在内核初始化 (调用 osKernelInitialize)之前调用该函数。
> **注意** :不能在中断服务调用该函数
**参数：**
|名字|描述|
|:--|:------|
| func | 函数指针指向回调函数.  |
| type | 定时器类型，osTimerOnce表示单次定时器，ostimer周期表示周期性定时器.  |
| argument |定时器回调函数的参数|
| attr |计时器属性|

## osTimerStart()

```c
  /// Start or restart a timer.
  /// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
  /// \param[in]     ticks         \ref CMSIS_RTOS_TimeOutValue "time ticks" value of the timer.
  /// \return status code that indicates the execution status of the function.
  osStatus_t osTimerStart(osTimerId_t timer_id, uint32_t ticks);

```
**描述：**

函数osTimerStart启动或重新启动指定参数timer_id的计时器。参数ticks指定计时器的计数值。

> **注意** :不能在中断服务调用该函数


**参数：**

|名字|描述|
|:--|:------|
| timer_id | 由osTimerNew获得的计时器ID.  |
| ticks | 时间滴答计时器的值.  |

## 软件设计

## 软件设计

**主要代码分析**

在OS_Timer_example函数中，通过osTimerNew()函数创建了回调函数为OS_Timer1_Callback的定时器1，并通过osTimerStart()函数将该定时器设置为100个tick,因为hi3861默认10ms为一个tick,所以100个tick正好为1S钟，1S计时到后会触发OS_Timer1_Callback()函数并打印日志。定时器2也同理为3S触发OS_Timer2_Callback()函数并打印日志.

```c
  static void OS_Timer_example(void)
{
    osTimerId_t timerId1, timerId2;
    uint32_t delay;
    osStatus_t status;

    timer1Exec = 1U;
    /// Create and Initialize a timer.
    /// \param[in]     func          function pointer to callback function.
    /// \param[in]     type          \ref osTimerOnce for one-shot or \ref osTimerPeriodic for periodic behavior.
    /// \param[in]     argument      argument to the timer callback function.
    /// \param[in]     attr          timer attributes; NULL: default values.
    /// \return timer ID for reference by other functions or NULL in case of error.
    timerId1 = osTimerNew((osTimerFunc_t)OS_Timer1_Callback, osTimerPeriodic, &timer1Exec, NULL);
    if (timerId1 != NULL)
    {
        delay = 100U;
        /// Start or restart a timer.
        /// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
        /// \param[in]     ticks         \ref CMSIS_RTOS_TimeOutValue "time ticks" value of the timer.
        /// \return status code that indicates the execution status of the function.
        //osStatus_t osTimerStart(osTimerId_t timer_id, uint32_t ticks);
        status = osTimerStart(timerId1, delay);
        if (status != osOK)
        {
            printf("Failed to start timer1!\n");
        }
    }

    timer2Exec = 100U;
    timerId2 = osTimerNew((osTimerFunc_t)OS_Timer2_Callback, osTimerPeriodic, &timer2Exec, NULL);
    if (timerId2 != NULL)
    {
        delay = 300U;
        status = osTimerStart(timerId2, delay);
        if (status != osOK)
        {
            printf("Failed to start timer2!\n");
        }
    }
}

```

## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `003_system_timer`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志，OS_Timer1_Callback会1S打印一次数据，OS_Timer2_Callback会3S打印一次数据。

```
This is Niobe407 Timer1_Callback:1!
This is Niobe407 Timer1_Callback:1!
This is Niobe407 Timer1_Callback:1!
This is Niobe407 Timer2_Callback:100!
This is Niobe407 Timer1_Callback:1!
This is Niobe407 Timer1_Callback:1!
This is Niobe407 Timer1_Callback:1!
This is Niobe407 Timer2_Callback:100!
This is Niobe407 Timer1_Callback:1!
This is Niobe407 Timer1_Callback:1!
```
