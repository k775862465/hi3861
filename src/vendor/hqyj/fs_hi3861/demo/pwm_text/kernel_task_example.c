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
#include "hal_bsp_pcf8574.h"
#include "hi_pwm.h"
#include "hi_io.h"
#include "hi_gpio.h"

osThreadId_t Task1_ID;   //  任务1 ID
osThreadId_t Task2_ID;   //  任务2 ID
#define TASK_STACK_SIZE 1024
#define TASK1_DELAY_TIME 8 // s
#define TASK2_DELAY_TIME 5 // s
#define STEPS_PER_REVOLUTION 200  // 步进电机每转步数
#define DELAY_MS 2  // 步进间隔时间（毫秒）
#define STEP_PIN_1 HI_IO_NAME_GPIO_8  // 步进电机控制引脚1
#define STEP_PIN_2 HI_IO_NAME_GPIO_12  // 步进电机控制引脚2
#define STEP_PIN_3 HI_IO_NAME_GPIO_11  // 步进电机控制引脚3
#define STEP_PIN_4 HI_IO_NAME_GPIO_5  // 步进电机控制引脚4

// 步进电机控制顺序（可以根据步进电机型号调整）正转
int step_forward[4][4] = {
    {1, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 1},
    {1, 0, 0, 1}
};
//反转
int step_back[4][4] = {
    {1, 0, 0, 1},
    {0, 0, 1, 1},
    {0, 1, 1, 0},
    {1, 1, 0, 0}
};

 void step() {
    static int step_index = 0;
    // printf("forward\n");
    // for(int i=0;i<10;i++){
    //     for (int k = 0; k< 4; k++) {
    //         hi_gpio_set_ouput_val(STEP_PIN_1, step_forward[step_index][0]);
    //         hi_gpio_set_ouput_val(STEP_PIN_2, step_forward[step_index][1]);
    //         hi_gpio_set_ouput_val(STEP_PIN_3, step_forward[step_index][2]);
    //         hi_gpio_set_ouput_val(STEP_PIN_4, step_forward[step_index][3]);
    //         step_index = (step_index + 1) % 4;usleep(500);
    //     }
    // }
    //  printf("back\n");
    //  sleep(3);
     for (size_t i = 0; i < 10; i++)
     {
        
        for (int k = 0; k< 4; k++) {
            hi_gpio_set_ouput_val(STEP_PIN_1, step_back[step_index][0]);
            hi_gpio_set_ouput_val(STEP_PIN_2, step_back[step_index][1]);
            hi_gpio_set_ouput_val(STEP_PIN_3, step_back[step_index][2]);
            hi_gpio_set_ouput_val(STEP_PIN_4, step_back[step_index][3]);
            //  hi_gpio_set_ouput_val(STEP_PIN_1, HI_GPIO_VALUE1);
            //   hi_gpio_set_ouput_val(STEP_PIN_2, HI_GPIO_VALUE0);
            //    hi_gpio_set_ouput_val(STEP_PIN_3,HI_GPIO_VALUE1);
            //     hi_gpio_set_ouput_val(STEP_PIN_4, HI_GPIO_VALUE0);
            step_index = (step_index + 1) % 4;usleep(500);
         }
     }
}


/**
 * @description: 任务1为低优先级任务
 * @param {*}
 * @return {*}
 */
void Task1(void)
{
    while (1) {
        printf("start\n");
       for (int i = 0; i < 360; i++) {
        step();
        }   
        sleep(TASK1_DELAY_TIME);
        printf("stop\n");
        // hi_gpio_set_ouput_val(HI_GPIO_IDX_13,  HI_GPIO_VALUE0);
        hi_pwm_stop(HI_PWM_PORT_PWM4);
        sleep(TASK1_DELAY_TIME);
    }
}
/**
 * @description: 任务2为高优先级任务
 * @param {*}
 * @return {*}
 */

/**
 * @description: 初始化并创建任务
 * @param {*}
 * @return {*}
 */
static void kernel_task_example(void)
{
    printf("Enter PWM()!\n");
    hi_gpio_init();
    //hi_io_set_func(HI_GPIO_IDX_13,HI_IO_FUNC_GPIO_13_PWM4_OUT);
    //hi_gpio_set_dir(HI_GPIO_IDX_13,HI_GPIO_DIR_OUT);
    // hi_gpio_set_dir(HI_GPIO_IDX_9,HI_GPIO_DIR_OUT);
    // hi_gpio_set_dir(HI_GPIO_IDX_10,HI_GPIO_DIR_OUT);
    
    //hi_io_set_pull(HI_GPIO_IDX_8, HI_IO_PULL_UP);   
    // hi_io_set_pull(HI_GPIO_IDX_11, HI_IO_PULL_UP);   
    // hi_io_set_pull(HI_GPIO_IDX_12, HI_IO_PULL_UP);   
    // hi_io_set_pull(HI_GPIO_IDX_5, HI_IO_PULL_UP);  
   
    // hi_io_set_func(HI_GPIO_IDX_8,HI_IO_FUNC_GPIO_8_GPIO);
    // hi_io_set_func(HI_GPIO_IDX_12,HI_IO_FUNC_GPIO_12_GPIO);
    // hi_io_set_func(HI_GPIO_IDX_11,HI_IO_FUNC_GPIO_11_GPIO);
    // hi_io_set_func(HI_GPIO_IDX_5,HI_IO_FUNC_GPIO_5_GPIO);

    hi_gpio_set_dir(HI_GPIO_IDX_8, HI_GPIO_DIR_OUT);
    hi_gpio_set_dir(HI_GPIO_IDX_12, HI_GPIO_DIR_OUT); 
    hi_gpio_set_dir(HI_GPIO_IDX_11, HI_GPIO_DIR_OUT); 
    hi_gpio_set_dir(HI_GPIO_IDX_5, HI_GPIO_DIR_OUT); 

    //  HI_GPIO_SetOutputMode(HI_GPIO_IDX_8, HI_GPIO_MODE_PP);
    //   HI_GPIO_SetOutputMode(HI_GPIO_IDX_12, HI_GPIO_MODE_PP);
    //    HI_GPIO_SetOutputMode(HI_GPIO_IDX_11, HI_GPIO_MODE_PP);
    //     HI_GPIO_SetOutputMode(HI_GPIO_IDX_5, HI_GPIO_MODE_PP);

    //  hi_io_set_func(HI_GPIO_IDX_10,HI_IO_FUNC_GPIO_10_GPIO);
    PCF8574_Init();
    osThreadAttr_t options;
    options.name = "Task1";       // 任务的名字
    options.attr_bits = 0;      // 属性位
    options.cb_mem = NULL;      // 堆空间地址
    options.cb_size = 0;        // 堆空间大小
    options.stack_mem = NULL;   // 栈空间地址
    options.stack_size = TASK_STACK_SIZE;  // 栈空间大小 单位:字节
    options.priority = osPriorityNormal;  // 任务的优先级

    Task1_ID = osThreadNew((osThreadFunc_t)Task1, NULL, &options);      // 创建任务1
    if (Task1_ID != NULL) {
        printf("ID = %d, Create Task1_ID is OK!\n", Task1_ID);
    }

   
}
SYS_RUN(kernel_task_example);
