#include "FreeRTOS.h"

#include "portable.h"
#include "portmacro.h"
#include "projdefs.h"
#include "queue.h"
#include "task.h"
#include <boards/adafruit_feather_rp2040.h>
#include <hardware/gpio.h>
#include <pico/stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TaskA_Priority (tskIDLE_PRIORITY + 1)
#define TaskB_Priority (tskIDLE_PRIORITY + 1)

#define BUF_LENGTH 40

static QueueHandle_t Queue2_handle;
static QueueHandle_t Queue1_handle;

static bool IsDelayCommand(char *entry) {
  if (strstr(entry, "delay"))
    return true;

  return false;
}

static uint32_t GetDelayTime(char *entry) {

  uint32_t val = 0;

  if (IsDelayCommand(entry)) {
    sscanf(entry, "delay %d", &val);
  }

  return val;
}

static void TaskA(void *param) {
  // reads serial input
  // echo input back to serial terminal
  // prints any new message from the queue 2
  // if "delay xxx" send xxx(number) to queue 1

  BaseType_t queueReadResult;
  int index = 0;
  char t = '\0';
  char *buf = NULL;
  volatile char *queue2Buf = (char *)pvPortMalloc(sizeof(char[BUF_LENGTH]));

  for (;;) {
    t = '\0';
    buf = (char *)pvPortMalloc(sizeof(char[BUF_LENGTH]));
    index = 0;
    BaseType_t queueReadResult;
    // print string to continue where user input left off

    if (queueReadResult != pdPASS)
      printf("echo > ");

    // read serial input until queue input is xQueueReceive
    while ((queueReadResult = xQueueReceive(Queue2_handle, queue2Buf, 0)) ==
               errQUEUE_EMPTY &&
           ((t = getchar()) != '\r' && index < BUF_LENGTH - 2)) {
      buf[index++] = t;
      printf("%c", t);
    }

    if (queueReadResult == pdPASS) {
      printf("\n%s\n", queue2Buf);
    } else {
      uint32_t delay = 0;
      if ((delay = GetDelayTime(buf)) > 0) {
        // add to queue
        xQueueSend(Queue1_handle, &delay, 50);
      } else {
        buf[index] = '\0';
        xQueueSend(Queue2_handle, buf, 50);
      }
      printf("\n");
    }

    printf("end") vPortFree(buf);
  }
  vPortFree(queue2Buf);
}

static void TaskB(void *param) {
  // updates t with any new values from queue 1
  // blinks LED with t delay
  // every time LED blinks 100 times, send "Blinked" string to queue 2 and
  // number of times Blinked

  uint32_t delay = 500;
  int counter = 0;
  int totalCounter = 0;
  for (;;) {

    if (counter >= 100) {
      char *buf = (char *)pvPortMalloc(sizeof(char[BUF_LENGTH]));
      snprintf(buf, BUF_LENGTH, "Blinked %d times\n", totalCounter + 1);
      printf(buf, "Blinked %d times\n", totalCounter);

      if (xQueueSend(Queue2_handle, &buf, portMAX_DELAY) == pdPASS) {
        counter = 0;
        ++totalCounter;
      }

      vPortFree(buf);
    }

    xQueueReceive(Queue1_handle, &delay, 0);
    gpio_xor_mask(1u << PICO_DEFAULT_LED_PIN);
    vTaskDelay(delay);
    counter++;
  }
}

void Queue(void) {

  Queue1_handle = xQueueCreate(20, sizeof(uint32_t));
  Queue2_handle = xQueueCreate(20, BUF_LENGTH);

  TaskHandle_t TaskA_Handle;
  TaskHandle_t TaskB_Handle;

  xTaskCreate(TaskA, "IO", 1024 * 5, NULL, TaskA_Priority, &TaskA_Handle);

  xTaskCreate(TaskB, "Blink", 1024, NULL, TaskB_Priority, &TaskB_Handle);

  vTaskStartScheduler();
}
