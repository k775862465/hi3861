#include <unistd.h>
#include <stdio.h>
#include "motor_task.h"
#include "hal_bsp_structAll.h"

#include "stdio.h"
#include "hi_io.h"
#include "hi_gpio.h"
#include "sys_config.h"


#define TASK_DELAY_TIME (300 * 1000) // us
#define SENSOR_COUNT_TIMES (50)
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
void over(){
    printf("enter over\n");
    sys_msg_data.motor_flag=0;
    hi_gpio_set_ouput_val(STEP_PIN_1, 0);
    hi_gpio_set_ouput_val(STEP_PIN_2, 0);
    hi_gpio_set_ouput_val(STEP_PIN_3, 0);
    hi_gpio_set_ouput_val(STEP_PIN_4, 0);
}
void forstep() {
    int step_index = 0;
    printf("forward\n");
    for(int i=0;i<80;i++){
        for (int k = 0; k< 4; k++) {
            hi_gpio_set_ouput_val(STEP_PIN_1, step_forward[step_index][0]);
            hi_gpio_set_ouput_val(STEP_PIN_2, step_forward[step_index][1]);
            hi_gpio_set_ouput_val(STEP_PIN_3, step_forward[step_index][2]);
            hi_gpio_set_ouput_val(STEP_PIN_4, step_forward[step_index][3]);
            step_index = (step_index + 1) % 4;
            usleep(500);
        }
    }
     printf("forward ok\n");
}

void backstep(){
    int step_index = 0;
    printf("back\n");
     for (size_t i = 0; i < 80; i++)
     {
        for (int k = 0; k< 4; k++) {
            hi_gpio_set_ouput_val(STEP_PIN_1, step_back[step_index][0]);
            hi_gpio_set_ouput_val(STEP_PIN_2, step_back[step_index][1]);
            hi_gpio_set_ouput_val(STEP_PIN_3, step_back[step_index][2]);
            hi_gpio_set_ouput_val(STEP_PIN_4, step_back[step_index][3]);
            step_index = (step_index + 1) % 4;usleep(500);
         }
     }
     printf("back ok\n");
}

/**
 * @brief  传感器采集任务
 * @note
 * @retval None
 */
void motor_task(void)
{
        //判断电机，1上升，2下降,使用完关闭
        if(sys_msg_data.motor_flag==1) {
            forstep();
            over();
        }
        if(sys_msg_data.motor_flag==2) {
            backstep();
             over();
        }
        usleep(TASK_DELAY_TIME);
}
