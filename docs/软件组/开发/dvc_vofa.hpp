#include <cstdint>
#include <cstddef>

#include "std_typedef.h"
#include "drv_uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"

extern "C" __weak void UART6RxCallback(uint8_t *pRxData, uint16_t rxDataLength);

/* Typedef -------------------------------------------------------------------*/

/* Define --------------------------------------------------------------------*/
template <size_t N>
class Vofa
{
public:
    class Frame
    {
    private:
        static constexpr size_t DATA_FRAME_SIZE = N; // 数据帧大小
        fp32 fdata[DATA_FRAME_SIZE + 1];             // 发送缓冲区（+1用于帧尾）
        uint16_t fPos = 0;                           //  当前发送缓冲区写入位置

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
    size_t functionCount = 0;
    // 定义函数指针类型别名：指向返回 fp32 且无参数的函数
    using CallbackFunction = fp32 (*)();

private:
    Frame m_frame;

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

    CallbackFunction functionPointers[N];

public:
    Vofa()
    {
    }

    // 接收函数指针，调用对应函数获取数据
    bool BindFunction(CallbackFunction func)
    {
        if (functionCount < N) {
            functionPointers[functionCount++] = func;
            return true;
        } else {
            return false;
        }
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
        // 通过串口发送数据
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
        // 检查UART6是否已初始化
        if (huart6.gState != HAL_UART_STATE_READY) {
            UART_Init(&huart6, UART6RxCallback, 18);
        }
        if (functionCount == 0) {
            return;
        }
        static StackType_t xVofaTaskStack[1024];
        static StaticTask_t uxVofaTaskTCB;
        xTaskCreateStatic(TaskEntry, "vofa_task", 1024, this, 9, xVofaTaskStack, &uxVofaTaskTCB);
    }
};
