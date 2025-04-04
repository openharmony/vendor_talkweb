# Niobe407开发板OpenHarmony内核编程开发——Thread多线程
本示例将演示如何在Niobe407开发板上使用cmsis 2.0 接口进行多线程开发

## Thread API分析

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

## 软件设计

**主要代码分析**

在OS_Thread_example函数中，通过osThreadNew()函数创建了thread1和thread2两个进程，thread1和thread2启动后会输出打印日志。

```c
void thread_entry1(void)
{
    int sum=0;
    while (1)
    {
        /* 示例代码 */
        printf("This is Niobe407 Thread1----%d\r\n",sum++);
        usleep(500000);
    }
}

void thread_entry2(void)
{
    int sum=0;
    while (1)
    {
       /* 示例代码 */
        printf("This is Niobe407 Thread2----%d\r\n",sum++);
        usleep(500000);
    }
}

static void OS_Thread_example(void)
{
    osThreadAttr_t attr;

    attr.name = "thread1";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024*4;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t)thread_entry1, NULL, &attr) == NULL) {
        printf("Failed to create thread1!\n");
    }

    attr.name = "thread2";

    if (osThreadNew((osThreadFunc_t)thread_entry2, NULL, &attr) == NULL) {
        printf("Failed to create thread2!\n");
    }
}

```

## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `002_system_thread`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志，Thread1和Thread2会交替打印信息
```
This is Niobe407 Thread1----0
This is Niobe407 Thread2----0
This is Niobe407 Thread1----1
This is Niobe407 Thread2----1
This is Niobe407 Thread1----2
This is Niobe407 Thread2----2
This is Niobe407 Thread1----3
This is Niobe407 Thread2----3
This is Niobe407 Thread1----4
This is Niobe407 Thread2----4
This is Niobe407 Thread1----5
This is Niobe407 Thread2----5
```
