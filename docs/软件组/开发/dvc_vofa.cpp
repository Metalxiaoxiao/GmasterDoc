/**
 ******************************************************************************
 * @file           : vofa.cpp
 * @brief          : vofa串口驱动
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 GMaster
 * All rights reserved.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

#include "dvc_vofa.hpp"
#include <cstdint>
#include <cstddef>
#include <mpu_wrappers.h>
#include <portmacro.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include "usart.h"

/* Macro ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// 如果需要不同大小,可以声明: Vofa<200> vofa_large;
static VofaUart6RxHandler s_vofaUart6RxHandler = nullptr;

/* Function prototypes -------------------------------------------------------*/

/* User code -----------------------------------------------------------------*/

void Vofa_SetUART6RxHandler(VofaUart6RxHandler handler)
{
    s_vofaUart6RxHandler = handler;
}

extern "C" __weak void UART6RxCallback(uint8_t *pRxData, uint16_t rxDataLength)
{
    if (s_vofaUart6RxHandler != nullptr) {
        s_vofaUart6RxHandler(pRxData, rxDataLength);
    }
}

/**
 * @brief 重定向printf输出到UART6
 * @param file 文件描述符
 * @param ptr 要发送的数据指针
 * @param len 数据长度
 * @return 实际发送的字节数
 */
extern "C" int _write(int file, char *ptr, int len)
{
    if ((file != STDOUT_FILENO && file != STDERR_FILENO) || ptr == nullptr || len <= 0) {
        errno = EBADF;
        return -1;
    }

    // HAL_UART_Transmit 的长度参数是 uint16_t，长数据分片发送。
    int remaining = len;
    uint8_t *data = reinterpret_cast<uint8_t *>(ptr);

    while (remaining > 0) {
        uint16_t chunk = static_cast<uint16_t>((remaining > 0xFFFF) ? 0xFFFF : remaining);
        if (HAL_UART_Transmit(&huart6, data, chunk, HAL_MAX_DELAY) != HAL_OK) {
            errno = EIO;
            return -1;
        }
        data += chunk;
        remaining -= chunk;
    }

    return len;
}
