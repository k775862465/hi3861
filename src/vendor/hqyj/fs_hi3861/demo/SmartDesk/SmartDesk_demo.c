
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "cJSON.h"


#include "hal_bsp_wifi.h"
#include "hal_bsp_mqtt.h"
#include "hal_bsp_ap3216c.h"
#include "hal_bsp_ssd1306.h"
#include "hal_bsp_pcf8574.h"
#include "hal_bsp_sht20.h"
#include "mqtt_send_task.h"
#include "mqtt_recv_task.h"
#include "sensor_collect_task.h"
#include "oled_show_log.h"
#include "hi_pwm.h"
#include "hi_io.h"
#include "hi_gpio.h"
#include "sys_config.h"
#include "sensor_collect_task.h"
#include "motor_task.h"

#define TASK_STACK_SIZE (5 * 1024)
#define TASK_INIT_DELAY 5 // s
#define KEY HI_IO_NAME_GPIO_14 // WiFi模组的IO14引脚

osThreadId_t mqtt_send_task_id;      // mqtt 发布数据任务ID
osThreadId_t mqtt_recv_task_id;      // mqtt 接收数据任务ID
osThreadId_t sensor_collect_task_id; // 传感器采集任务ID
osThreadId_t motor_task_id; //电机驱动任务
hi_gpio_value gpio_value;


int8_t mqtt_sub_payload_callback(unsigned char *topic, unsigned char *payload)
{
    printf("[info] topic:[%s]    recv<== %s\r\n", topic, payload);
}

int mqtt_init(void)
{
   // 连接Wifi
    if (WiFi_connectHotspots(WIFI_INFO, WIFI_PWD) != WIFI_SUCCESS) {
        printf("[error] WiFi_connectHotspots\r\n");
    }

    // 连接MQTT服务器
    if (MQTTClient_connectServer(SERVER_IP_ADDR, SERVER_IP_PORT) != 0) {
        printf("[error] MQTTClient_connectServer\r\n");
    } else {
        printf("[success] MQTTClient_connectServer\r\n");
    }
    sleep(TASK_INIT_DELAY);

    // 初始化MQTT客户端
    if (MQTTClient_init(MQTT_CLIENT_ID,MQTT_USER_NAME, MQTT_PASS_WORD) != 0) {
        printf("[error] MQTTClient_init\r\n");
    } else {
        printf("[success] MQTTClient_init\r\n");
    }
    sleep(TASK_INIT_DELAY);

    // 订阅Topic
    if (MQTTClient_subscribe(MQTT_TOPIC_SUB) != 0) {
        printf("[error] MQTTClient_subscribe\r\n");
    } else {
        printf("[success] MQTTClient_subscribe\r\n");
    }
    sleep(TASK_INIT_DELAY);

    return 0;
}

void motor_init(){
    hi_gpio_set_dir(HI_GPIO_IDX_8, HI_GPIO_DIR_OUT);
    hi_gpio_set_dir(HI_GPIO_IDX_12, HI_GPIO_DIR_OUT); 
    hi_gpio_set_dir(HI_GPIO_IDX_11, HI_GPIO_DIR_OUT); 
    hi_gpio_set_dir(HI_GPIO_IDX_5, HI_GPIO_DIR_OUT); 
}

void peripheral_Init(void)
{
    // 外设的初始化
    hi_pwm_init(HI_PWM_PORT_PWM4); //pwm初始化
    KEY_Init();    // 按键初始化
    PCF8574_Init();  // 初始化IO扩展芯片
   // AW2013_Init(); // 三色LED灯的初始化
   // AW2013_Control_RGB(0, 0, 0);
    AP3216C_Init(); // 三合一传感器初始化
    SSD1306_Init(); // OLED 显示屏初始化
    SSD1306_CLS();  // 清屏
    SSD1306_ShowStr(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_0, "Smart Desk", TEXT_SIZE_16);
    PCF8574_Init();
    SHT20_Init(); // SHT20初始化 
    motor_init(); //电机初始化
}
static void SmartDesk_Project(void)
{
    printf("Enter smartDesk_Projec()!\r\n");
   p_MQTTClient_sub_callback = &mqtt_sub_payload_callback;
    // 全局变量的初始化
    peripheral_Init();
    
    mqtt_init();
    //  创建线程
    osThreadAttr_t options;
    options.name = "sensor_collect_task";
    options.attr_bits = 0;
    options.cb_mem = NULL;
    options.cb_size = 0;
    options.stack_mem = NULL;
    options.stack_size = TASK_STACK_SIZE;
    options.priority = osPriorityNormal;
    mqtt_send_task_id = osThreadNew((osThreadFunc_t)sensor_collect_task, NULL, &options);
    if (mqtt_send_task_id != NULL) {
        printf("ID = %d, Create sensor_collect_task_id is OK!\r\n", sensor_collect_task_id);
    }
    options.name = "mqtt_send_task";
    mqtt_send_task_id = osThreadNew((osThreadFunc_t)mqtt_send_task, NULL, &options);
    if (mqtt_send_task_id != NULL) {
        printf("ID = %d, Create mqtt_send_task_id is OK!\r\n", mqtt_send_task_id);
    }

    options.name = "mqtt_recv_task";
    mqtt_recv_task_id = osThreadNew((osThreadFunc_t)mqtt_recv_task, NULL, &options);
    if (mqtt_recv_task_id != NULL) {
        printf("ID = %d, Create mqtt_recv_task_id is OK!\r\n", mqtt_recv_task_id);
    }

    options.name = "motor_task";
    motor_task_id = osThreadNew((osThreadFunc_t)motor_task, NULL, &options);
    if (mqtt_recv_task_id != NULL) {
        printf("ID = %d, Create mqtt_recv_task_id is OK!\r\n", motor_task_id);
    }
}

SYS_RUN(SmartDesk_Project);