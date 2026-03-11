# Vofa

精细调节 PID 和难以把脉完成的 PID 需要使用 Vofa+ 进行精细化调参。新手在使用 Vofa 时可能会遇到一些问题，请阅读此文档。

## 1. Vofa 下载

[点击下载 vofa](https://je00.github.io/downloads/vofa+_1.3.10_x64_installer.exe)

---

## 2. 添加 Vofa 配置文件

直接在 `dev` 文件夹内加入 `.cpp` 和 `.hpp` 文件：

点击查看 [vofa.cpp](dvc_vofa.cpp)

点击查看 [vofa.hpp](dvc_vofa.hpp)

配合 `tsk_test.cpp` 使用，只需要调用这里面对应的函数即可使用（初始化 `Init`、写入函数 `WriteData` 等）。

#### 方法二附加：VOFA+ 串口调试助手（`Vofa` 类）使用指南

该模块（`Vofa` 类）具备两大核心功能：

1. `实时数据上传`：通过 JustFloat 协议高速发送浮点数据波形。
2. `在线参数调优`：通过串口命令行实时修改程序内部参数。

##### 快速集成

由于使用了模板类，建议在全局或类的成员变量中定义。`N` 代表你需要同时发送的数据通道数量。

```cpp
#include "dvc_vofa.hpp"

// 定义一个拥有 9 个数据通道的 Vofa 实例
Vofa<9> vofa;
```

在任务开始前配置数据源：

```cpp
extern "C" void test_task(void *argument)
{
    CAN_Init(&hcan1, can1RxCallback);                  // 初始化CAN1
    UART_Init(&huart3, dr16ITCallback, 36);            // 初始化DR16串口
    vofa.Init();                                      // 初始化VOFA
    vofa.AddParameterListener("data", [](fp32 *newValue) {
        data = *newValue;
        printf("TestParameter updated: %f\n", *newValue);
    });
    TickType_t taskLastWakeTime = xTaskGetTickCount(); // 获取任务开始时间
    while (1) {
        motor.openloopControl(0.0f);
        transmitMotorsControlData();
        vofa.writeData(data);
        vofa.sendFrame();
        vTaskDelayUntil(&taskLastWakeTime, 1); // 确保任务以定周期1ms运行
    }
}
```

##### 在线调参使用方法

使用 `AddParameterListener` 绑定参数名和回调函数：

```cpp
// 示例：调节云台 PID
vofa.AddParameterListener("gimbal_p", [](fp32 *val) {
    gimbal_pid.kp = *val;
});
```

在串口助手中发送以下格式字符串（支持换行符 `\n` 或回车符 `\r` 结束）：

格式：`参数名:数值`

```text
gimbal_p:15.5
speed:0
```

注意：

1. 冒号必须是英文冒号 `:`。
2. 参数名区分大小写。
3. 支持一次发送多条指令（需用换行分隔）。

##### VOFA+ 上位机设置

1. 协议选择：`FireWater`（即 JustFloat 协议）。
2. 数据引擎：开启。
3. 添加控件：拖入 `Waveform`（波形图）。
4. 绑定通道：

   右键波形图 -> 配置 -> 绑定数据源。
   代码中 `BindFunction` 的调用顺序对应通道索引：
   第 1 个绑定的函数 -> `ch0`
   第 2 个绑定的函数 -> `ch1`
   以此类推。

---

## 3. 使用 Vofa

详见 [B站 vofa 教程](https://www.bilibili.com/video/BV1q1421R7uK/?spm_id_from=333.337.search-card.all.click)

一般来说调 PID 使用 `JustFloat` 即可。

## 4. 常见问题

你 先 别 急

1. 有数据但是不是你要的数据：模式没有选择串口。
2. 读不到数据：

- 波特率没对齐。
- 端口选择错误。
- Tx/Rx 有没有接反（T 对 R，R 对 T）。
- vofa 传奇 bug。

如果你打开串口，确认波特率正确、端口正确、接线 Tx/Rx 正确，以上所有问题都排查过并确认没有问题，先别急，这个是 vofa 祖传 bug：

1. 先关闭你的 vofa。
2. 寻找 `C:\Users\[username]\AppData\Local\vofa+`，将这个文件夹删掉。
3. 确认这个文件夹中内容删干净之后，再次重启 vofa。

请注意，这个 vofa 可能在任何时间、任何地点、随机概率崩溃。不要怀疑自己也不要怀疑你的电脑，毕竟这个 vofa 已经很久没更新了。

补充排查（方法二常见问题）：

1. 参数修改没反应：检查是否发送了换行、参数名是否完全一致、`AddParameterListener` 参数名是否常量字符串。
2. 波形显示乱码或锯齿：确认 VOFA+ 协议为 `FireWater`，并确认波特率与 `usart.c` 一致（常见 `115200` 或更高）。
3. 可以发送多少个数据：由模板参数 `N` 决定，例如 `Vofa<10>` 最多支持 10 个通道；超出 `N` 的绑定会无效。

