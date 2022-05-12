# vendor_talkweb

## 介绍

该仓库托管拓维信息系统股份有限公司官方开发的产品样例代码，主要包括如下开发板案例代码及配置:

* Niobe407 开发板

  Niobe407开发板是基于意法半导体STM32F407IGT6芯片,由拓维信息系统股份有限公司出品的一款高性能、多功能，助力于工业、交通领域的开发板。它基于高性能 ARM® Cortex™-M4 32 位内核，工作频率高达 168 MHz，具有浮点运算单元 (FPU)，该单元支持所有ARM单精度数据处理指令和数据类型。集成了高速嵌入式存储器，具有Flash大小1MB，SRAM大小192KB。外设包含一路RJ45以太网、两路CAN、一路RS232、两路R485、一路I2C和一个全速USB OTG。并且底板板载支持外挂usb 4G模块、Lora及WIFI蓝牙模块。

## 软件架构

代码路径：

```
vendor/talkweb/                  --- vendor_talkweb    仓库路径
└── niobe407                     --- niobe407开发板(基于意法半导体STM32F407IGT6的开发板)
```

## 使用教程

- [开发环境搭建与固件编译](https://gitee.com/openharmony-sig/device_board_talkweb/blob/master/niobe407/docs/software/开发环境搭建与固件编译.md)

- [固件烧录](https://gitee.com/openharmony-sig/device_board_talkweb/blob/master/niobe407/docs/software/固件烧录.md)

- [快速开发指南](https://gitee.com/openharmony-sig/device_board_talkweb/blob/master/niobe407/docs/software/快速开发指南.md)

## 使用说明

## 贡献

[如何参与](https://gitee.com/openharmony/docs/blob/HEAD/zh-cn/contribute/%E5%8F%82%E4%B8%8E%E8%B4%A1%E7%8C%AE.md)

[Commit message规范](https://gitee.com/openharmony/device_qemu/wikis/Commit%20message%E8%A7%84%E8%8C%83?sort_id=4042860)

## 相关仓

[device_board_talkweb](https://gitee.com/openharmony/device_board_talkweb)

[device_soc_st](https://gitee.com/openharmony/device_soc_st)