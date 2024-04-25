/*******************************************************************************
  RNWF Host Assisted Wifi Provision Service Implementation

  File Name:
    sys_rnwf_wifi_provision_service.c

  Summary:
    Source code for the RNWF Host Assisted Wifi Provision  Service implementation.

  Description:
    This file contains the source code for the RNWF Host Assisted Wifi Provision Service
    implementation.
 *******************************************************************************/

/*******************************************************************************
Copyright (C) 2020 released Microchip Technology Inc.  All rights reserved.

 
Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This section lists the other files that are included in this file.
 */
#include "system/inf/sys_rnwf_interface.h"
#include "system/net/sys_rnwf_net_service.h"
#include "system/wifi/sys_rnwf_wifi_service.h"
#include "system/wifiprov/sys_rnwf_provision_service.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/*Provision callback handler*/
SYS_RNWF_PROV_CALLBACK_t g_provCallBackHandler; 


/* TCP Socket Configurations */
SYS_RNWF_NET_SOCKET_t g_provisionSocket = {
    /*Socket bind type*/
    .bind_type = SYS_RNWF_NET_BIND_TYPE0,
    
    /*Socket port number */
    .sock_port = SYS_RNWF_NET_SOCK_PORT0,
    
    /*Socket type(TCP/UDP)*/
    .sock_type = SYS_RNWF_NET_SOCK_TYPE0,
};


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */



/*Parse or translate security type received from mobile app*/
static SYS_RNWF_WIFI_SECURITY_t SYS_RNWF_PROV_ParseAuth(uint8_t secType)
{
    SYS_RNWF_WIFI_SECURITY_t authType = SYS_RNWF_OPEN;
    
    switch(secType)
    {
        case 1:
            authType = secType - 1;
            break;

        case 2:
            authType = secType;
            break;

        case 4:
            authType = secType + 1;
            break;
            
        default:
            SYS_RNWF_PROV_DBG_MSG("Invalid security type\r\n");
            break;
    }
    return authType;
}
            
/* Parse Wi-Fi configuration file */
/* Format is APP_WIFI_PROV_WIFI_CONFIG_ID,<SSID>,<AUTH>,<PASSPHRASE>*/
static SYS_RNWF_RESULT_t SYS_RNWF_PROV_AppParse(uint8_t *wifiCofnig, SYS_RNWF_WIFI_PARAM_t *wifi_config)
{
    char* p;    
    SYS_RNWF_RESULT_t ret = SYS_RNWF_PASS;
    
    p = strtok((char *)wifiCofnig, ",");
    if (p != NULL && !strncmp(p, SYS_RNWF_APP_WIFI_PROV_CONFIG_ID, strlen(SYS_RNWF_APP_WIFI_PROV_CONFIG_ID))) 
    {
        p = strtok(NULL, ",");
        if (p)
            wifi_config->ssid = p;

        p = strtok(NULL, ",");
        if (p) 
        {
            uint8_t security = (SYS_RNWF_WIFI_SECURITY_t)atoi(p);
             
            wifi_config->security = SYS_RNWF_PROV_ParseAuth(security);
            
            if (SYS_RNWF_OPEN < wifi_config->security &&  wifi_config->security <= SYS_RNWF_WPA3)
            {
                p = strtok(NULL, ",");
                if (p) 
                    wifi_config->passphrase =  p;
                else
                    ret = SYS_RNWF_FAIL;
            } 
            else if (wifi_config->security == SYS_RNWF_OPEN)
                wifi_config->passphrase = NULL;
            else
                ret = SYS_RNWF_FAIL;
        }
        else
            ret = SYS_RNWF_FAIL;

        SYS_RNWF_PROV_DBG_MSG("SSID:%s - PASSPHRASE:%s - AUTH:%d\r\n", 
                            wifi_config->ssid, 
                            wifi_config->passphrase, 
                            wifi_config->security);
    }
    else if(p != NULL && !strncmp(p, SYS_RNWF_APP_WIFI_PROV_DONE_ID, strlen(SYS_RNWF_APP_WIFI_PROV_DONE_ID)))
    {
        
    }
    return ret;
}

/*This function processes the data received from the provision app*/
static SYS_RNWF_RESULT_t SYS_RNWF_PROV_AppProcess(uint32_t socket, uint16_t rx_len) {
        
    SYS_RNWF_WIFI_PARAM_t wifiConfig;
    uint8_t prov_buf[SYS_RNWF_PROV_BUF_LEN_MAX];
    
    if(SYS_RNWF_NET_TcpSockRead(socket, rx_len, (uint8_t *)prov_buf) > 0)
    {
        if(SYS_RNWF_PROV_AppParse(prov_buf, &wifiConfig) == SYS_RNWF_PASS)
        {
            SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_CLOSE, &socket);
            wifiConfig.mode = SYS_RNWF_WIFI_MODE_STA;
            wifiConfig.autoconnect = 1;
            if(g_provCallBackHandler)
                g_provCallBackHandler(SYS_RNWF_PROV_COMPLTE, (uint8_t *)&wifiConfig);
            return SYS_RNWF_PASS;
        }
        else
        {
            if(g_provCallBackHandler)
                g_provCallBackHandler(SYS_RNWF_PROV_FAILURE, NULL);
        }
    }
    return SYS_RNWF_FAIL;
}

/*Provision Socket callback function*/
static void SYS_RNWF_PROV_SocketCallback(uint32_t sock, SYS_RNWF_NET_SOCK_EVENT_t event, uint8_t *p_str)
{
    switch(event)
    {
        case SYS_RNWF_NET_SOCK_EVENT_CONNECTED:
        {
            break;
        }

        case SYS_RNWF_NET_SOCK_EVENT_DISCONNECTED:
        {
            SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_CLOSE, &sock);
            break;
        }

        case SYS_RNWF_NET_SOCK_EVENT_READ:
        {
            SYS_RNWF_PROV_DBG_MSG("SYS_RNWF_NET_SOCK_EVENT_READ\r\n");
        SYS_RNWF_PROV_AppProcess(sock, *(uint16_t *)p_str);
        }

        default:
        {
            break;
        }
    }
}

/*Provision Wi-Fi Callback function*/
static void SYS_RNWF_PROV_WifiCallback(SYS_RNWF_WIFI_EVENT_t event, uint8_t *p_str)
{            
    switch(event)
    {
        case SYS_RNWF_DHCP_DONE:
        {
            SYS_RNWF_PROV_DBG_MSG("DHCP IP:%s\r\n", &p_str[2]);
            SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_TCP_OPEN, &g_provisionSocket);
        }    
        break;

        case SYS_RNWF_SCAN_INDICATION:
        {
        }
        break; 

        case SYS_RNWF_SCAN_DONE:  
        {          
        }     
        break;

        default:
        {
            break;
        }
                    
    }    
}

/*Provision Service control function*/
SYS_RNWF_RESULT_t SYS_RNWF_PROV_SrvCtrl(SYS_RNWF_PROV_SERVICE_t request, void *input)  {
    
    switch(request)
    {
        case SYS_RNWF_PROV_ENABLE:
        {                   
            /* RNWF Application Callback register */
            SYS_RNWF_WIFI_SrvCtrl(SYS_RNWF_WIFI_SET_SRVC_CALLBACK, SYS_RNWF_PROV_WifiCallback);
            SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_SET_SRVC_CALLBACK, SYS_RNWF_PROV_SocketCallback);

            const char *dhcps_cfg[] = {"192.168.1.1/24", "192.168.1.10"};
            SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_DHCP_SERVER_ENABLE, dhcps_cfg);  

            /* Wi-Fii Connectivity */
            SYS_RNWF_WIFI_PARAM_t wifi_ap_cfg = {SYS_RNWF_WIFI_MODE_AP, SYS_RNWF_WIFI_PROV_SSID, SYS_RNWF_WIFI_PROV_PWD, SYS_RNWF_PROV_SECURITY,SYS_RNWF_WIFI_PROV_AUTOCONNECT};    
            SYS_RNWF_WIFI_SrvCtrl(SYS_RNWF_SET_WIFI_PARAMS, &wifi_ap_cfg);
            SYS_RNWF_PROV_DBG_MSG("Provision mode enabled\r\n");
            SYS_RNWF_PROV_DBG_MSG("SSID:%s - PASSPHRASE:%s - AUTH:%d\r\n", 
                                wifi_ap_cfg.ssid, 
                                wifi_ap_cfg.passphrase,
                                wifi_ap_cfg.security);
        }
        break;
        
        case SYS_RNWF_PROV_DISABLE:
        {
            /* RNWF Application Callback register */
            SYS_RNWF_WIFI_SrvCtrl(SYS_RNWF_WIFI_SET_SRVC_CALLBACK, NULL);
            SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_SET_SRVC_CALLBACK, NULL);
            SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_DHCP_SERVER_DISABLE, NULL);
        }
        break;
        
        case SYS_RNWF_PROV_SET_CALLBACK:
        {
            if(input != NULL)
                g_provCallBackHandler = (SYS_RNWF_PROV_CALLBACK_t)input;
        }    
        break;
            
        default:
        {
            break;
        }
    }
    
    return SYS_RNWF_PASS;
}


/*Provision service init function*/
SYS_RNWF_RESULT_t SYS_RNWF_PROV_SrvInit(SYS_RNWF_PROV_MODE_t provMode)  
{
    if(provMode == SYS_RNWF_PROV_MOBILE_APP)
    {
    }
    return SYS_RNWF_PASS;
}
/* *****************************************************************************
 End of File
 */
