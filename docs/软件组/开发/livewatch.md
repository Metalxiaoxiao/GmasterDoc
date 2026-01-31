# livewatch

## 第一步 下载Cortex-Debug
## 第二步 修改launch.json
在此附GSRL库，stlink适配的launch.json,记得添加自己的gcc path：
```bash
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug (OpenOCD)",
            "cwd": "${workspaceFolder}",
            "executable": "${workspaceFolder}/build/Debug/GMStdRobotLib.elf",
            "request": "launch",
            "type": "cortex-debug",
            "runToEntryPoint": "main",
            "servertype": "openocd",
            "gdbPath": "C:/[route]/arm-none-eabi-gdb.exe",
            "configFiles": [
                "interface/stlink.cfg",
                "target/stm32f4x.cfg"
            ],
            "showDevDebugOutput": "none",
            "liveWatch": {
                "enabled": true,
                "samplesPerSecond": 4
            }
        }
    ]
}
```
---