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

#ifndef MAIN_H
#define MAIN_H

#define mainRUN_ON_CORE 0

/* These tests should work in all modes */
#define mainENABLE_COUNTING_SEMAPHORE 1
#define mainENABLE_DEATH 1

// Tasks = 0
// Memory = 1
// Queue = 2
#define RUN_PROG 2

/* TODO: This still seems flaky on SMP */
#if (portSUPPORT_SMP == 0)
#define mainENABLE_INTERRUPT_QUEUE 1
#endif
#define mainENABLE_MATH 1
#define mainENABLE_QUEUE_OVERWRITE 1
#define mainENABLE_REG_TEST 1
#define mainENABLE_SEMAPHORE 1
#define mainENABLE_TASK_NOTIFY 1

#if (configNUM_CORES != 2)
/* This test just expects two tasks not to run concurrently */
#define mainENABLE_DYNAMIC_PRIORITY 1
#endif

#endif /* MAIN_H */
