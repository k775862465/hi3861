#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "sys_config.h"
#include "cJSON.h"
#include "cmsis_os2.h"
#include "hal_bsp_mqtt.h"
#include "oled_show_log.h"
#include "mqtt_send_task.h"

#define TASK_DELAY_TIME (1000 * 1000) // us
void publish_sensor_data(msg_data_t *msg);
#define MQTT_TOPIC_MAX 128
char publish_topic[MQTT_TOPIC_MAX] = {0};

/**
 * @brief MQTT  发布消息任务
 */
void mqtt_send_task(void)
{
    while (1) {
        // 发布消息
        publish_sensor_data(&sys_msg_data);
        sleep(5); // 1s
    }
}

/**
 * @brief  发布传感器的信息
 * @note
 * @param  msg:
 * @retval None
 */
void publish_sensor_data(msg_data_t *msg)
{
    // 拼接Topic
    memset_s(publish_topic, MQTT_TOPIC_MAX, 0, MQTT_TOPIC_MAX);
    if (sprintf_s(publish_topic, MQTT_TOPIC_MAX, MQTT_TOPIC_PUB, DEVICE_ID) > 0) {
        // 组装JSON数据
        cJSON *json_root = cJSON_CreateObject();
        cJSON *json_services = cJSON_CreateArray();
        cJSON *json_services_root = cJSON_CreateObject();
        cJSON *json_properties = cJSON_CreateObject();
        cJSON_AddNumberToObject(json_root, "light", msg->AP3216C_Value.light);
        cJSON_AddNumberToObject(json_root, "Temperature",msg->SHT20_Value.Temperature);
        cJSON_AddNumberToObject(json_root, "Humidity", msg->SHT20_Value.Humidity);
        cJSON_AddNumberToObject(json_root, "proximity", msg->AP3216C_Value.proximity);
        cJSON_AddStringToObject(json_root, "device", "1125");
        //cJSON_AddStringToObject(json_properties, "auto_light_control", (msg->is_auto_light_mode) ? "ON" : "OFF");

        char *payload = cJSON_PrintUnformatted(json_root);
        // 发布消息
        MQTTClient_pub(publish_topic, payload, strlen((char *)payload));
        cJSON_Delete(json_root);
        json_root = json_services = json_services_root = json_properties = NULL;
        usleep(TASK_DELAY_TIME);
    }
}
