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

/*
    Main application
*/

#ifdef RNWF11_SERVICE
/* Keeps the device IP address */
static uint8_t g_DevIp[16];

/* Button Press Event */
bool    g_ButtonPress = false;
#endif

/* MQTT Connection States */
#define MQTT_DISCONNECTED       0
#define MQTT_CONNECTING         1
#define MQTT_CONNECTED          2

uint8_t g_isMqttConnected = MQTT_DISCONNECTED;
#ifdef RNWF11_SERVICE
uint8_t g_iothubUsernameCorrected = 0;

/* System Tick Counter for 1mSec*/
uint32_t g_SysTickCount;

static int16_t g_RebootDelay = -1;
static uint16_t g_ReportRate = APP_CLOUD_REPORT_INTERVAL;
#endif

/* Application buffer */
uint8_t app_buf[APP_BUFFER_SIZE_MAX];
/* MQTT Subscribe Topic Name List */
static const char *subscribe_list[] = {"$iothub/twin/PATCH/properties/desired/#", "$iothub/methods/POST/#", "$iothub/twin/res/#", NULL, NULL};
static uint8_t subCnt;
/* TLS Configuration details */
#ifdef RNWF11_SERVICE
const char *cloud_tls_cfg[] = {"DigiCertGlobalRootG2", NULL, NULL, 0, "*.azure-devices.net", 0};
#else
const char *cloud_tls_cfg[] = {"DigiCertGlobalRootG2", "rnwf02_device_01", "rnwf02_device_01", 0, 0, 0};
#endif
RNWF_MQTT_CFG_t mqtt_cfg = {
    .url = "g2-cert-dps.azure-devices-provisioning.net",
    .clientid = CLIENT_ID,
    .username = "0ne00B6AF97/registrations/sn012362DA8ABEC97F01/api-version=2019-03-31",
    .password = "",
    .port = 8883,
    .tls_conf = cloud_tls_cfg,
    .tls_idx = RNWF_NET_TLS_CONFIG_2,
    .azure_dps = 1,
#ifdef RNWF11_SERVICE
    .tls_use_ecc608 = RNWF_TLS_USE_ECC608
#endif
};

#ifdef RNWF11_SERVICE
uint8_t *APP_GET_IP_Address(void)
{
    return g_DevIp;
}

void APP_SYS_Tick(void)
{       
    g_SysTickCount++;     
}

void APP_SW_Handler(void)
{
    static uint32_t press_ticks;    
    if(!PB2_GetValue())
    {
        press_ticks = g_SysTickCount;
    }
    else
    {
        if((g_SysTickCount - press_ticks) > APP_SW_LONG_PRESS_DELAY)
        {
            APP_RESET_Device();
        }
        else
        {
            g_ButtonPress = 1;            
        }
    }    
}

void APP_AZURE_BUTTON_Telemetry(uint32_t press_count)
{            
    snprintf(app_buf, sizeof(app_buf), AZURE_FMT_BUTTON_TEL, press_count);
    printf("Telemetry ->> buttonEvent count %d\r\n", press_count);
    APP_MQTT_Publish(AZURE_PUB_TELEMETRY, app_buf);
}
#endif

void APP_AZURE_COUNTER_Telemetry(uint32_t counter)
{            
    snprintf(app_buf, sizeof(app_buf), AZURE_FMT_COUNTER_TEL, counter);
    printf("Telemetry ->> counter count %d\r\n", counter);
    APP_MQTT_Publish(AZURE_PUB_TELEMETRY, app_buf);
}

#ifdef RNWF11_SERVICE
void APP_LED_STATE_Handler(APP_LED_STATE_t ledState)
{
    printf("LED State = %d\r\n", ledState);
    switch(ledState)
    {
        case APP_LED_BLINK:
            TCA1_Interface.Start();
            break;
        case APP_LED_OFF:
            TCA1_Interface.Stop();
            LED_SetHigh();
            break;
        case APP_LED_ON:
            TCA1_Interface.Stop();
            LED_SetLow();
            break;
    }   
}
#endif

void APP_AZURE_SUBACK_Handler(void)
{    
    if(subscribe_list[subCnt] != NULL)
    {
#ifdef RNWF11_SERVICE
        switch (subCnt) {
            case 1:
            {
                sprintf(app_buf, "%s", subscribe_list[1]);
                ++subCnt;
            }
            break;
            case 2:
            {
                sprintf(app_buf, "%s", subscribe_list[2]);
                ++subCnt;
            }
            break;
            default:
                break;
        }
        RNWF_MQTT_SrvCtrl(RNWF_MQTT_SUBSCRIBE_QOS0, app_buf);
#else
		sprintf(app_buf, "%s", subscribe_list[subCnt++]);
		RNWF_MQTT_SrvCtrl(RNWF_MQTT_SUBSCRIBE_QOS0, app_buf);
#endif
    }
    else
    {        
        // get device twin
        APP_MQTT_Publish(AZURE_PUB_TWIN_GET, "");
    }
}

void APP_AZURE_Task(void)
{     
    static uint32_t counter = 0;  
    /* Implement app specific Azure_Task() method here */  
    /* set periodic events for telemetry */
#ifdef RNWF11_SERVICE
    static uint32_t press_count = 0;
    
    if(!(g_SysTickCount % APP_SYS_TICK_COUNT_1SEC))
    {            
        if(!(g_SysTickCount % g_ReportRate))
        {
            APP_AZURE_COUNTER_Telemetry(counter++);     //this will continuously trigger telemetry action
            APP_AZURE_BUTTON_Telemetry(press_count);
        }

        if(g_RebootDelay > 0)
        {            
            printf("Rebooting in %d seconds..\r\n", g_RebootDelay);
            g_RebootDelay--;
            if(g_RebootDelay == 0)
                APP_RESET_Device();            
        }                    
    }
        
    if(g_ButtonPress)
    {        
        APP_AZURE_BUTTON_Telemetry(++press_count);
        g_ButtonPress = 0;
    }

    if(!subCnt && subscribe_list[subCnt] != NULL)
    {
        sprintf(app_buf, "%s", subscribe_list[subCnt++]);
        RNWF_MQTT_SrvCtrl(RNWF_MQTT_SUBSCRIBE_QOS0, app_buf);            
    }
#else
    APP_AZURE_COUNTER_Telemetry(counter++);
#endif
}

void APP_AZURE_SUB_Handler(char *p_str)
{
    char *end_ptr = NULL;
    const char *ver_ptr = NULL, *led_ptr = NULL, *rate_ptr = NULL;
    if(strstr(p_str, "twin/res/200"))
    {
        /* Complete app-specific implementation here */
#ifdef RNWF11_SERVICE
        sprintf(app_buf, "{"AZURE_MSG_IPADDRESS"}", APP_GET_IP_Address());
        APP_MQTT_Publish(AZURE_PUB_PROPERTY, app_buf);
#endif
    }
    if(strstr(p_str, "POST"))
    {
        /* Complete app-specific implementation here */
#ifdef RNWF11_SERVICE
        char *reboot_ptr = (char *)strstr(p_str, AZURE_DEALY_TAG);
        char *echo_ptr = (char *)strstr(p_str, AZURE_ECHO_TAG);   
        char *rid_ptr = (char *)strstr(p_str, "rid="); 
        if(rid_ptr != NULL)
        {
            end_ptr = (char *)strstr(rid_ptr, "\" \"");
            *end_ptr = '\0';
            //+1 for null character
            uint16_t pubLen = sprintf(app_buf, AZURE_PUB_CMD_RESP, rid_ptr) + 1;             
            if(reboot_ptr != NULL)
            {                 
                end_ptr = (char *)strstr(reboot_ptr, "S\"}");
                *end_ptr = '\0';
                g_RebootDelay = atoi((char *)reboot_ptr+strlen(AZURE_DEALY_TAG));
                printf("Reboot delay = %d Sec\r\n", g_RebootDelay);
                sprintf(app_buf+pubLen, AZURE_FMT_DELAY_RSP, g_RebootDelay);                
                APP_MQTT_Publish(app_buf, app_buf+pubLen);
            }
            if(echo_ptr != NULL)
            {                 
                echo_ptr += strlen(AZURE_ECHO_TAG);
                end_ptr = (char *)strstr(echo_ptr, "\\\"}");
                *end_ptr = '\0';            
                printf("Echo = %s\r\n", echo_ptr);
                sprintf(app_buf+pubLen, AZURE_FMT_ECHO_RSP, echo_ptr);                
                APP_MQTT_Publish(app_buf, app_buf+pubLen);
            }
        }
#endif
    }
    if(strstr(p_str, "desired"))
    {
        /* Complete app-specific implementation here */
#ifdef RNWF11_SERVICE
        ver_ptr = strstr(p_str, AZURE_VERSION_TAG);                
        led_ptr = strstr(p_str, AZURE_LED0_TAG);  
        rate_ptr = strstr(p_str, AZURE_RATE_TAG);
        
        if(ver_ptr != NULL)
        {
            ver_ptr += +strlen(AZURE_VERSION_TAG); 
            end_ptr = (char *)strstr(ver_ptr, "}");
            *end_ptr = '\0';
            if(led_ptr != NULL)
            {
                led_ptr += strlen(AZURE_LED0_TAG);
                end_ptr = (char *)strstr(led_ptr, " \\");
                *end_ptr = '\0';
                sprintf(app_buf, AZURE_FMT_LED0_PROP);
                APP_LED_STATE_Handler(atoi(led_ptr));
                APP_MQTT_Publish(AZURE_PUB_PROPERTY, app_buf);
            }
            if(rate_ptr != NULL)
            {
                rate_ptr += strlen(AZURE_RATE_TAG);
                end_ptr = (char *)strstr(rate_ptr, " \\");
                *end_ptr = '\0';
                sprintf(app_buf, AZURE_FMT_RATE_PROP);
                g_ReportRate = atoi(rate_ptr) * APP_SYS_TICK_COUNT_1SEC;
                printf("Report Rate =  %d \r\n", g_ReportRate);
                APP_MQTT_Publish(AZURE_PUB_PROPERTY, app_buf);
            }
        }
#endif
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
            g_isMqttConnected = MQTT_CONNECTED;
#ifdef RNWF11_SERVICE
            printf("Azure Central Connection successful\r\n");
            TCA1_Interface.Stop();
            LED_SetLow();
#endif
        }
        break;
        case RNWF_MQTT_SUBCRIBE_ACK:
        {
            CLOUD_SUBACK_HANDLER();           
        }
        break;
        case RNWF_MQTT_SUBCRIBE_MSG:
        {
//            printf("RNWF11 <- %s\r\n", p_str);
            CLOUD_SUBMSG_HANDLER(p_str);
        }
        break;
        case RNWF_MQTT_DISCONNECTED:
        {
#ifdef RNWF11_SERVICE
            TCA1_Interface.Start();
#endif
            printf("MQTT - Reconnecting...\r\n"); 
            g_isMqttConnected = MQTT_DISCONNECTED;          
            RNWF_MQTT_SrvCtrl(RNWF_MQTT_CONNECT, NULL);
            g_isMqttConnected = MQTT_CONNECTING;         
        }
        break;
#ifndef RNWF11_SERVICE
        case RNWF_MQTT_DPS_STATUS:
        {
            if(*p_str == 1)
            {
                printf("DPS Successful! Connecting to Azure IoT Hub\r\n");
            }
            else
            {
                RNWF_MQTT_SrvCtrl(RNWF_MQTT_CONFIG, (void *)&mqtt_cfg);
            }
            RNWF_MQTT_SrvCtrl(RNWF_MQTT_CONNECT, NULL);
            g_isMqttConnected = MQTT_CONNECTING;
        }
        break;
#endif
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
#ifdef RNWF11_SERVICE
                TCA1_Interface.PeriodCountSet(APP_LED_TCA1_PER_TICK<<1);
#endif
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
            break;
        }
        case RNWF_DISCONNECTED:
        {
#ifdef RNWF11_SERVICE
            TCA1_Interface.PeriodCountSet(APP_LED_TCA1_PER_TICK);
            TCA1_Interface.Start();
#endif
            printf("Wi-Fi Disconnected\nReconnecting... \n");
            RNWF_WIFI_SrvCtrl(RNWF_STA_CONNECT, NULL);
            break;
        }
        case RNWF_DHCP_DONE:
        {
#ifdef RNWF11_SERVICE
            uint8_t flag=0;
            printf("DHCP IP:%s\n", &p_str[2]);
            for(int i=3; i<=strlen(&p_str[3]); i++)
            {
                if((p_str[i] >= 'A') && (p_str[i] <= 'Z'))
                {
                    flag = 1;
                    break;
                }
            }
            if(!flag)
                strncpy(g_DevIp, &p_str[3], strlen(&p_str[3])-1);
            break;
#else
            printf("DHCP IP:%s\n", &p_str[2]);

            break;
#endif
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
#ifdef RNWF11_SERVICE
        if((g_iothubUsernameCorrected == 0) && (g_isMqttConnected == MQTT_CONNECTED))
        {
            RNWF_RESULT_t result = RNWF_FAIL;
            RNWF_SYSTEM_SrvCtrl(RNWF_SYSTEM_GET_MQTT_USERNAME, app_buf);
            if((strstr((const char*)app_buf, "api-")) == NULL) {
                char * token = strtok((char *)app_buf, "\"");
                token = strtok(NULL, "\"");
                strcat(token, "/?api-version=2021-04-12");
                result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_MQTT_SET_USERNAME, token);
//                RNWF_MQTT_SrvCtrl(RNWF_MQTT_DISCONNECT, (void *)NULL);
                RNWF_MQTT_SrvCtrl(RNWF_MQTT_CONNECT, NULL);
                g_iothubUsernameCorrected = 1;
            }
            else
                g_iothubUsernameCorrected = 1;
        }
        else if(g_isMqttConnected == MQTT_CONNECTED)
        {
            CLOUD_STATE_MACHINE();
        }
#else
		if(g_isMqttConnected == MQTT_CONNECTED)
        {                
            CLOUD_STATE_MACHINE();                                                              
        }
#endif
        RNWF_EVENT_Handler();
    }    
}