# Niobe407开发板OpenHarmony基于HDF驱动框架编程开发——SPI
本示例将演示如何在Niobe407开发板上通过HDF驱动框架，使用SPI接口读写外部SPI-FLASH。


## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `202_hdf_spi_flash`

- 在menuconfig的`(Top) → Driver`选项中使能如下配置:

```
    [*] Enable Driver
    [*]     HDF driver framework support
    [*]         Enable HDF platform driver
    [*]             Enable HDF platform spi driver
```
- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```
[HDF:I/HDF_LOG_TAG]spi 0 open success 0x40003800
[HDF:I/HDF_LOG_TAG]read device id is 0x17
[HDF:I/HDF_LOG_TAG]read flash id is 0x4018
[HDF:I/HDF_LOG_TAG]send buffer is welcome to OpenHarmony
[HDF:I/HDF_LOG_TAG]recv send buffer is welcome to OpenHarmony
[HDF:I/HDF_LOG_TAG]hdf spi write read flash success
[HDF:I/HDF_LOG_TAG]spi 0 close success 0x40003800
```

## SPI HDF HCS配置文件解析

- device_spi_info.hcs 在/device/board/talkweb/niobe407/sdk/hdf_config/device_spi_info.hcs
```
root {
    module = "talkweb,stm32f407";
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
            device_spi :: device {
				spi0 :: deviceNode {
                    policy = 2;
                    priority = 100; // 因为spi的gpio引脚要在spi初始化之前初始化，所以优先级比gpio的hdf优先级低               
                    moduleName = "STM_TW_SPI_MODULE_HDF"; //驱动名称，该字段的值必须和驱动入口结构的moduleName值一致
                    serviceName = "HDF_PLATFORM_SPI_0";   
                    deviceMatchAttr = "spi_w25q_config";  // 对应hdf_spi.hcs中的config
				}
            }
        }
    }
}
```
- hdf_spi.hcs在在/device/board/talkweb/niobe407/sdk/hdf_config/hdf_spi.hcs，主要包括spi设置的信息的配置

```
#include "device_spi_info.hcs"
root {
    platform {
        spi1_config {
			spi1_gpio {
			    // 要配置的引脚个数，接下来的引脚名必须定义成gpio_num_1, gpio_num_2, gpio_num_3...
                gpio_num_max = 4; 
                // port, pin, mode, speed, outputType, pull, alternate
                gpio_num_1 = [0, 5, 2, 3, 0, 0, 5]; // clk pa5 
                gpio_num_2 = [1, 5, 2, 3, 0, 0, 5]; // mosi pb5
                gpio_num_3 = [1, 4, 2, 3, 0, 0, 5]; // miso pb4
                gpio_num_4 = [0, 15, 1, 3, 0, 0 ,0]; // cs pa15
	        }
	        spi_config : spi1_gpio {
                match_attr = "spi_w25q_config";
                busNum = 0; // 注册的总线号
                csNum = 0;  // 片选号
                transDir = 0;  // 0: TW_HAL_SPI_FULL_DUPLEX 1: TW_HAL_SPI_SIMPLEX_RX 2: TW_HAL_SPI_HALF_DUPLEX_RX 3: TW_HAL_SPI_HALF_DUPLEX_TX
                transMode = 1; // 1: normal 0:dma
                smMode = 1;      //  0: slave 1: master
                dataWidth = 0; // 0:8bit 1:16bit
                clkMode = 0;   // 0: cpol 0 cpha 0  1:CPOL = 1; CPHA = 0 2:CPOL = 0; CPHA = 1 3:CPOL = 1; CPHA = 1
                nss = 0;       // 0:NSS SOFT 1: NSS HARDWARE INPUT 2: NSS HARDWARE OUTPUT
                baudRate = 1; // 0:div2 1:div4 2:div8 3:div16 4:div32 5:div64 6:div128 6:div256
                bitOrder = 0; // 0: MSB first 1: LSB first
                crcEnable = 0; // 0: crc disable 1: crc enable
                crcPoly = 10;  // Min_Data = 0x00 and Max_Data = 0xFFFF
                spix = 0;   // 0: spi1  1: spi2  2:spi3 本例程使用SPI1作为示例
                csPin = 15;  // cs pin
                csGpiox = 0; // cs pin group
                standard = 0; // 0:motorola 1: ti
                dummyByte = 255;
	        }
        }
    }
}
```

## 接口解析
```
1.在使用SPI进行通信时，首先要调用SpiOpen获取SPI设备句柄，该函数会返回指定总线号和片选号的SPI设备句柄。
DevHandle SpiOpen(const struct SpiDevInfo *info);
参数说明: 
            info: SPI设备描述符， 设置了总线号和片选号
返回值：
            NULL：  打开失败
            设备句柄：打开成功

2.在获取到SPI设备句柄之后，需要配置SPI设备属性。配置SPI设备属性之前，可以先获取SPI设备属性，获取SPI设备属性的函数如下所示：
int32_t SpiGetCfg(DevHandle handle, struct SpiCfg *cfg);
参数说明: 
            handle: 设备句柄
            cfg:    设置的spi的设置
返回值：
            0：  设置成功
            负数：设置失败
3.如果需要发起一次自定义传输，则可以通过以下函数完成：
int32_t SpiTransfer(DevHandle handle, struct SpiMsg *msgs, uint32_t count);
参数说明: 
            handle: 设备句柄
            msgs:   待传输数据的数组
            count:  待传输的数组的长度
返回值：
            0：  执行成功
            负数：执行失败
4.SPI通信完成之后，需要销毁SPI设备句柄，销毁SPI设备句柄的函数如下所示：
void SpiClose(DevHandle handle);
参数说明: 
            handle: 设备句柄
```

### 示例代码解析
- 开启spi,参数获取和设置示例
``` c
static void* HdfSpiTestEntry(void* arg)
{
    int32_t ret;
    uint16_t flashId = 0;
    uint16_t deviceId = 0;
    struct SpiCfg cfg;                  /* SPI配置信息 */
    struct SpiDevInfo spiDevinfo;       /* SPI设备描述符 */
    DevHandle spiHandle = NULL; /* SPI设备句柄 */
    spiDevinfo.busNum = 0;              /* SPI设备总线号 */
    spiDevinfo.csNum = 0;               /* SPI设备片选号 */
    spiHandle = SpiOpen(&spiDevinfo);   /* 根据spiDevinfo获取SPI设备句柄 */
    if (spiHandle == NULL) {
        HDF_LOGE("SpiOpen: failed\n");
        return;
    }
    /* 获取SPI设备属性 */
    ret = SpiGetCfg(spiHandle, &cfg);
    if (ret != 0) {
        HDF_LOGE("SpiGetCfg: failed, ret %d\n", ret);
        goto err;
    }
    HDF_LOGI("speed:%d, bitper:%d, mode:%d, transMode:%d\n", cfg.maxSpeedHz, cfg.bitsPerWord, cfg.mode, cfg.transferMode);
    cfg.maxSpeedHz = 1;                /* spi2，spi3 最大频率为42M, spi1  频率为84M， 此处的值为分频系数，0:1/2 1:1/4， 2:1/8     . */
                                         /* 3: 1/16, 4: 1/32 5:1/64, 6:1/128  7:1/256 */
    cfg.bitsPerWord = 8;                    /* 传输位宽改为8比特 */
    cfg.mode = 0;
    cfg.transferMode = 1;              /* 0:dma  1:normal */
    /* 配置SPI设备属性 */
    ret = SpiSetCfg(spiHandle, &cfg);
    if (ret != 0) {
        HDF_LOGE("SpiSetCfg: failed, ret %d\n", ret);
        goto err;
    }
    SpiClose(spiHandle);

```
- spi消息传输示例使用SpiTransfer
``` c
static uint16_t ReadDeviceId(DevHandle spiHandle)
{
    struct SpiMsg msg;                  /* 自定义传输的消息 */
    uint16_t deviceId = 0;
    uint8_t rbuff[5] = { 0 };
    uint8_t wbuff[5] = { 0xAB, 0xFF, 0xFF, 0xFF, 0xFF }; // oxab为读DeviceId 指令，其他为dummyData
    int32_t ret = 0;
    msg.wbuf = wbuff;  /* 写入的数据 */
    msg.rbuf = rbuff;   /* 读取的数据 */
    msg.len = 5;        /* 读取写入数据的长度为4 */
    msg.keepCs = 0;   /* 进行下一次传输前关闭片选 */
    msg.delayUs = 0;    /* 进行下一次传输前不进行延时 */
    //msg.speed = 115200; /* 本次传输的速度 */
    /* 进行一次自定义传输，传输的msg个数为1 */
    ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
    } else {
        deviceId = rbuff[4]; // 最后一次发送oxff返回deviceid
    }

    return deviceId;
}

static void BufferWrite(DevHandle spiHandle, const uint8_t* buf, uint32_t size)
{
    WriteEnable(spiHandle); //使能spi写
    uint8_t wbuf[4] = {0x02, 0x00, 0x00, 0x00}; // 0x02 写指令write buf头，0x00, 0x00, 0x00 三个为6位写的地址
    uint8_t rbuf[4] = {0};
    uint8_t *rbuf1 = NULL;
    int32_t ret = 0;

    struct SpiMsg msg = {0};
    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = 4;
    msg.keepCs = 1; // 写指令发送后不关闭片选
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1); 
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
    }

    rbuf1 = (uint8_t*)OsalMemAlloc(size);
    memset_s(rbuf1, size, 0, size);
    msg.wbuf = buf;
    msg.rbuf = rbuf1;
    msg.len = size;
    msg.keepCs = 0; // 写完内容后关闭片选
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1); //传输真正书写的内容
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
    }

    WaitForWriteEnd(spiHandle); // 等待书写完成
    if (rbuf1!= NULL) {
        OsalMemFree(rbuf1);
    }

}

static void BufferRead(DevHandle spiHandle, uint8_t* buf, uint32_t size)
{
    int32_t ret = 0;

    uint8_t wbuf[4] = {0x03, 0x00, 0x00, 0x00}; // 0x03 读指令， 后三个0x00 为地址
    uint8_t rbuf[4] = {0};
    struct SpiMsg msg = {0};
    msg.wbuf = wbuf;
    msg.rbuf = rbuf;
    msg.len = 4;
    msg.keepCs = 1; // 写使能不关闭片选
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1);
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
        return;
    }
    uint8_t *wbuf1 = NULL;
    wbuf1 = (uint8_t*)OsalMemAlloc(size);
    memset_s(wbuf1, size, 0xff, size);
    msg.wbuf = wbuf1;
    msg.rbuf = buf;
    msg.len = size;
    msg.keepCs = 0;
    msg.delayUs = 0;
    ret = SpiTransfer(spiHandle, &msg, 1); // 读取spi flash中存储的值
    if (ret != 0) {
        HDF_LOGE("SpiTransfer: failed, ret %d\n", ret);
        return;
    }

    if (wbuf1!= NULL) {
        OsalMemFree(wbuf1);
    }

}
```

- 使用SpiWrite, SpiRead通信，由于使用这两个接口无法制定是否关闭片选，因此在写或者读得buf第一个字节为自定义关闭片选信号，0不关闭片选， 1关闭片选，读取内容时要注意去掉第一个字节
``` c
static uint16_t ReadDeviceId(DevHandle spiHandle)
{
    struct SpiMsg msg;                  /* 自定义传输的消息 */
    uint16_t deviceId = 0;
    uint8_t rbuff1[2] = { 0 };
    uint8_t wbuff1[5] = {0x00, 0xAB, 0xff,0xff, 0xff}; //0x00 不关闭片选，0xab读取设备指令，0xff为dummydata
    int32_t ret = 0;

    /* 发送命令：读取芯片型号ID */
    ret =SpiWrite(spiHandle, wbuff1, 5);
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }
    rbuff1[0] = 0x01; //关闭片选
    ret = SpiRead(spiHandle, rbuff1, 2);
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }

    deviceId = rbuff1[1];

    return deviceId;
}

static void BufferWrite(DevHandle spiHandle, const uint8_t* buf, uint32_t size)
{
    WriteEnable(spiHandle);
    uint8_t wbuf[5] = {0x01, 0x02, 0x00, 0x00, 0x00};// 0x01 关闭片选， 0x02写指令，后面3个0x00为地址
    uint8_t rbuf[4] = {0};
    uint8_t *wbuf1 = NULL;
    int32_t ret = 0;
    wbuf1 = (uint8_t*)OsalMemAlloc(size + sizeof(wbuf));

    strncpy_s(wbuf1, size + sizeof(wbuf), wbuf, sizeof(wbuf));
    strncpy_s(wbuf1 + sizeof(wbuf), size, buf, size);
    ret = SpiWrite(spiHandle, wbuf1, size + sizeof(wbuf)); //将写写指令和内容一起写入spi flash
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }

    WaitForWriteEnd(spiHandle);
    if (wbuf1!= NULL) {
        OsalMemFree(wbuf1);
    }

}

static void BufferRead(DevHandle spiHandle, uint8_t* buf, uint32_t size)
{
    WriteEnable(spiHandle);
    uint8_t wbuf[5] = {0x00, 0x03, 0x00, 0x00, 0x00};// 0x00 不关闭片选， 0x03读指令，后面3个0x00为地址
    uint8_t *rbuf = NULL;
    int32_t ret = 0;
    rbuf = (uint8_t*)OsalMemAlloc(size + 1);

    ret = SpiWrite(spiHandle, wbuf, 5);// 先写读指令和地址
    if (ret != 0) {
        HDF_LOGE("SpiWrite: failed, ret %d\n", ret);
    }
    rbuf[0] = 0x01; // 读完关闭片选
    ret = SpiRead(spiHandle, rbuf, size + 1); // 读取内容
    if (ret != 0) {
        HDF_LOGE("SpiRead: failed, ret %d\n", ret);
    }

    strncpy_s(buf, size, rbuf + 1, size);

    if (rbuf!= NULL) {
        OsalMemFree(rbuf);
    }

    return;
```
