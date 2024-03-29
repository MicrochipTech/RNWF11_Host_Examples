 /*
 * MAIN Generated Driver File
 * 
 * @file rnwf_app.c
 * 
 * @defgroup 
 *
 * @ingroup
 * 
 * @brief 
 *
 * @version Driver Version 1.0.0
*/

/*
? [2023] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include "rnwf_app.h"
#include "rnwf_wifi_service.h"
#include "rnwf_mqtt_service.h"
#include "rnwf_net_service.h"
#include "rnwf_system_service.h"
#ifdef RNWF11_SERVICE
#include "rnwf_ecc_service.h"
#endif

/*
    Main application
*/


/* MQTT Connection States */
#define MQTT_DISCONNECTED       0
#define MQTT_CONNECTING         1
#define MQTT_CONNECTED          2

uint8_t g_isMqttConnected = MQTT_DISCONNECTED;
#ifdef RNWF11_SERVICE
uint32_t gSysTickCount;
#endif

/* Application buffer */
uint8_t app_buf[APP_BUFFER_SIZE_MAX];
/* MQTT Subscribe Topic Name List */
static const char *subscribe_list[] = {"mchp/rnwf/to", NULL, NULL, NULL, NULL};
static uint8_t subCnt;
#ifdef RNWF11_SERVICE
#define MQTT_PUBLISH_INTERVAL (1000 * 5) //5 Sec
#endif
/* TLS Configuration details */
const char *cloud_tls_cfg[] = {"AmazonRootCA1", 0, 0, 0, "aznb5n71crum1-ats.iot.us-east-1.amazonaws.com", 0};
RNWF_MQTT_CFG_t mqtt_cfg = {
    .url = "aznb5n71crum1-ats.iot.us-east-1.amazonaws.com",
    .clientid = CLIENT_ID,
    .username = "",
    .password = "",
    .port = 8883,
    .tls_conf = cloud_tls_cfg,
    .tls_idx = RNWF_NET_TLS_CONFIG_2,
    .azure_dps = 0
#ifdef RNWF11_SERVICE
	,.tls_use_ecc608 = RNWF_TLS_USE_ECC608
#endif            
};


void APP_AWS_Telemetry(uint8_t sensorInput1, uint8_t sensorInput2)
{
    snprintf(app_buf, sizeof(app_buf), AWS_FMT_TEMP_LIGHT, sensorInput1, sensorInput2);
    printf("Telemetry ->> sensorInput1 %d & sensorInput2 = %d\r\n", sensorInput1, sensorInput2);
    APP_MQTT_Publish(AWS_PUB_TELEMETRY, app_buf);
    }

void APP_AWS_SUBACK_Handler(void)
{
#ifdef RNWF11_SERVICE
    static int led_value = 0;
    led_value++;
#endif    
    if(subscribe_list[subCnt] != NULL)
    {
        sprintf(app_buf, "%s", subscribe_list[subCnt++]);
        RNWF_MQTT_SrvCtrl(RNWF_MQTT_SUBSCRIBE_QOS0, app_buf);            
    }    
    else
    {           
#ifdef RNWF11_SERVICE        
        sprintf(app_buf, AWS_FMT_SHADOW_LIGHT, (led_value%2)?1:0);
#else        
        sprintf(app_buf, AWS_FMT_SHADOW_LIGHT, !LED_GetValue());
#endif        
        APP_MQTT_Publish(AWS_PUB_SHADOW_UPDATE, app_buf);
    }
}

void APP_AWS_Task(void)
{
#ifdef RNWF11_SERVICE
    static uint8_t input1 = 0xff;
    static uint8_t input2 = 0;
    
    if((gSysTickCount % MQTT_PUBLISH_INTERVAL) == 0)
    {
        APP_AWS_Telemetry(input1--, input2++);    //this will continuously trigger telemetry action
    }
#else
    /* Implement app specific Azure_Task() method here */  
    /* set periodic events for telemetry */
    APP_AWS_Telemetry(input1++, input2++);    //this will continuously trigger telemetry action
#endif    
}

void APP_AWS_SUB_Handler(char *p_str)
    {                                                   
    if(strstr(p_str, "shadow/update/delta"))
    {        
        /* Complete app-specific implementation here */
    }
}



RNWF_RESULT_t APP_MQTT_Publish(const char *top, const char *msg)
{    
    RNWF_MQTT_FRAME_t mqtt_pub;    
    mqtt_pub.isNew = NEW_MSG;
    mqtt_pub.qos = MQTT_QOS0;
    mqtt_pub.isRetain = NO_RETAIN;
    mqtt_pub.topic = top;
    mqtt_pub.message = msg;        
    return RNWF_MQTT_SrvCtrl(RNWF_MQTT_PUBLISH, (void *)&mqtt_pub);              
}     

RNWF_RESULT_t APP_MQTT_Callback(RNWF_MQTT_EVENT_t event, uint8_t *p_str)
{   
  
    switch(event)
    {
        case RNWF_MQTT_CONNECTED:
        {                        
#ifdef RNWF11_SERVICE
            RNWF_RESULT_t result = RNWF_FAIL; 
            printf("MQTT Connected\r\n");
            sprintf(app_buf, "%s", subscribe_list[subCnt++]);
#endif            

            g_isMqttConnected = MQTT_CONNECTED;                   
            }
        break;
        case RNWF_MQTT_SUBCRIBE_ACK:
        {
            CLOUD_SUBACK_HANDLER();           
        }
        break;
        case RNWF_MQTT_SUBCRIBE_MSG:
        {
//            printf("RNWF02 <- %s\r\n", p_str);
            CLOUD_SUBMSG_HANDLER(p_str);
        }
        break;
        case RNWF_MQTT_DISCONNECTED:
        {            
            printf("MQTT - Reconnecting...\r\n"); 
            g_isMqttConnected = MQTT_DISCONNECTED;          
            RNWF_MQTT_SrvCtrl(RNWF_MQTT_CONNECT, NULL);
            g_isMqttConnected = MQTT_CONNECTING;         
        }
        break;
        default:
        break;
    }
    return RNWF_PASS;
}   


void APP_WIFI_Callback(RNWF_WIFI_EVENT_t event, uint8_t *p_str)
{
    switch(event)
    {
        case RNWF_SNTP_UP:
        {            
            if(g_isMqttConnected < MQTT_CONNECTING)
            {            
                printf("SNTP UP:%s\n", &p_str[2]);             
                printf("Connecting to the Cloud\r\n");
                RNWF_MQTT_SrvCtrl(RNWF_MQTT_SET_CALLBACK, APP_MQTT_Callback);
                RNWF_MQTT_SrvCtrl(RNWF_MQTT_CONFIG, (void *)&mqtt_cfg);
                RNWF_MQTT_SrvCtrl(RNWF_MQTT_CONNECT, NULL);
                g_isMqttConnected = MQTT_CONNECTING;
            }
            break;
        }
        case RNWF_CONNECTED:
        {
            printf("Wi-Fi Connected\n");
#ifdef RNWF11_SERVICE            
            RNWF_MQTT_SrvCtrl(RNWF_MQTT_SUBSCRIBE_QOS0, app_buf);
#endif            
            break;
        }
        case RNWF_DISCONNECTED:
        {
            printf("Wi-Fi Disconnected\nReconnecting... \n");
            RNWF_WIFI_SrvCtrl(RNWF_STA_CONNECT, NULL);
            break;
        }
        case RNWF_DHCP_DONE:
        {
            printf("DHCP IP:%s\n", &p_str[2]); 

            break;       
        }
        case RNWF_SCAN_INDICATION:
        {
            break;
        }
        case RNWF_SCAN_DONE:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}


void RNWF_APP_Initialize(void)
{    
    const char sntp_url[] =  "0.in.pool.ntp.org";    
    RNWF_SYSTEM_SrvCtrl(RNWF_SYSTEM_SET_SNTP, sntp_url);           

 
    /* Wi-Fii Connectivity */
    RNWF_WIFI_PARAM_t wifi_sta_cfg = {RNWF_WIFI_MODE_STA, HOME_AP_SSID, HOME_AP_PASSPHRASE, HOME_AP_SECURITY, 1};    
    printf("Connecting to %s\r\n", HOME_AP_SSID);
    RNWF_WIFI_SrvCtrl(RNWF_WIFI_SET_CALLBACK, APP_WIFI_Callback);
    RNWF_WIFI_SrvCtrl(RNWF_SET_WIFI_PARAMS, &wifi_sta_cfg);

    /* RNWF Application Callback register */


    while(1)
    {  
        if(g_isMqttConnected == MQTT_CONNECTED)     
        {                
            CLOUD_STATE_MACHINE();                                                              
        }
        RNWF_EVENT_Handler();
    }    
}