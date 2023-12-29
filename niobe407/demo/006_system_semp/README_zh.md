# Niobe407开发板OpenHarmony内核编程开发——Semaphore
本示例将演示如何在Niobe407开发板上使用cmsis 2.0 接口进行信号量开发

## Semaphore API分析

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

## osSemaphoreNew()

```c
osSemaphoreId_t osSemaphoreNew (uint32_t max_count, uint32_t initial_count, const osSemaphoreAttr_t *attr)
```
**描述：**

创建信号量
> **注意** :不能在中断服务调用该函数


**参数：**

|名字|描述|
|:--|:------|
| max_count |信号量计数值的最大值|
| initial_count |信号量计数值的初始值|
| max_count |信号量计数值的最大值|
| attr |信号量属性|
返回 信号量ID

## osSemaphoreRelease()

```c
osStatus_t osSemaphoreRelease (osSemaphoreId_t semaphore_id)
```
**描述：**

释放信号量
> **注意** :该函数可以在中断服务例程调用

**参数：**

|名字|描述|
|:--|:------|
| semaphore_id |信号量ID|
返回	0 - 成功，非0 - 失败


## osSemaphoreAcquire()

```c
osStatus_t osSemaphoreAcquire (osSemaphoreId_t semaphore_id, uint32_t timeout)
```
**描述：**

获取信号量


**参数：**

|名字|描述|
|:--|:------|
| semaphore_id |信号量ID|
| timeout |超时值|
返回	0 - 成功，非0 - 失败


## 软件设计

**主要代码分析**

在semp_example函数中，通过osThreadNew()函数创建了Thread_Semp1、Thread_Semp2两个线程，Thread_Semp1、Thread_Semp2两个线程启动后会输出打印日志。

```c
void Thread_Semp1(void)
{
	while(1)
	{
		osSemaphoreRelease(sem1);
		printf("Thread_Semp1 Release  Semap \n");
		osDelay(200U);
	}
}

void Thread_Semp2(void)
{
	while(1)
	{
		osSemaphoreAcquire(sem1, osWaitForever);
		printf("Thread_Semp2 get Semap \n");
		osDelay(100U);
	}
}

void semp_example(void)
{
    osThreadAttr_t attr;

    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 10;

    attr.name = "Thread_Semp1";
    if (osThreadNew((osThreadFunc_t)Thread_Semp1, NULL, &attr) == NULL)
    {
        printf("create Thread_Semp1 failed!\n");
    }

    attr.name = "Thread_Semp2";
    if (osThreadNew((osThreadFunc_t)Thread_Semp2, NULL, &attr) == NULL)
    {
        printf("create Thread_Semp2 failed!\n");
    }
    
    sem1 = osSemaphoreNew(3, 0, NULL);
    if (sem1 == NULL)
    {
        printf("Semp1 create failed!\n");
    }
}

```


## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `006_system_semp`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```
Thread_Semp1 Release  Semap 
Thread_Semp2 get Semap 
Thread_Semp1 Release  Semap 
Thread_Semp2 get Semap 
Thread_Semp1 Release  Semap 
Thread_Semp2 get Semap 
Thread_Semp1 Release  Semap 
Thread_Semp2 get Semap
```