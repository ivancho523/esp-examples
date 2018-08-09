/*

Copyright (c) 2018 Mika Tuupola

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

/*
#include <stdio.h>
#include <stdint.h>
#include <string.h>

*/

#include <esp_log.h>
#include <driver/uart.h>

#include "sdkconfig.h"

//#define UART_RX_PIN             21
//  GPIO16-TXD
// GPIO17-RXD
// GPIO34-PPS

#define UART_RX_BUF_SIZE    2048
#define UART_TX_BUF_SIZE    0
#define UART_QUEUE_SIZE     0
#define UART_QUEUE_HANDLE   NULL
#define UART_INTR_FLAGS     0

static const char* TAG = "main";

void uart_init()
{
    uart_config_t uart_config = {
        //.baud_rate = 115200,
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        // .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        // .rx_flow_ctrl_thresh = 122,
    };

    ESP_ERROR_CHECK(
        uart_param_config(UART_NUM_2, &uart_config)
    );
    ESP_ERROR_CHECK(
        uart_driver_install(
            UART_NUM_2,
            UART_RX_BUF_SIZE,
            UART_TX_BUF_SIZE,
            UART_QUEUE_SIZE,
            UART_QUEUE_HANDLE,
            UART_INTR_FLAGS
        )
    );
    /*
    // Set UART pins(TX: IO16 (UART2 default), RX: IO17 (UART2 default), RTS: IO18, CTS: IO19)
ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, 18, 19));
    */

    ESP_ERROR_CHECK(
        uart_set_pin(
            UART_NUM_2,
            GPIO_NUM_16,
            GPIO_NUM_17,
            UART_PIN_NO_CHANGE,
            UART_PIN_NO_CHANGE
        )
    );
}

void uart_read_task(void *params)
{
    char data[1];
    char *ptr = data;

    ESP_LOGI(TAG, "Reading uart...");
    while(1) {
        int8_t size = uart_read_bytes(UART_NUM_2, (unsigned char *)ptr, 1, 500 / portTICK_RATE_MS);
        ESP_LOGI(TAG, "%s, %d", data, size);
        //vTaskDelay(1000 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

void app_main()
{
    uart_init();
    xTaskCreatePinnedToCore(uart_read_task, "UART read", 2048, NULL, 1, NULL, 1);
}