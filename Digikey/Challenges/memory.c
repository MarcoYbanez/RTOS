#include "FreeRTOS.h"
#include "portable.h"
#include "task.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USER_INPUT_PRIORITY (tskIDLE_PRIORITY + 1)
#define PRINT_PRIORITY (tskIDLE_PRIORITY + 1)

// create 2 tasks that mimic a serial echo program using dynamic memory

static volatile char *buf;
int size = 100;

static void user_input(void *parameters) {
  const TaskHandle_t *printHandle = (TaskHandle_t *)parameters;

  char *localBuf = (char *)pvPortMalloc(size * sizeof(char));
  uint8_t count = 0;

  if (localBuf != NULL) {
    char temp;
    for (;;) {
      xTaskNotifyWait(ULONG_MAX, ULONG_MAX, NULL, 20);

      printf("echo > ");
      // get user input and terminate string
      while ((temp = (char)getchar()) != '\r' && count < size - 2) {
        printf("%c", temp);
        localBuf[count++] = temp;
      }

      printf("\n");
      localBuf[count++] = '\0';

      // transfer into buffer
      taskENTER_CRITICAL();
      if ((buf = (char *)pvPortMalloc(count * sizeof(char))) != NULL) {
        memcpy((void *)buf, localBuf, count);
      } else {
        printf("ERROR ALLOC");
      }
      taskEXIT_CRITICAL();

      // notify thread buffer data has transferred
      while (xTaskNotify(*printHandle, count, eSetValueWithOverwrite) ==
             pdFAIL) {
        vTaskDelay(20);
      }

      // reset count
      count = 0;
    }
  }
}

static void print_input(void *param) {
  uint32_t count = 0;
  char *localBuf;
  TaskHandle_t *userHandle = (TaskHandle_t *)param;
  for (;;) {
    // wait until user has finished input string
    xTaskNotifyWait(0, ULONG_MAX, &count, portMAX_DELAY);

    // copy string into local buffer
    taskENTER_CRITICAL();
    localBuf = (char *)pvPortMalloc(count * sizeof(char));

    if (localBuf == NULL) {
      printf("ERROR print\n");
    }

    memcpy((void *)localBuf, (void *)buf, count);
    vPortFree((void *)buf);
    taskEXIT_CRITICAL();

    printf("%s\n", localBuf);
    // printf("Heap: %d\ncount: %d", xPortGetFreeHeapSize(), count);
    vPortFree((void *)localBuf);
    xTaskNotify(*userHandle, 0, eNoAction);
  }
}

void Memory(void) {
  TaskHandle_t userInputHandle;
  TaskHandle_t printInputHandle;

  xTaskCreate(print_input, "print", 1024, &userInputHandle, PRINT_PRIORITY,
              &printInputHandle);

  xTaskCreate(user_input, "UX", 1024, &printInputHandle, USER_INPUT_PRIORITY,
              &userInputHandle);

  vTaskStartScheduler();
}
