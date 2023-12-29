# Niobe407开发板OpenHarmony基于HDF驱动框架编程开发——I2C

I2C是一种串行，同步，半双工通信协议，允许在同一总线上同时存在多个主机和从机。I2C总线由两条线组成：串行数据线（SDA）和串行时钟（SCL）。两条线都需要上拉电阻。I2C组件为设备提供读写等操作。

本示例将演示如何在Niobe407开发板上通过HDF驱动框架，使用I2C3接口对RJGT102芯片进行读写芯片UUID操作。

## 接口说明

```
1. I2C open打开I2C函数:DevHandle I2cOpen(int16_t number);
    参数说明: 
        number:  I2C控制器ID
        return:  返回NULL,表示初始化成功

2. I2C close打开I2C函数:void I2cClose(DevHandle handle);

3. I2C transfer启动到I2C设备的自定义传输函数:int32_t I2cTransfer(DevHandle handle, struct I2cMsg *msgs, int16_t count);
    参数说明: 
        handle:  指向通过{@link I2cOpen} 获得的I2C 控制器的设备句柄的指针
        msgs:    指示指向 I2C 传输消息结构体数组的指针
        count:   表示消息结构数组的长度
        return:  返回传输的消息结构数，表示成功，返回负数，表示失败
```

## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `204_hdf_i2c`

- 在menuconfig的`(Top) → Driver`选项中使能如下配置:

```
    [*] Enable Driver
    [*]     HDF driver framework support
    [*]         Enable HDF platform driver
    [*]             Enable HDF platform uart driver
```
- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```
Entering scheduler
[HDF:E/HDF_LOG_TAG]DeviceManagerStart in
[HDF:I/devmgr_service]start svcmgr result 0
[HDF:I/hcs_blob_if]CheckHcsBlobLength: the blobLength: 1248, byteAlign: 1
[HDF:D/device_node]node HDF_PLATFORM_I2C_MANAGER property empty, match attr: 
[HDF:I/device_node]launch devnode HDF_PLATFORM_I2C_MANAGER
[HDF:I/i2c_core]I2cManagerBind: Enter!
[HDF:I/device_node]launch devnode HDF_PLATFORM_I2C_3
[HDF:I/i2c_core]I2cCntlrAdd: use default lock methods!
OHOS # hiview init success.

<--------------- OHOS Application Start Here --------------->
[HDF:E/HDF_LOG_TAG]into HdfI2cTestEntry!

```
## I2C HDF HCS配置文件解析
- device_i2c_info.hcs文件位于/device/board/talkweb/niobe407/sdk/hdf_config/device_i2c_info.hcs,本例子使用的是i2c3
```

root {
    module = "talkweb,407"; 
    device_info {
        match_attr = "hdf_manager";
        template host {
            hostName = "";
            priority = 100;
            template device {
                template deviceNode {
                    policy = 0;
                    priority = 100;
                    preload = 0;
                    permission = 0664;
                    moduleName = "";
                    serviceName = "";
                    deviceMatchAttr = "";
                }
            }
        }
        platform :: host {
            hostName = "platform_host";
            priority = 0;

            device_i2c :: device {
                i2c_manager :: deviceNode {
                    policy = 2;
                    priority = 50;
                    moduleName = "HDF_PLATFORM_I2C_MANAGER";
                    serviceName = "HDF_PLATFORM_I2C_MANAGER";
                } 

                i2c3 :: deviceNode {      
                    policy = 0;             
                    priority = 100;        
                    preload = 0;           
                    permission = 0664;     
                    moduleName = "HDF_I2C";
                    serviceName = "HDF_PLATFORM_I2C_3";   
                    deviceMatchAttr = "i2c3_config";
                }                
            }            
        }
    }
}
```

- hdf_i2c.hcs文件位于/device/board/talkweb/niobe407/sdk/hdf_config/hdf_i2c.hcs,在此文件中配置串口对应的GPIO引脚信息，I2C配置信息
```
#include "device_i2c_info.hcs"
root {
    platform {
        i2c_config {
            i2c3 {
                gpio{
                    // 要配置的引脚个数，接下来的引脚名必须定义成gpio_num_1, gpio_num_2, gpio_num_3...
                    gpio_num_max = 2; //配置的GPIO个数

                    // port, pin, mode, speed, outputType, pull, alternate
                    gpio_num_1 = [7, 7, 2, 3, 1, 0, 4]; //GPIO配置1
                    gpio_num_2 = [7, 8, 2, 3, 1, 0, 4]; //GPIO配置2                
                }

                driver : gpio {
                    match_attr = "i2c3_config"; 
                    port = 3;  // I2C端口号(1~3)
                    dev_mode = 0; 
                    dev_addr = 0;  //I2C 本机地址
                    speed = 100000; //I2C速率
                }
            }        
        }       
    }
}
```

## 关于I2C通讯测试芯片RJGT102

    RJGT102芯片为一款标准I2C通讯的加密芯片。

    这里将此芯片当成普通EEPROM使用，通过读写芯片内部UUID区域，以测试HDF I2C的基本收发功能。

    具体芯片读写操作时序及寄存器信息，请自行参考数据手册
