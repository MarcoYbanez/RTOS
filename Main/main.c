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

/* Scheduler include files. */
#include "BME280.h"
#include "FreeRTOS.h"
#include "I2C/Bme280.h"
#include "SPI/W25Q16.h"
#include "semphr.h"
#include "task.h"
#include <boards/adafruit_feather_rp2040.h>
#include <hardware/gpio.h>
#include <pico/time.h>

#include "main.h"

/* Library includes. */
#include "pico/stdlib.h"
#include <stdio.h>
#if (mainRUN_ON_CORE == 1)
#include "pico/multicore.h"
#endif

static void SetupHardware(void);

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

/*-----------------------------------------------------------*/
#define BME_THREAD_PRIORITY (tskIDLE_PRIORITY + 1)
#define W25Q_THREAD_PRIORITY (tskIDLE_PRIORITY + 1)
/*-----------------------------------------------------------*/

extern void I2C(void);
extern void W25Q_Init();
extern void BME_Init();
extern void BME_GetDeviceIdLoop(void *);
extern void W25Q_GetDeviceIdLoop(void *);

void ThreadDispatch(void) {
  //
  xTaskCreate(W25Q_GetDeviceIdLoop, "W25Q_Id", configMINIMAL_STACK_SIZE, NULL,
              W25Q_THREAD_PRIORITY, NULL);

  xTaskCreate(BME_GetDeviceIdLoop, "BME_Id", configMINIMAL_STACK_SIZE, NULL,
              BME_THREAD_PRIORITY, NULL);

  vTaskStartScheduler();
}

void vLaunch(void) {
  /* The mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is described at the top
of this file. */
  I2C();
}

int main(void) {
  SetupHardware();

  ThreadDispatch();

  for (;;) {
  }

  return 0;
}
/*-----------------------------------------------------------*/

static void SetupHardware(void) {
  stdio_init_all();

  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, 1);
  gpio_put(PICO_DEFAULT_LED_PIN, !PICO_DEFAULT_LED_PIN_INVERTED);
}
/*-----------------------------------------------------------*/

// FreeRTOS Override
void vApplicationMallocFailedHook(void) {
  /* Called if a call to pvPortMalloc() fails because there is insufficient
  free memory available in the FreeRTOS heap.  pvPortMalloc() is called
  internally by FreeRTOS API functions that create tasks, queues, software
  timers, and semaphores.  The size of the FreeRTOS heap is set by the
  configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

  /* Force an assert. */
  configASSERT((volatile void *)NULL);
}
/*-----------------------------------------------------------*/

// FreeRTOS Override
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
  (void)pcTaskName;
  (void)pxTask;

  /* Run time stack overflow checking is performed if
  configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
  function is called if a stack overflow is detected. */

  /* Force an assert. */
  configASSERT((volatile void *)NULL);
}
/*-----------------------------------------------------------*/

// FreeRTOS Override
void vApplicationIdleHook(void) {
  volatile size_t xFreeHeapSpace;

  /* This is just a trivial example of an idle hook.  It is called on each
  cycle of the idle task.  It must *NOT* attempt to block.  In this case the
  idle task just queries the amount of FreeRTOS heap that remains.  See the
  memory management section on the http://www.FreeRTOS.org web site for memory
  management options.  If there is a lot of heap memory free then the
  configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
  RAM. */
  xFreeHeapSpace = xPortGetFreeHeapSize();

  /* Remove compiler warning about xFreeHeapSpace being set but never used. */
  (void)xFreeHeapSpace;
}
/*-----------------------------------------------------------*/

// FreeRTOS Override -- check action later
void vApplicationTickHook(void) {}
