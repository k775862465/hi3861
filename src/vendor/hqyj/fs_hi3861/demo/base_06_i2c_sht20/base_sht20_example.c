/*
 * Copyright (c) 2023 Beijing HuaQing YuanJian Education Technology Co., Ltd
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hal_bsp_sht20.h"
#include "hal_bsp_pcf8574.h"

osThreadId_t Task1_ID; // 任务1设置为低优先级任务
#define TASK_STACK_SIZE 1024

void Task1(void)
{
    float temperature = 0, humidity = 0;
    while (1) {
        SHT20_ReadData(&temperature, &humidity);
        printf("temperature = %.2f      humidity = %.2f\r\n", temperature, humidity);
        sleep(1); // 1s
    }
}
static void base_sht20_demo(void)
{
    printf("Enter base_sht20_demo()!");

    PCF8574_Init();
    SHT20_Init(); // SHT20初始化

    osThreadAttr_t options;
    options.name = "thread_1";
    options.attr_bits = 0;
    options.cb_mem = NULL;
    options.cb_size = 0;
    options.stack_mem = NULL;
    options.stack_size = TASK_STACK_SIZE;
    options.priority = osPriorityNormal;

    Task1_ID = osThreadNew((osThreadFunc_t)Task1, NULL, &options);
    if (Task1_ID != NULL) {
        printf("ID = %d, Create Task1_ID is OK!\r\n", Task1_ID);
    }
}
SYS_RUN(base_sht20_demo);