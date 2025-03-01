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
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "main.h"

/* Library includes. */
#include "pico/stdlib.h"
#include <stdio.h>
#if (mainRUN_ON_CORE == 1)
#include "pico/multicore.h"
#endif

/*-----------------------------------------------------------*/

/*
 * Configure the hardware as necessary to run this demo.
 */
static void prvSetupHardware(void);
extern void Tasks(void);
extern void Memory(void);
extern void Queue(void);

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

/*-----------------------------------------------------------*/

void vLaunch(void) {
  /* The mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is described at the top
of this file. */
#if RUN_PROG == 0
  Tasks();
#elif RUN_PROG == 1
  Memory();
#elif RUN_PROG == 2
  Queue();
#endif
}

int main(void) {
  /* Configure the hardware ready to run the demo. */
  prvSetupHardware();
  const char *rtos_name;
#if (portSUPPORT_SMP == 1)
  rtos_name = "FreeRTOS SMP";
#else
  rtos_name = "FreeRTOS";
#endif

#if (portSUPPORT_SMP == 1) && (configNUMBER_OF_CORES == 2)
  printf("%s on both cores:\n", rtos_name);
  vLaunch();
#endif

#if (mainRUN_ON_CORE == 1)
  printf("%s on core 1:\n", rtos_name);
  multicore_launch_core1(vLaunch);
  while (true)
    ;
#else
  printf("%s on core 0:\n", rtos_name);
  vLaunch();
  for (;;)
    printf("Error\n");
#endif

  return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware(void) {
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
