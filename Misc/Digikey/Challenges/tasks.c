/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */
/* Kernel includes. */

#include "FreeRTOS.h"
#include "task.h"

/* Library includes. */
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include <pico.h>
#include <pico/stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* The LED toggled by the Rx task. */

#define BLINKLEDPRIORITY (tskIDLE_PRIORITY + 2)
#define USERINPUTPRIORITY (tskIDLE_PRIORITY + 1)
/*-----------------------------------------------------------*/

void Tasks(void);

/*
 */
static volatile int ledBlinkRate = 500;

static void blink_led(void *parameters) {
  int delayRate;

  for (;;) {

    gpio_xor_mask(1u << PICO_DEFAULT_LED_PIN);
    taskENTER_CRITICAL();
    delayRate = ledBlinkRate;
    taskEXIT_CRITICAL();
    vTaskDelay(delayRate);
  }
}

static void listen_for_user_input(void *parameters) {

  int userInputNum;

  for (;;) {
    scanf("%d", &userInputNum);

    taskENTER_CRITICAL();
    ledBlinkRate = userInputNum;
    taskEXIT_CRITICAL();
  }
}

void Tasks(void) {

  static TaskHandle_t blinkHandle;
  volatile TaskHandle_t userInputHandle;

  xTaskCreate(blink_led, "blinkled", 1024, NULL, BLINKLEDPRIORITY,
              &blinkHandle);

  xTaskCreate(listen_for_user_input, "userinput", 1024, NULL, USERINPUTPRIORITY,
              &userInputHandle);

  vTaskStartScheduler();

  for (;;) {
  }
}
