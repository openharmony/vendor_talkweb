# Niobe407开发板OpenHarmony内核编程开发——message
本示例将演示如何在Niobe407开发板上使用cmsis 2.0 接口进行消息队列开发

## message API分析

## osThreadNew()

```c
osThreadId_t osThreadNew(osThreadFunc_t	func, void *argument,const osThreadAttr_t *attr )
```
**描述：**

函数osThreadNew通过将线程添加到活动线程列表并将其设置为就绪状态来启动线程函数。线程函数的参数使用参数指针*argument传递。当创建的thread函数的优先级高于当前运行的线程时，创建的thread函数立即启动并成为新的运行线程。线程属性是用参数指针attr定义的。属性包括线程优先级、堆栈大小或内存分配的设置。可以在RTOS启动(调用 osKernelStart)之前安全地调用该函数，但不能在内核初始化 (调用 osKernelInitialize)之前调用该函数。
> **注意** :不能在中断服务调用该函数


**参数：**

|名字|描述|
|:--|:------|
| func | 线程函数.  |
| argument |作为启动参数传递给线程函数的指针|
| attr |线程属性|

## osMessageQueueNew()

```c
osMessageQueueId_t osMessageQueueNew (uint32_t msg_count,uint32_t msg_size,const osMessageQueueAttr_t *attr)
```
**描述：**

创建消息队列
> **注意** :不能在中断服务调用该函数


**参数：**

|名字|描述|
|:--|:------|
| msg_count |队列中的最大消息数|
| msg_size |最大消息大小(以字节为单位)|
| attr |消息队列属性;空:默认值|
返回 消息队列ID

## osMessageQueuePut()

```c
osStatus_t osMessageQueuePut (osMessageQueueId_t mq_id,const void *msg_ptr,uint8_t msg_prio,uint32_t timeout)
```
**描述：**

发送消息
> **注意** :如果参数timeout设置为0，可以从中断服务例程调用

**参数：**

|名字|描述|
|:--|:------|
| mq_id |由osMessageQueueNew获得的消息队列ID|
| msg_ptr |要发送的消息|
| msg_prio |指优先级|
| timeout |超时值|
返回	0 - 成功，非0 - 失败


## osMessageQueueGet()

```c
osStatus_t osMessageQueueGet (osMessageQueueId_t mq_id,void *msg_ptr,uint8_t *msg_prio,uint32_t timeout)
```
**描述：**

获取消息
> **注意** :如果参数timeout设置为0，可以从中断服务例程调用

**参数：**

|名字|描述|
|:--|:------|
| mq_id |由osMessageQueueNew获得的消息队列ID|
| msg_ptr |指针指向队列中获取消息的缓冲区指针|
| msg_prio |指优先级|
| timeout |超时值|
返回	0 - 成功，非0 - 失败


## 软件设计

**主要代码分析**

在Msg_example函数中，通过osThreadNew()函数创建了Thread_MsgQ1、Thread_MsgQ2两个线程，Thread_MsgQ1、Thread_MsgQ2两个线程启动后会输出打印日志。

```c
typedef struct
{
    char *buffer;
} MSG_BUF;
MSG_BUF msg_buf;
void Thread_MsgQueue1(void *arg)
{
	msg_buf.buffer = "hello niobe";
	while (1)
	{
		osMessageQueuePut(MsgQueue, &msg_buf.buffer, 0U, 0U);
		//suspend thread
		osThreadYield();
		osDelay(100);
	}
}

void Thread_MsgQueue2(void *arg)
{
	osStatus_t status;

	while (1)
	{
		//wait for message
		status = osMessageQueueGet(MsgQueue, &msg_buf.buffer, NULL, 0U);
		if (status == osOK)
		{
            printf("Thread_MsgQueue2 Get msg: %s\n", msg_buf.buffer);
        }
        else
        {
            osDelay(100);
		}
	}
}

static void Msg_example(void)
{

	MsgQueue = osMessageQueueNew(10, 50, NULL);
	if (MsgQueue == NULL)
	{
		printf("create Message Queue failed!\n");
	}

	osThreadAttr_t attr;

	attr.attr_bits = 0U;
	attr.cb_mem = NULL;
	attr.cb_size = 0U;
	attr.stack_mem = NULL;
	attr.stack_size = 1024 * 10;
	attr.priority = 10;

	attr.name = "Thread_MsgQueue1";
	if (osThreadNew(Thread_MsgQueue1, NULL, &attr) == NULL)
	{
		printf("create Thread_MsgQueue1 failed!\n");
	}
	
	attr.name = "Thread_MsgQueue2";
	if (osThreadNew(Thread_MsgQueue2, NULL, &attr) == NULL)
	{
		printf("create Thread_MsgQueue2 failed!\n");
	}
}

APP_FEATURE_INIT(Msg_example);

```

## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `007_system_message`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```
ThreadMsgQueue2 Get msg: hello niobe407
ThreadMsgQueue2 Get msg: hello niobe407
ThreadMsgQueue2 Get msg: hello niobe407
ThreadMsgQueue2 Get msg: hello niobe407
ThreadMsgQueue2 Get msg: hello niobe407
ThreadMsgQueue2 Get msg: hello niobe407
ThreadMsgQueue2 Get msg: hello niobe407
```
