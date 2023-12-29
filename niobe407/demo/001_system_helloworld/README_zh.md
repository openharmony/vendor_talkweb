# 调试打印示例

## Hello World

沿袭程序界的传统，入门程序都是Hello World。在Niobe407 开发板中，指的是通过开发板串口，在串口日志中输出Hello World字符串。

在Niobe407开发板系统代码中，Hello World程序位于applications目录下面，具体路径如下：

```text
device/board/talkweb/niobe407/applications/001_system_helloworld/hello_world.c
```
## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `001_system_helloworld`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

## 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志，显示内容如下则表示执行成功:
```
**********************
[Talkweb] Hello world.
**********************
```
