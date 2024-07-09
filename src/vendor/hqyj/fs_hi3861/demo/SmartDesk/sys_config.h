

#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

#include "cmsis_os2.h"
#include "hal_bsp_structAll.h"

//WIFI账号
#define WIFI_INFO "4kkkk"
#define WIFI_PWD "4k021125"

// 设备密码 fs12345678
// 设备ID
#define DEVICE_ID "csk1125"
// MQTT客户端ID
#define MQTT_CLIENT_ID "mqtt_client_123"
// MQTT用户名
#define MQTT_USER_NAME "csk"
// MQTT密码
#define MQTT_PASS_WORD "4k021125"
// 华为云平台的IP地址
#define SERVER_IP_ADDR "47.120.25.231"
// 华为云平台的IP端口号
#define SERVER_IP_PORT 1883
// 订阅 接收控制命令的主题
#define MQTT_TOPIC_PUB "pubTopic"
// 发布 成功接收到控制命令后的主题
#define MQTT_TOPIC_SUB "subTopic"

// 发布 成功接收到控制命令后的主题
#define MQTT_TOPIC_PUB_COMMANDS_REQ "$oc/devices/%s/sys/commands/response/request_id=%s"
#define MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ "$oc/devices//sys/commands/response/request_id="

// 发布 设备属性数据的主题
#define MQTT_TOPIC_PUB_PROPERTIES "$oc/devices/%s/sys/properties/report"
#define MALLOC_MQTT_TOPIC_PUB_PROPERTIES "$oc/devices//sys/properties/report"

// 三色灯的PWM值
typedef struct _RGB_Value {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGB_Value_t;

// 三合一传感器
typedef struct _AP3216C_Value {
    uint16_t light;     // 光照强度
    uint16_t proximity; // 接近传感器
    uint16_t infrared;  // 人体红外传感器
} AP3216C_Value_t;

//温湿度传感器
typedef struct _SHT20_Value
{
    /* data */
    float Temperature;
    float Humidity;
}SHT20_Value_t;


// 灯的状态
typedef enum _Lamp_Status {
    OFF_LAMP = 0,
    SUN_LIGHT_MODE, // 白光模式
    SLEEP_MODE,     // 睡眠模式
    READ_BOOK_MODE, // 看书模式
    LED_BLINK_MODE, // 闪烁模式
    SET_RGB_MODE,   //   RGB调光模式
} Lamp_Status_t;

typedef struct message_data {
    uint16_t fan_flag;//风扇状态
    uint16_t led_flag;//灯光状态
    SHT20_Value_t SHT20_Value;  //温湿度
    Lamp_Status_t Led_Status;     // 控制灯是否开灯
    AP3216C_Value_t AP3216C_Value; // 三合一传感器的数据
    uint16_t motor_flag; //升降桌
    uint16_t buzzer_flag;
} msg_data_t;

// 日期、时间
typedef struct date_time_value {
    uint16_t yaer;
    uint8_t month;
    uint8_t date;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} date_time_value_t;

extern msg_data_t sys_msg_data; // 传感器的数据

#endif
