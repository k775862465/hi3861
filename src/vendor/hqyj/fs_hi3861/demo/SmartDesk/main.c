#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "ohos_init.h"
#include "cmsis_os2.h"


#define TASK_STACK_SIZE (1024 * 5)
static void SmartDesk_main(void)
{




   // 创建系统初始化任务
    // osThreadAttr_t options;
    // options.name = "system_Init_Task";
    // options.attr_bits = 0;
    // options.cb_mem = NULL;
    // options.cb_size = 0;
    // options.stack_mem = NULL;
    // options.stack_size = TASK_STACK_SIZE;
    // options.priority = osPriorityNormal;
    // system_Init_Task_ID = osThreadNew((osThreadFunc_t)system_Init_Task, NULL, &options);
    // if (system_Init_Task_ID != NULL) {
    //     printf("ID = %d, Create mqtt_send_task_id is OK!\r\n", system_Init_Task_ID);
    // }
}
SYS_RUN(SmartDesk_main);