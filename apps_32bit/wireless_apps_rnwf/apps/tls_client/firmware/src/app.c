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
#include "system/wifi/sys_rnwf_wifi_service.h"
#include "system/inf/sys_rnwf_interface.h"
#include "system/net/sys_rnwf_net_service.h"
#include "system/sys_rnwf_system_service.h"

/* Variable to check the UART transfer */
static volatile bool isUARTTxComplete = true,isUART0TxComplete = true;;

/*TCP buffer maximum length*/
#define TCP_BUF_LEN_MAX     1500
#define HTTP_CONTENT_LEN    "Content-Length:"

/*Stores TCP data*/
uint8_t g_tcpData[TCP_BUF_LEN_MAX];

/*variable to store the downloading file length*/
static uint32_t g_fileLen = 0;

/*Application buffer to store data*/
static uint8_t g_appBuf[SYS_RNWF_IF_LEN_MAX];

/*Shows the he application's current state*/
static APP_DATA g_appData; 

/*TLS client configurations*/
static SYS_RNWF_NET_SOCKET_t g_tcpClientSock = {
        .bind_type = SYS_RNWF_NET_BIND_TYPE0,
        .sock_port = SYS_RNWF_NET_SOCK_PORT0,
        .sock_type = SYS_RNWF_NET_SOCK_TYPE0,
        .sock_addr = SYS_RNWF_NET_SOCK_ADDR0,
        .tls_conf = SYS_RNWF_TLS_ENABLE0,
};

/*TLS Configurations*/
static const char *g_tlsCfg_1[] = {SYS_RNWF_NET_ROOT_CERT0, SYS_RNWF_NET_ROOT_CERT0, SYS_RNWF_NET_DEVICE_KEY0, SYS_RNWD_NET_DEVICE_KEY_PWD0, SYS_RNWF_NET_SERVER_NAME0,SYS_RNWF_NET_DOMAIN_NAME0};

/*AWS server file request configuration*/
static uint8_t g_awsFileRequest[] = "GET /ref_doc.pdf HTTP/1.1\r\nHost: file-download-files.s3-us-west-2.amazonaws.com\r\nConnection: close\r\n\r\n";


/* DMAC Channel Handler Function */
static void APP_RNWF_usartDmaChannelHandler(DMAC_TRANSFER_EVENT event, uintptr_t contextHandle)
{
    if (event == DMAC_TRANSFER_EVENT_COMPLETE)
    {
        isUARTTxComplete = true;
    }
}


/* Application Wi-fi Callback Handler function */
void SYS_RNWF_WIFI_CallbackHandler(SYS_RNWF_WIFI_EVENT_t event, uint8_t *p_str)
{
            
    switch(event)
    {
        /* SNTP UP event code*/
        case SYS_RNWF_SNTP_UP:
        {            
            SYS_CONSOLE_PRINT("SNTP UP:%s\r\n", &p_str[2]);  
            SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_TCP_OPEN, &g_tcpClientSock);
        }
        break;

        /* Wi-Fi connected event code*/
        case SYS_RNWF_CONNECTED:
        {
            SYS_CONSOLE_PRINT("Wi-Fi Connected    \r\n");
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
            static uint8_t sockOpen = 0;

            SYS_CONSOLE_PRINT("DHCP IP:%s\r\n", &p_str[2]);
            
            if(sockOpen == 0)
            {
                SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_TLS_CONFIG_1, g_tlsCfg_1);
                g_tcpClientSock.tls_conf = SYS_RNWF_NET_TLS_CONFIG_1;
//                SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_TCP_OPEN, &g_tcpClientSock);
                sockOpen = 1;
            }
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


/* Application NET socket Callback Handler function */
void SYS_RNWF_NET_SockCallbackHandler(uint32_t socket, SYS_RNWF_NET_SOCK_EVENT_t event, uint8_t *p_str)
{
    uint8_t *tmpPtr;
    static uint32_t rcvd_bytes;
    switch(event)
    {
        /* Net socket connected event code*/
        case SYS_RNWF_NET_SOCK_EVENT_CONNECTED:      
        {
            SYS_CONSOLE_PRINT("Connected to server!\r\n" );
            break;
        }
        
        /* Net socket TLS done event code*/
        case SYS_RNWF_NET_SOCK_EVENT_TLS_DONE:
        {
            SYS_RNWF_NET_TcpSockWrite(socket, strlen((char *)g_awsFileRequest), g_awsFileRequest);                            
            break;
        }   
        
        /* Net socket disconnected event code*/
        case SYS_RNWF_NET_SOCK_EVENT_DISCONNECTED:
        {
            SYS_CONSOLE_PRINT("DisConnected to server!\r\n");
            SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_CLOSE, &socket);
            break;
        }        
        
        /* Net socket read event code*/
        case SYS_RNWF_NET_SOCK_EVENT_READ:
        {                     
            int ret_val;
            uint16_t rx_len = *(uint16_t *)p_str;              
            while(rx_len)
            {
                uint16_t read_len = (rx_len > TCP_BUF_LEN_MAX)?TCP_BUF_LEN_MAX:rx_len;                
                if(((ret_val = SYS_RNWF_NET_TcpSockRead(socket, read_len, g_tcpData)) > 0))
                {                              
                    if(!g_fileLen)
                    {
                        SYS_CONSOLE_PRINT("%.*s\r\n", ret_val, g_tcpData);
                        if((tmpPtr = (uint8_t *)strstr((const char *)g_tcpData,(const char *) HTTP_CONTENT_LEN)) != NULL)
                        {
                            volatile char *token = strtok((char *)tmpPtr, "\r\n");
                            g_fileLen = strtol((const char *)(token+sizeof(HTTP_CONTENT_LEN)), NULL, 10);                                                        
                            SYS_CONSOLE_PRINT("File Size = %lu\r\n", g_fileLen);
                        }                                                
                        break;
                    }
                    rcvd_bytes += ret_val;
                    rx_len -= ret_val;
                    SYS_CONSOLE_PRINT("Received %lu bytes\r\n", rcvd_bytes);
                    if(rcvd_bytes >= g_fileLen)
                    {
                        SYS_CONSOLE_PRINT("Receive Complete!\r\n");  
                        SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_CLOSE, (void *)&socket);
                    }                    
                }            
                else
                {
                    SYS_CONSOLE_PRINT("Read Timeout!\r\n");
                    break;
                }
            }
            break; 
        }
        
        default:
            break;                  
    }    
    
}

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

/* Application initialization function*/
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

            DMAC_ChannelCallbackRegister(DMAC_CHANNEL_0, APP_RNWF_usartDmaChannelHandler, 0);
            SYS_RNWF_IF_Init();
            
            g_appData.state = APP_STATE_REGISTER_CALLBACK;
            SYS_CONSOLE_PRINT("##################################\r\n");
            SYS_CONSOLE_PRINT("  Welcome RNWF02 TLS Client Demo  \r\n");
            SYS_CONSOLE_PRINT("##################################\r\n");
            break;
        }
        
        /* Register the necessary callbacks */
        case APP_STATE_REGISTER_CALLBACK:
        {
            SYS_RNWF_SYSTEM_SrvCtrl(SYS_RNWF_SYSTEM_GET_MAN_ID, g_appBuf);    
            SYS_CONSOLE_PRINT("Manufacturer = %s\r\n", g_appBuf);            
    
            SYS_RNWF_SYSTEM_SrvCtrl(SYS_RNWF_SYSTEM_GET_CERT_LIST, g_appBuf);  
            SYS_CONSOLE_PRINT("Cert List:- \n%s\r\n", g_appBuf);

            char sntp_url[] =  "0.in.pool.ntp.org";    
            SYS_RNWF_SYSTEM_SrvCtrl(SYS_RNWF_SYSTEM_SET_SNTP, sntp_url); 

            /* RNWF Application Callback register */
            SYS_RNWF_WIFI_SrvCtrl(SYS_RNWF_WIFI_SET_CALLBACK, SYS_RNWF_WIFI_CallbackHandler);      
            SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_SET_CALLBACK, SYS_RNWF_NET_SockCallbackHandler);
          
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
            SYS_RNWF_IF_EventHandler();
            break;
        }
        default:
        {
            break;
        }
    }
}
