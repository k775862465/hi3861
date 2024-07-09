#include "mqtt_recv_task.h"
#include "hal_bsp_mqtt.h"
#include "hal_bsp_aw2013.h"
#include "hal_bsp_structAll.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "cJSON.h"
#include "cmsis_os2.h"
#include "sys_config.h"

#define MQTT_RECV_TASK_TIME (200 * 1000) // us

int get_jsonData_value(const cJSON *const object)
{
    cJSON *json_value = NULL;
    int ret = -1;
    if (!strcmp(object->valuestring, "ON")) {
            json_value = NULL;
            ret = 1; // 0为成功
        } else if (!strcmp(object->valuestring, "OFF")) {
            json_value = NULL;
            ret = 0;
        }
    json_value = NULL;
    return ret; // -1为失败
}

int get_jsonMotor_value(const cJSON *const object)
{
    cJSON *json_value = NULL;
    int ret = -1;
    if (!strcmp(object->valuestring, "UP")) {
            json_value = NULL;
            ret = 1; // 0为成功
        } else if (!strcmp(object->valuestring, "DOWN")) {
            json_value = NULL;
            ret = 2;
        }
    json_value = NULL;
    return ret; // -1为失败
}

// 解析JSON数据
uint8_t cJSON_Parse_Payload(uint8_t *payload)
{
    uint8_t ret = 0;
    printf("send success\n");
    //解析json文件
    cJSON *root = cJSON_Parse((const char *)payload);
    cJSON *product_id = cJSON_GetObjectItem(root,"product_ID");//发送产品ID号
    cJSON *command_name = cJSON_GetObjectItem(root, "command_name"); //发送需要控制的设备
    cJSON *paras = cJSON_GetObjectItem(root, "paras");//发送ON是开,OFF是关
    printf("%s\n",cJSON_Print(root)); 
    if (paras && root && product_id && command_name && !strcmp(product_id->valuestring,"1125")){
        if (!strcmp(command_name->valuestring, "fan")) {
            // 风扇开启自动模式
            sys_msg_data.fan_flag=get_jsonData_value(paras);
            printf("after recv fan:%d\n",sys_msg_data.fan_flag);
        }
        if (!strcmp(command_name->valuestring, "motor")) {
            // 风扇开启自动模式
            sys_msg_data.motor_flag=get_jsonMotor_value(paras);
            printf("after recv fan:%d\n",sys_msg_data.motor_flag);
        }
         if (!strcmp(command_name->valuestring, "buzzer")) {
            // 风扇开启自动模式
            sys_msg_data.buzzer_flag=get_jsonData_value(paras);
            printf("after recv fan:%d\n",sys_msg_data.buzzer_flag);
        }
    }
    cJSON_Delete(root);
    root = NULL;
    product_id  = NULL;
    command_name = NULL;
    paras = NULL;
    
    return 0;
}
// 向云端发送返回值
void send_cloud_request_code(const char *request_id, int ret_code, int request_len)
{
    char *request_topic = (char *)malloc(strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) +
                                         strlen(DEVICE_ID) + request_len + 1);
    if (request_topic != NULL) {
        memset_s(request_topic,
                 strlen(DEVICE_ID) + strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + request_len + 1,
                 0,
                 strlen(DEVICE_ID) + strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + request_len + 1);
        if (sprintf_s(request_topic,
                      strlen(DEVICE_ID) + strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + request_len + 1,
                      MQTT_TOPIC_PUB_COMMANDS_REQ, DEVICE_ID, request_id) > 0) {
            if (ret_code == 0) {
                MQTTClient_pub(request_topic, "{\"result_code\":0}", strlen("{\"result_code\":0}"));
            } else if (ret_code == 1) {
                MQTTClient_pub(request_topic, "{\"result_code\":1}", strlen("{\"result_code\":1}"));
            }
        }
        free(request_topic);
        request_topic = NULL;
    }
}
/**
 * @brief MQTT接收数据的回调函数
 */
int8_t mqttClient_sub_callback(unsigned char *topic, unsigned char *payload)
{
    if ((topic == NULL) || (payload == NULL)) {
        return -1;
    } else {
        printf("mqttClient_sub_callback topic: %s\r\n", topic);
        printf("mqttClient_sub_callback payload: %s\r\n", payload);

        // 提取出topic中的request_id
        char request_id[50] = {0};
        int ret_code = 1; // 0为成功, 其余为失败。不带默认表示成功
        if (strcpy_s(request_id, sizeof(request_id),
                     topic + strlen(DEVICE_ID) + strlen("$oc/devices//sys/commands/request_id=")) == 0) {
            printf("request_id: %s\r\n", request_id);
            // 解析JSON数据并控制
            ret_code = cJSON_Parse_Payload(payload);
            send_cloud_request_code(request_id, ret_code, sizeof(request_id));
        }
    }
    return 0;
}

/**
 * @brief MQTT  接收消息任务
 */
void mqtt_recv_task(void)
{
    p_MQTTClient_sub_callback = &mqttClient_sub_callback;
    while (1) {
        MQTTClient_sub();
        usleep(MQTT_RECV_TASK_TIME);
    }
}
