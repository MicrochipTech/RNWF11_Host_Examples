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

<#if sysWifiRNWF.SYS_RNWF_WIFI_PROV_METHOD == "PROV_WEB_SERVER">

static SYS_RNWF_WEB_PAGES_t g_webPages[] = {
    {SYS_RNWF_PROV_HttpCbIndex, "/"},
    {SYS_RNWF_PROV_HttpCbScan, "/scan"},            
    {SYS_RNWF_PROV_HttpCbProvision, "/provision"},  
    {SYS_RNWF_PROV_HttpCbLogo, "/mchp.gif"},    
    //{SYS_RNWF_PROV_HttpCbIcon, "/favicon.ico"},   
    {NULL, NULL}
};

/* Main html size */
static unsigned int g_mainHtmLen = 4689;

/* Fav Icon size */
static unsigned int g_faviconIcoLen = 1406;

/* MCHP Gif size */
static unsigned int g_mchpGifLen = 1263;

</#if>
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


<#if sysWifiRNWF.SYS_RNWF_WIFI_PROV_METHOD == "PROV_MOBILE_APP">

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
<#else>

/* Socket ID */
uint32_t g_scanSocket = 0;

/* Buffer to hold HTTP data */
char g_httpBuffer[SYS_RNWF_PROV_HTTP_BUFFER_LEN] = {'\0'};

/* HTTP Index */
void SYS_RNWF_PROV_HttpCbIndex(uint32_t socket, uint8_t *input)
{
    SYS_RNWF_PROV_HttpRespSend(socket, SYS_RNWF_PROV_HTTP_RESPONSE_HDR, g_mainHtm, "text/html", g_mainHtmLen);
}

/* HTTP Scan */
void SYS_RNWF_PROV_HttpCbScan(uint32_t socket, uint8_t *input)
{
    SYS_RNWF_PROV_DBG_MSG("Trigger Scan\r\n");
    SYS_RNWF_WIFI_SrvCtrl(SYS_RNWF_WIFI_ACTIVE_SCAN, NULL);
    g_scanSocket = socket;
    g_httpBuffer[SYS_RNWF_PROV_HTTP_HDR_OFFSET] = '\0';
}

/* HTTP Provision  */
void SYS_RNWF_PROV_HttpCbProvision(uint32_t socket, uint8_t *input)
{
    
    if(input != NULL)
    {
        uint8_t *ssid = NULL, *passphrase = NULL, *security = NULL;
        SYS_RNWF_PROV_DBG_MSG("FORM Data = %s\r\n", input);
        if((passphrase = (uint8_t *)strstr((char *)input, "pwd:")) != NULL)
        {
            *(passphrase-1) = '\0';
            passphrase += 4;
        }
        else
            return;

        if((security = (uint8_t *)strstr((char *)input, "security:")) != NULL)
        {
            *(security-1) = '\0';
            security += 9;
        }
        else
            return;

        if((ssid = (uint8_t *)strstr((char *)input, "ssid:")) != NULL)
        {
            ssid += 5;
        }
        else
            return;


        SYS_RNWF_PROV_HttpRespSend(socket, SYS_RNWF_PROV_HTTP_RESPONSE_HDR, "Provision Success!", "text/html", strlen("Provision Success!"));

        SYS_RNWF_WIFI_PARAM_t wifi_sta_cfg = {SYS_RNWF_WIFI_MODE_STA,(const char  *) ssid,(const char  *) passphrase, *security-'0', 1};

        if(g_provCallBackHandler)
                g_provCallBackHandler(SYS_RNWF_PROV_COMPLTE, (uint8_t *)&wifi_sta_cfg);
        /* Close the HTTP server sockets and clean up the web server */
        /* Wi-Fii Connectivity */
    }
}

/* HTTP Icon */
void SYS_RNWF_PROV_HttpCbIcon(uint32_t socket, uint8_t *input)
{
    SYS_RNWF_PROV_HttpRespSend(socket, SYS_RNWF_PROV_HTTP_RESPONSE_HDR, g_faviconIco, "image/ico", g_faviconIcoLen);
}

/* HTTP Logo */
void SYS_RNWF_PROV_HttpCbLogo(uint32_t socket, uint8_t *input)
{
    SYS_RNWF_PROV_HttpRespSend(socket, SYS_RNWF_PROV_HTTP_RESPONSE_HDR, g_mchpGif, "image/gif", g_mchpGifLen);
}

/* HTTP Response Error */
void SYS_RNWF_PROV_HttpResponseError(uint32_t socket, uint8_t *input)
{
    SYS_RNWF_PROV_HttpRespSend(socket, SYS_RNWF_PROV_HTTP_RESPONSE_NOT_FOUND, NULL, NULL, 0);
}

/* HTTP Web Process  */
void SYS_RNWF_PROV_WebProcess(uint32_t socket, uint16_t rx_len)
{
    uint8_t *request = NULL, *end_str, *content = NULL, *tmpPtr;
    uint8_t page_idx = 0;
    volatile int result;
    uint16_t read_len = 0;
    static uint16_t offset = 0, content_len = 0, hdr_len;

    while(rx_len)
    {
        read_len = ((rx_len + offset) > SYS_RNWF_PROV_HTTP_BUFFER_LEN)?(SYS_RNWF_PROV_HTTP_BUFFER_LEN - offset):(rx_len-offset);
        if((result = SYS_RNWF_NET_TcpSockRead(socket, read_len, (uint8_t *)g_httpBuffer+offset)) > 0)
        {    
            SYS_RNWF_PROV_DBG_MSG("\r\nrx_len =%d\r\n",rx_len);
            rx_len -= result;
            g_httpBuffer[result+offset] = '\0';                                    
            
            //Calculate the Header Length
            hdr_len = ((uint32_t)(strstr((const char  *)g_httpBuffer, "\r\n\r\n") - ((uint32_t)g_httpBuffer + offset)) + 4);
            
            if(!offset && (tmpPtr = (uint8_t *)strstr((const char  *)g_httpBuffer, SYS_RNWF_PROV_HTTP_CONTENT_LEN)) != NULL)
            {                
                volatile char *token = strtok((char  *)tmpPtr, "\r\n");                                                
                content_len = strtol((const char  *)(token+sizeof(SYS_RNWF_PROV_HTTP_CONTENT_LEN)), NULL, 10);
                offset = result;                
            }

            if(content_len && (content_len == result-hdr_len))
            {
                content = (uint8_t *)&g_httpBuffer[hdr_len];
                hdr_len = 0;
                break;
            }
        }
    }    

    content_len = 0;
    offset = 0;
    request = (uint8_t *)strchr((char *)g_httpBuffer, ' ') + 1;
    end_str = (uint8_t *)strchr((char *)request, ' ');

    if(request != NULL && (end_str != NULL))
    {
        g_httpBuffer[end_str - (uint8_t *)g_httpBuffer] = '\0';        
        while(g_webPages[page_idx].url != NULL)
        {            
            if(strlen((char *)request) == strlen(g_webPages[page_idx].url))
            {                
                for(int idx = 0; idx < strlen((char *)request); idx++)
                {
                    if(request[idx] != g_webPages[page_idx].url[idx])
                    {                        
                        page_idx++;
                        continue;
                    }
                }

                if(g_webPages[page_idx].handler != NULL)
                {                    
                    g_webPages[page_idx].handler(socket, content);
                    return;
                }
            }
            page_idx++;
        }
        SYS_RNWF_PROV_HttpResponseError(socket, NULL);
    }
}

/* HTTP Response Send */
uint16_t SYS_RNWF_PROV_HttpRespSend(uint32_t socket, const char * resp_hdr, const char *webpage, const char *page_type, uint16_t page_len) 
{
    uint16_t page_offset = 0;
    SYS_RNWF_PROV_DBG_MSG("\r\nSYS_RNWF_PROV_HttpRespSend resp_hdr=%s  page_type=%s page_len=%d size=%d\r\n",resp_hdr,page_type,page_len,sizeof(resp_hdr)+sizeof(page_type)+sizeof(page_len));
    sprintf((char *)g_httpBuffer, resp_hdr, page_type, page_len);
    SYS_RNWF_NET_TcpSockWrite(socket, strlen((char *)g_httpBuffer), (uint8_t *)g_httpBuffer);

    while(page_len > 0)
    {
        if(page_len > SYS_RNWF_PROV_HTTP_BUFFER_LEN)
        {
            memcpy(g_httpBuffer, &webpage[page_offset], SYS_RNWF_PROV_HTTP_BUFFER_LEN);
            SYS_RNWF_NET_TcpSockWrite(socket, SYS_RNWF_PROV_HTTP_BUFFER_LEN, (uint8_t *)g_httpBuffer);
            page_offset += SYS_RNWF_PROV_HTTP_BUFFER_LEN;
            page_len -= SYS_RNWF_PROV_HTTP_BUFFER_LEN;
        }
        else
        {
            memcpy(g_httpBuffer, &webpage[page_offset], page_len);
            SYS_RNWF_NET_TcpSockWrite(socket, page_len, (uint8_t *)g_httpBuffer);
            page_len = 0;
        }
    }

    return 0;
}
</#if>

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
<#if sysWifiRNWF.SYS_RNWF_WIFI_PROV_METHOD == "PROV_WEB_SERVER">
        SYS_RNWF_PROV_WebProcess(sock, *(uint16_t *)p_str);
<#elseif sysWifiRNWF.SYS_RNWF_WIFI_PROV_METHOD == "PROV_MOBILE_APP">
        SYS_RNWF_PROV_AppProcess(sock, *(uint16_t *)p_str);
</#if>
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
<#if sysWifiRNWF.SYS_RNWF_WIFI_PROV_METHOD == "PROV_WEB_SERVER">             
            uint16_t resp_len = strlen((char *)p_str);
            uint16_t buff_len = strlen((char *)&g_httpBuffer[SYS_RNWF_PROV_HTTP_HDR_OFFSET])+SYS_RNWF_PROV_HTTP_HDR_OFFSET;
            // reuse the http buffer with a offset to hold header info 
            // sanity check for buffer over flow            
            if((buff_len + resp_len + 2) < SYS_RNWF_PROV_HTTP_BUFFER_LEN)
            {
                strcat((char *)&g_httpBuffer[SYS_RNWF_PROV_HTTP_HDR_OFFSET], (char *)p_str);                
                buff_len += resp_len;                
                g_httpBuffer[buff_len] = '|';
                g_httpBuffer[buff_len+1] = '\0';            
            }
</#if>                      
        }
        break; 

        case SYS_RNWF_SCAN_DONE:  
        {          
<#if sysWifiRNWF.SYS_RNWF_WIFI_PROV_METHOD == "PROV_WEB_SERVER">
            if(g_scanSocket)             
            {
                SYS_RNWF_PROV_HttpRespSend(g_scanSocket, SYS_RNWF_PROV_HTTP_RESPONSE_HDR, (char *)&g_httpBuffer[SYS_RNWF_PROV_HTTP_HDR_OFFSET], "text/html", strlen((char *)&g_httpBuffer[SYS_RNWF_PROV_HTTP_HDR_OFFSET]));                
                g_scanSocket = 0;
            }
</#if>    
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
<#if sysWifiRNWF.SYS_RNWF_WIFI_PROV_METHOD == "PROV_MOBILE_APP">
    if(provMode == SYS_RNWF_PROV_MOBILE_APP)
    {
    }
</#if>
    return SYS_RNWF_PASS;
}
/* *****************************************************************************
 End of File
 */
