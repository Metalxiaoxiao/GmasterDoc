#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>

#include "std_typedef.h"
#include "drv_uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"

extern "C" __weak void UART6RxCallback(uint8_t *pRxData, uint16_t rxDataLength);
using VofaUart6RxHandler = void (*)(uint8_t *pRxData, uint16_t rxDataLength);
void Vofa_SetUART6RxHandler(VofaUart6RxHandler handler);

/* Typedef -------------------------------------------------------------------*/

/* Define --------------------------------------------------------------------*/
template <size_t N>
class Vofa
{
public:
    static constexpr size_t PARAMETER_MAX_COUNT = 100;

    class Frame
    {
    private:
        static constexpr size_t DATA_FRAME_SIZE = N; // 数据帧大小
        fp32 fdata[DATA_FRAME_SIZE + 1];             // 发送缓冲区（+1用于帧尾）
        uint16_t fPos = 0;                           // 当前发送缓冲区写入位置

    public:
        Frame() = default;

        bool write(fp32 data)
        {
            if (fPos >= DATA_FRAME_SIZE) {
                return false; // 缓冲区已满
            }
            fdata[fPos++] = data;
            return true;
        }

        const fp32 *getData()
        {
            // 添加帧尾
            uint8_t *tail = reinterpret_cast<uint8_t *>(&fdata[fPos]);
            tail[0]       = 0x00;
            tail[1]       = 0x00;
            tail[2]       = 0x80;
            tail[3]       = 0x7f;
            return fdata;
        }

        uint16_t getSize() const
        {
            return fPos * sizeof(fp32) + 4;
        }

        bool isEmpty() const
        {
            return fPos == 0;
        }

        void reset()
        {
            fPos = 0;
        }
    };

    size_t functionCount   = 0;
    using CallbackFunction = fp32 (*)();

private:
    struct Parameter {
        const char *name;
        fp32 value;
        void (*callback)(fp32 *newValue);
    };

    Frame m_frame;
    Parameter m_parameters[PARAMETER_MAX_COUNT] = {};
    size_t m_parameterCount                     = 0;

    // 这个函数可以作为左值
    //  通过GetActiveInstance()获取当前活动的Vofa实例指针，供静态回调函数使用
    static Vofa<N> *&GetActiveInstance()
    {
        static Vofa<N> *instance = nullptr;
        return instance;
    }

    static char *SkipLeadingSpace(char *str)
    {
        while (*str == ' ' || *str == '\t') {
            ++str;
        }
        return str;
    }

    static void TrimTrailingSpace(char *str)
    {
        size_t len = std::strlen(str);
        while (len > 0) {
            char ch = str[len - 1];
            if (ch == ' ' || ch == '\t') {
                str[len - 1] = '\0';
                --len;
                continue;
            }
            break;
        }
    }

    void HandleCommandLine(char *line)
    {
        line = SkipLeadingSpace(line);
        TrimTrailingSpace(line);
        if (*line == '\0') {
            return;
        }

        char *separator = std::strchr(line, ':');
        if (separator == nullptr) {
            return;
        }

        *separator     = '\0';
        char *namePart = SkipLeadingSpace(line);
        TrimTrailingSpace(namePart);
        char *valueStr = SkipLeadingSpace(separator + 1);
        TrimTrailingSpace(valueStr);
        if (*namePart == '\0' || *valueStr == '\0') {
            return;
        }

        char *endPtr   = nullptr;
        float newValue = std::strtof(valueStr, &endPtr);
        if (endPtr == valueStr) {
            return;
        }
        endPtr = SkipLeadingSpace(endPtr);
        if (*endPtr != '\0') {
            return;
        }

        for (size_t i = 0; i < m_parameterCount; ++i) {
            if (m_parameters[i].name == nullptr) {
                continue;
            }
            if (std::strcmp(m_parameters[i].name, namePart) == 0) {
                m_parameters[i].value = static_cast<fp32>(newValue);
                if (m_parameters[i].callback != nullptr) {
                    m_parameters[i].callback(&m_parameters[i].value);
                }
                break;
            }
        }
    }

    void HandleRxData(uint8_t *pRxData, uint16_t rxDataLength)
    {
        if (pRxData == nullptr || rxDataLength == 0) {
            return;
        }

        constexpr size_t RX_CMD_BUFFER_SIZE = 128;
        char buffer[RX_CMD_BUFFER_SIZE]     = {0};

        size_t copyLen = rxDataLength;
        if (copyLen >= RX_CMD_BUFFER_SIZE) {
            copyLen = RX_CMD_BUFFER_SIZE - 1;
        }

        std::memcpy(buffer, pRxData, copyLen);
        buffer[copyLen] = '\0';

        char *cursor = buffer;
        while (*cursor != '\0') {
            while (*cursor == '\r' || *cursor == '\n') {
                ++cursor;
            }
            if (*cursor == '\0') {
                break;
            }

            char *lineEnd = cursor;
            while (*lineEnd != '\0' && *lineEnd != '\r' && *lineEnd != '\n') {
                ++lineEnd;
            }
            char original = *lineEnd;
            *lineEnd      = '\0';
            HandleCommandLine(cursor);

            if (original == '\0') {
                break;
            }
            cursor = lineEnd + 1;
        }
    }

    static void ParameterRxCallback(uint8_t *pRxData, uint16_t rxDataLength)
    {
        Vofa<N> *instance = GetActiveInstance();
        if (instance != nullptr) {
            instance->HandleRxData(pRxData, rxDataLength);
        }
    }

    static void TaskEntry(void *pvParam)
    {
        Vofa<N> *vofa               = static_cast<Vofa<N> *>(pvParam);
        TickType_t taskLastWakeTime = xTaskGetTickCount();
        while (1) {
            if (vofa->functionCount == 0) {
                vTaskDelayUntil(&taskLastWakeTime, pdMS_TO_TICKS(1));
                continue;
            }
            vofa->excuteFunctions();
            vofa->sendFrame();
            vTaskDelayUntil(&taskLastWakeTime, pdMS_TO_TICKS(1));
        }
    }

    CallbackFunction functionPointers[N] = {};

public:
    Vofa()
    {
    }

    bool BindFunction(CallbackFunction func)
    {
        if (functionCount < N) {
            functionPointers[functionCount++] = func;
            return true;
        }
        return false;
    }

    bool UnbindFunction(CallbackFunction func)
    {
        for (size_t i = 0; i < functionCount; i++) {
            if (functionPointers[i] == func) {
                for (size_t j = i; j < functionCount - 1; j++) {
                    functionPointers[j] = functionPointers[j + 1];
                }
                functionCount--;
                return true;
            }
        }
        return false;
    }

    void excuteFunctions()
    {
        for (size_t i = 0; i < functionCount; i++) {
            if (functionPointers[i] == nullptr) {
                continue;
            }
            fp32 data = functionPointers[i]();
            m_frame.write(data);
        }
    }

    void sendFrame()
    {
        sendFrame(m_frame);
    }

    void sendFrame(Frame &frame)
    {
        if (frame.isEmpty()) {
            return;
        }

        HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(&huart6, reinterpret_cast<const uint8_t *>(frame.getData()), frame.getSize());
        if (status == HAL_OK) {
            frame.reset();
        }
    }

    void setFrame(Frame &frame)
    {
        m_frame = frame;
    }

    void writeData(fp32 data)
    {
        m_frame.write(data);
    }

    void Init()
    {
        GetActiveInstance() = this;
        Vofa_SetUART6RxHandler(ParameterRxCallback);

        // 如果还未由 CubeMX 初始化，则先完成 UART6 外设初始化。
        if (huart6.gState == HAL_UART_STATE_RESET) {
            MX_USART6_UART_Init();
        }

        // 无条件启动/注册接收链路，确保 data:xxx 指令能被处理。
        UART_Init(&huart6, UART6RxCallback, 128);

        if (functionCount == 0) {
            return;
        }
    }

    void TaskStart()
    {
        // 512 words 足够这种简单的发送任务使用
        static StackType_t xVofaTaskStack[512];
        static StaticTask_t uxVofaTaskTCB;
        xTaskCreateStatic(TaskEntry, "vofa_task", 512, this, 9, xVofaTaskStack, &uxVofaTaskTCB);
    }

    // 使用串口接受数据动态修改参数。
    // 格式：参数名:新参数值
    // 注意：parameterName 必须是字符串常量(String Literal)或全局静态字符串！
    // 不要传入局部变量或动态分配的字符串，否则会导致野指针崩溃。
    void AddParameterListener(const char *parameterName, void (*callback)(fp32 *newValue))
    {
        if (parameterName == nullptr || callback == nullptr) {
            return;
        }

        for (size_t i = 0; i < m_parameterCount; ++i) {
            if (m_parameters[i].name != nullptr && std::strcmp(m_parameters[i].name, parameterName) == 0) {
                m_parameters[i].callback = callback;
                return;
            }
        }

        if (m_parameterCount < PARAMETER_MAX_COUNT) {
            m_parameters[m_parameterCount++] = {parameterName, 0.0f, callback};
        }
    }
};
