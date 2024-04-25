/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h> 
#include <stdlib.h> 

/* This section lists the other files that are included in this file.*/
#include "app.h"
#include "user.h"
#include "definitions.h"
#include "configuration.h"
#include "system/debug/sys_debug.h"
#include "peripheral/port/plib_port.h"
#include "system/wifi/sys_rnwf_wifi_service.h"
#include "system/inf/sys_rnwf_interface.h"
#include "system/net/sys_rnwf_net_service.h"
#include "system/sys_rnwf_system_service.h"
#include "system/mqtt/sys_rnwf_mqtt_service.h"
#include "peripheral/eic/plib_eic.h"

/* MQTT Connection States */
#define MQTT_DISCONNECTED       0
#define MQTT_CONNECTING         1
#define MQTT_CONNECTED          2

uint8_t g_isMqttConnected = MQTT_DISCONNECTED;

#define APP_RNWF_MQTT_PUBLISH_INTERVAL (1000 * 5) //5 Sec

#define CLOUD_STATE_MACHINE()         APP_RNWF_AWS_Task()
#define CLOUD_SUBACK_HANDLER()        APP_RNWF_AWS_SubackHandler()
#define CLOUD_SUBMSG_HANDLER(msg)     SYS_RNWF_APP_AWS_SUB_Handler(msg)

/* Variable to check the UART transfer */
static volatile bool g_isUARTTxComplete = true,isUART0TxComplete = true;;
    
/*Shows the he application's current state*/
static APP_STATE_t g_appState = APP_SYS_INIT;

/* Keeps the device IP address */
static uint8_t g_devIp[64];

/*Shows the he application's current state*/
static APP_DATA1 g_appData;

/*Application buffer to store data*/
static uint8_t g_appBuf[SYS_RNWF_BUF_LEN_MAX];


/**TLS Configuration for the aws */
const char *g_tlsCfgAws[] = {SYS_RNWF_MQTT_SERVER_CERT, NULL, NULL, NULL, SYS_RNWF_MQTT_TLS_SERVER_NAME, NULL};
         

/*MQTT Configurations for aws*/
SYS_RNWF_MQTT_CFG_t mqtt_cfg = {
    .url = SYS_RNWF_MQTT_CLOUD_URL,
    .username = SYS_RNWF_MQTT_SCOPE_ID"/registrations/"SYS_RNWF_MQTT_CLIENT_ID"/api-version=2019-03-31",    
    .clientid = SYS_RNWF_MQTT_CLIENT_ID,    
    .password = SYS_RNWF_MQTT_PASSWORD,
    .port = SYS_RNWF_MQTT_CLOUD_PORT,
    .tls_conf = (uint8_t *) g_tlsCfgAws,
    .tls_idx = SYS_RNWF_NET_TLS_CONFIG_2,  
    .azure_dps = SYS_RNWF_MQTT_AZURE_DPS_ENABLE,
#ifdef RNWF11_SERVICE
    .tls_use_ecc608 = SYS_RNWF_TLS_USE_ECC608
#endif
};


/**Aws IoT HUB subscribe list */
static const char *g_subscribeList[] = {SYS_RNWF_MQTT_SUB_TOPIC_2, SYS_RNWF_MQTT_SUB_TOPIC_1, SYS_RNWF_MQTT_SUB_TOPIC_0,  NULL};

/* System Tick Counter for 1mSec*/
static uint32_t g_sysTickCount;

/*Aws subscribe count*/
static uint8_t g_subCnt;

/*MQTT data publish function*/
static SYS_RNWF_RESULT_t APP_RNWF_mqttPublish(const char *top, const char *msg)
{    
    SYS_RNWF_MQTT_FRAME_t mqtt_pub;    
    mqtt_pub.isNew = SYS_RNWF_NEW_MSG;
    mqtt_pub.qos = SYS_RNWF_MQTT_QOS0;
    mqtt_pub.isRetain = SYS_RNWF_NO_RETAIN;
    mqtt_pub.topic = top;
    mqtt_pub.message = msg;        
    return SYS_RNWF_MQTT_SrvCtrl(SYS_RNWF_MQTT_PUBLISH, (void *)&mqtt_pub);              
} 

/*Function to send the button press count data*/
static void APP_RNWF_AWS_Telemetry(uint8_t sensorInput1, uint8_t sensorInput2)
{            
    snprintf((char *)g_appBuf, sizeof(g_appBuf), SYS_RNWF_AWS_FMT_TEMP_LIGHT, sensorInput1, sensorInput2);
    SYS_CONSOLE_PRINT("Telemetry ->> sensorInput1 %d & sensorInput2 = %d\r\n", sensorInput1, sensorInput2);
    APP_RNWF_mqttPublish((const char *)SYS_RNWF_MQTT_TOPIC_NAME,(const char *) g_appBuf);
}


/*Aws subscribe acknowledgement handler*/
static void APP_RNWF_AWS_SubackHandler()
{    
#ifdef RNWF11_SERVICE
    static int led_value = 0;
    led_value++;
#endif    
    if(g_subscribeList[g_subCnt] != NULL)
    {
        sprintf((char *)g_appBuf, "%s", g_subscribeList[g_subCnt++]);
        SYS_RNWF_MQTT_SrvCtrl(SYS_RNWF_MQTT_SUBSCRIBE_QOS0, g_appBuf);            
    }    
    else
    {           
#ifdef RNWF11_SERVICE        
        sprintf((char *)g_appBuf, SYS_RNWF_AWS_FMT_SHADOW_LIGHT, (led_value%2)?1:0);
#else        
        sprintf((char *)g_appBuf, AWS_FMT_SHADOW_LIGHT, !LED_GetValue());
#endif        
        APP_RNWF_mqttPublish((const char *)SYS_RNWF_AWS_PUB_SHADOW_UPDATE, (const char *) g_appBuf);
    }
}

/*Function to handle Aws tasks*/
static void APP_RNWF_AWS_Task(void)
{
#ifdef RNWF11_SERVICE
    static uint8_t input1 = 0xff;
    static uint8_t input2 = 0;
    uint32_t totalTickCount = g_sysTickCount + SYS_TIME_MSToCount(APP_RNWF_MQTT_PUBLISH_INTERVAL);
    
    if(totalTickCount < SYS_TIME_CounterGet())
    {
        APP_RNWF_AWS_Telemetry(input1--, input2++);    //this will continuously trigger telemetry action
        g_sysTickCount = SYS_TIME_CounterGet();
    }
#else
    /* Implement app specific AWS_Task() method here */  
    /* set periodic events for telemetry */
    APP_RNWF_AWS_Telemetry(input1++, input2++);    //this will continuously trigger telemetry action
#endif    
}

/*Aws subscribe handler function*/
void SYS_RNWF_APP_AWS_SUB_Handler(char *p_str)
{
    if(strstr(p_str, "shadow/update/delta"))
    {        
        /* Complete app-specific implementation here */
    }
}


/* DMAC Channel Handler Function */
static void usartDmaChannelHandler(DMAC_TRANSFER_EVENT event, uintptr_t contextHandle)
{
    if (event == DMAC_TRANSFER_EVENT_COMPLETE)
    {
        g_isUARTTxComplete = true;
    }
}


/*function to get IP address*/
uint8_t *APP_GET_IP_Address(void)
{
    return g_devIp;
}


/* Application MQTT Callback Handler function */
SYS_RNWF_RESULT_t APP_MQTT_Callback(SYS_RNWF_MQTT_EVENT_t event, uint8_t *p_str)
{
    switch(event)
    {
        /* MQTT connected event code*/
        case SYS_RNWF_MQTT_CONNECTED:
        {    
#ifdef RNWF11_SERVICE
            sprintf((char *)g_appBuf, "%s", g_subscribeList[g_subCnt++]);
#endif            
            g_sysTickCount = SYS_TIME_CounterGet();
            
            SYS_CONSOLE_PRINT("MQTT : Connected\r\n");
            g_appState = APP_CLOUD_UP;
            g_isMqttConnected = MQTT_CONNECTED;
            break;
        }
        
        /* MQTT Subscribe acknowledge event code*/
        case SYS_RNWF_MQTT_SUBCRIBE_ACK:
        {
            CLOUD_SUBACK_HANDLER();
            break;
        }
        
        /* MQTT Subscribe message event code*/
        case SYS_RNWF_MQTT_SUBCRIBE_MSG:
        {
            CLOUD_SUBMSG_HANDLER((char *)p_str);
            break;
        }
        
        /*MQTT Disconnected event code*/
        case SYS_RNWF_MQTT_DISCONNECTED:
        {            
            SYS_CONSOLE_PRINT("MQTT - Reconnecting...\r\n");
            g_isMqttConnected = MQTT_DISCONNECTED;
            SYS_RNWF_MQTT_SrvCtrl(SYS_RNWF_MQTT_CONNECT, NULL); 
            g_isMqttConnected = MQTT_CONNECTING;
            break;
        }
               
        default:
        {
            break;
        }
    }
    return SYS_RNWF_PASS;
}

/* Application Wi-fi Callback Handler function */
void APP_WIFI_Callback(SYS_RNWF_WIFI_EVENT_t event, uint8_t *p_str)
{
            
    switch(event)
    {
        /* SNTP UP event code*/
        case SYS_RNWF_SNTP_UP:
        {            
            SYS_CONSOLE_PRINT("SNTP UP:%s\n", &p_str[2]);  
            if(g_isMqttConnected < MQTT_CONNECTING)
            {                        
                SYS_CONSOLE_PRINT("Connecting to the Cloud\r\n");
                SYS_RNWF_MQTT_SrvCtrl(SYS_RNWF_MQTT_SET_CALLBACK, APP_MQTT_Callback);
                SYS_RNWF_MQTT_SrvCtrl(SYS_RNWF_MQTT_CONFIG, (void *)&mqtt_cfg);
                SYS_RNWF_MQTT_SrvCtrl(SYS_RNWF_MQTT_CONNECT, NULL);
                g_isMqttConnected = MQTT_CONNECTING;
            }
                break;
        }
        
        /* Wi-Fi connected event code*/
        case SYS_RNWF_CONNECTED:
        {
            SYS_CONSOLE_PRINT("Wi-Fi Connected\r\n");
            break;
        }
        
        /* Wi-Fi disconnected event code*/
        case SYS_RNWF_DISCONNECTED:
        {
            SYS_CONSOLE_PRINT("Wi-Fi Disconnected\nReconnecting... \r\n");
            SYS_RNWF_WIFI_SrvCtrl(SYS_RNWF_STA_CONNECT, NULL);
            break;
        }
        
        /* Wi-Fi DHCP complete event code*/
        case SYS_RNWF_DHCP_DONE:
        {
            SYS_CONSOLE_PRINT("DHCP IP:%s\r\n", &p_str[2]); 
            strncpy((char *)g_devIp,(const char *) &p_str[3], strlen((const char *)(&p_str[3]))-1);
            break;
        }
        
        /* Wi-Fi scan indication event code*/
        case SYS_RNWF_SCAN_INDICATION:
        {
            break;
        }
        
        /* Wi-Fi scan complete event code*/
        case SYS_RNWF_SCAN_DONE:
        {
            break;
        }
        
        default:
        {
            break;
        }
                    
    }    
}

/*Application initialization function*/
void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    g_appData.state = APP_STATE_INITIALIZE;
}


/* Maintain the application's state machine.*/
void APP_Tasks ( void )
{
    switch(g_appData.state)
    {
        /* Application's state machine's initial state. */
        case APP_STATE_INITIALIZE:
        {
#ifdef RNWF11_SERVICE    
            if(SYS_RNWF_SYSTEM_Module_Init(SYS_RNWF_SYSTEM_APP_MODE) != SYS_RNWF_PASS)
                printf("RNWF_SYSTEM_Module_Init, fail..");
#endif

            DMAC_ChannelCallbackRegister(DMAC_CHANNEL_0, usartDmaChannelHandler, 0);
            SYS_RNWF_IF_Init();
            
            g_appData.state = APP_STATE_REGISTER_CALLBACK;
            break;
        }
        
        /* Register the necessary callbacks */
        case APP_STATE_REGISTER_CALLBACK:
        {    
            char sntp_url[] =  "0.in.pool.ntp.org";    
            SYS_RNWF_SYSTEM_SrvCtrl(SYS_RNWF_SYSTEM_SET_SNTP,sntp_url);

            SYS_RNWF_SYSTEM_SrvCtrl(SYS_RNWF_SYSTEM_GET_CERT_LIST, g_appBuf);  
            SYS_CONSOLE_PRINT("%s\r\n", g_appBuf);
                          
            /* RNWF Application Callback register */
            SYS_RNWF_WIFI_SrvCtrl(SYS_RNWF_WIFI_SET_CALLBACK, APP_WIFI_Callback);
          
            /* Wi-Fi Connectivity */
            SYS_RNWF_WIFI_PARAM_t wifi_sta_cfg = {SYS_RNWF_WIFI_MODE_STA, SYS_RNWF_WIFI_STA_SSID, SYS_RNWF_WIFI_STA_PWD, SYS_RNWF_STA_SECURITY, SYS_RNWF_WIFI_STA_AUTOCONNECT};        
            SYS_CONSOLE_PRINT("Connecting to %s\r\n", SYS_RNWF_WIFI_STA_SSID);
            SYS_RNWF_WIFI_SrvCtrl(SYS_RNWF_SET_WIFI_PARAMS, &wifi_sta_cfg);

            g_appData.state = APP_STATE_TASK;
            break;
        }
        
        /* Run Event handler */
        case APP_STATE_TASK:
        {
            if(g_isMqttConnected == MQTT_CONNECTED)
            {
                CLOUD_STATE_MACHINE();
            }
            SYS_RNWF_IF_EventHandler();
            break;
        }
        
        default:
        {
            break;
        }
    }    
}