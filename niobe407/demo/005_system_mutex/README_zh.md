# Niobe407开发板OpenHarmony内核编程开发——mutex
本示例将演示如何在Niobe407开发板上使用cmsis 2.0 接口进行互斥锁开发

## mutex API分析

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

## osMutexNew()

```c
osMutexNew (const osMutexAttr_t *attr);
```
**描述：**

创建互斥锁


**参数：**

|名字|描述|
|:--|:------|
| attr |mutex属性|

## osMutexAcquire()

```c
osMutexAcquire (osMutexId_t mutex_id, uint32_t timeout);
```
**描述：**

获取互斥锁


**参数：**

|名字|描述|
|:--|:------|
| mutex_id |mutex ID|
| timeout |delay时间|


## osMutexRelease()

```c
osMutexRelease (osMutexId_t mutex_id);
```
**描述：**

释放互斥锁


**参数：**

|名字|描述|
|:--|:------|
| mutex_id |mutex ID|


## 软件设计

**主要代码分析**

在os_mutex_example函数中，通过osThreadNew()函数创建了firstThread、twoThread、threeThread三个线程，firstThread、twoThread、threeThread三个线程启动后会输出打印日志。

```c
void firstThread(void)
{
	osDelay(100U);
	while(1)
	{
		osMutexAcquire(mutex_id, osWaitForever);
    	printf("firstThread is Acquire.\r\n");
    	osDelay(100U);
		osMutexRelease(mutex_id);
	}
}

void twoThread(void)
{
	osDelay(100U);
	while(1) 
	{
		printf("twoThread is Acquire.\r\n");
    	osDelay(100U);
	}
}

void threeThread(void)
{
    while(1)
	{
		osMutexAcquire(mutex_id, osWaitForever);
		printf("threeThread is Acquire.\r\n");
		osDelay(300U);
		osMutexRelease(mutex_id);
	}
}

void os_mutex_example(void)
{
    osThreadAttr_t attr;

    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;

    attr.name = "firstThread";
    attr.priority = 26;
    if (osThreadNew((osThreadFunc_t)firstThread, NULL, &attr) == NULL)
	{
    	printf("create firstThread failed!\n");
    }

	attr.name = "twoThread";
    attr.priority = 25;
    if (osThreadNew((osThreadFunc_t)twoThread, NULL, &attr) == NULL)
    {
      	printf("create twoThread failed!\n");
    }

    attr.name = "threeThread";
    attr.priority = 24;
    if (osThreadNew((osThreadFunc_t)threeThread, NULL, &attr) == NULL)
    {
      	printf("create threeThread failed!\n");
    }

	mutex_id = osMutexNew(NULL);
    if (mutex_id == NULL)
    {
      	printf("create Mutex failed!\n");
    }
}

```

## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `005_system_mutex`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```
threeThread is Acquire.
twoThread is Acquire.
twoThread is Acquire.
firstThread is Acquire.
twoThread is Acquire.
threeThread is Acquire.
twoThread is Acquire.
twoThread is Acquire.
twoThread is Acquire.
firstThread is Acquire.
twoThread is Acquire.
threeThread is Acquire.
twoThread is Acquire.
```
