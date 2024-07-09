#include "sensor_collect_task.h"
#include "hal_bsp_sht20.h"
#include "hal_bsp_pcf8574.h"
#include "hal_bsp_ssd1306.h"
#include "hal_bsp_ssd1306_bmps.h"
#include "oled_show_log.h"

#include "stdio.h"
#include "hi_io.h"
#include "hi_gpio.h"
#include "sys_config.h"


#define KEY HI_IO_NAME_GPIO_14 // WiFi模组的IO14引脚
#define TASK_DELAY_TIME (300 * 1000) // us
#define SENSOR_COUNT_TIMES (50)
#define DELAY_MS 2  // 步进间隔时间（毫秒）

msg_data_t sys_msg_data = {0};
hi_gpio_value key_value;


/**
 * @brief  传感器采集任务
 * @note
 * @retval None
 */
void sensor_collect_task(void)
{
    char data[21]="";//温湿度
    char val[21]="";//光照和距离
    int fan_flag=0;
    char fan_tip[21]="";
    uint16_t times = 0;
    sys_msg_data.fan_flag=0;
    while (1) {
        // 采集传感器的值
        if(!(times%SENSOR_COUNT_TIMES))
        {
            SHT20_ReadData(&sys_msg_data.SHT20_Value.Temperature, &sys_msg_data.SHT20_Value.Humidity);
            AP3216C_ReadData(&sys_msg_data.AP3216C_Value.infrared, &sys_msg_data.AP3216C_Value.light, &sys_msg_data.AP3216C_Value.proximity);//人体，光照，距离
            snprintf(data,21,"H:%.2f T:%.2f",sys_msg_data.SHT20_Value.Humidity,sys_msg_data.SHT20_Value.Temperature);
            snprintf(val,21,"val:%d dis:%d",sys_msg_data.AP3216C_Value.light,sys_msg_data.AP3216C_Value.proximity);
            oled_show_line_string(OLED_TEXT16_LINE_1, data);
            oled_show_line_string(OLED_TEXT16_LINE_2, val);
            oled_show_line_string(OLED_TEXT16_LINE_3, fan_tip);
            oled_show_line_string(OLED_TEXT16_LINE_0, "device:1125");
            times=0;
        }
        hi_gpio_get_input_val(KEY, &key_value);
        if(sys_msg_data.fan_flag==1)
            snprintf(fan_tip,21,"fan:Auto ");
        else
            snprintf(fan_tip,21,"fan:Manu");
        if(sys_msg_data.AP3216C_Value.proximity>200&&sys_msg_data.AP3216C_Value.light<200)
        {
            sys_msg_data.led_flag=1;
        }else{
            sys_msg_data.led_flag=0;
        }
        if(sys_msg_data.SHT20_Value.Temperature<25&&sys_msg_data.fan_flag==0){
            set_fan(true);
        }else if(sys_msg_data.fan_flag==0){
            set_fan(false);
        }
        if(key_value==0)
        {
            sys_msg_data.fan_flag=(sys_msg_data.fan_flag+1)%2;
            
        }
        //判断是否开灯
        if(sys_msg_data.led_flag==1) set_led(true);
        else set_led(false);
        //判断是否开风扇
        if(sys_msg_data.fan_flag==1) set_fan(true);
        else set_fan(false);
        //判断电机，1上升，2下降,使用完关闭
        if(sys_msg_data.motor_flag==1) {
            forstep();
            sys_msg_data.motor_flag=0;
        }
        if(sys_msg_data.motor_flag==2) {
            backstep();
            sys_msg_data.motor_flag=0;
        }
        if(sys_msg_data.buzzer_flag==1) set_buzzer(true);
        else set_buzzer(false);
        
        usleep(TASK_DELAY_TIME);
    }
}
