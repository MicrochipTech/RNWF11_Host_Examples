/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    http.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../../timer/delay.h"

#include "rnwf_interface.h"
#include "rnwf_net_service.h"
#include "rnwf_wifi_service.h"
#include "rnwf_provision_service.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

RNWF_PROV_CALLBACK_t gProv_CallBack_Handler; 

/* TCP Socket */
RNWF_NET_SOCKET_t provision_socket = {
        .bind_type = RNWF_BIND_LOCAL,
        .sock_port = 80,
        .sock_type = RNWF_SOCK_TCP,
        };

#ifdef PROV_MOBILE_APP
/*Parse or translate security type received from mobile app*/
RNWF_WIFI_SECURITY_t RNWF_PROV_ParseAuth(uint8_t secType)
{
    RNWF_WIFI_SECURITY_t authType;
    
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
            
        //To-Do: 
        //Enterprise mode security type if applicable
            
        default:
            printf("Invalid security type\r\n");
            break;
    }
    return authType;
}
            
/* Parse Wi-Fi configuration file */
/* Format is APP_WIFI_PROV_WIFI_CONFIG_ID,<SSID>,<AUTH>,<PASSPHRASE>*/
RNWF_RESULT_t RNWF_PROV_APP_Parse(uint8_t *wifiCofnig, RNWF_WIFI_PARAM_t *wifi_config)
{
    char* p;    
    RNWF_RESULT_t ret = RNWF_PASS;
    
    p = strtok((char *)wifiCofnig, ",");
    if (p != NULL && !strncmp(p, APP_WIFI_PROV_CONFIG_ID, strlen(APP_WIFI_PROV_CONFIG_ID))) 
    {
        p = strtok(NULL, ",");
        if (p)
            wifi_config->ssid = p;

        p = strtok(NULL, ",");
        if (p) 
        {
            uint8_t security = (RNWF_WIFI_SECURITY_t)atoi(p);
            
            
            
            wifi_config->security = RNWF_PROV_ParseAuth(security);
                    
//            wifi_config->security = (RNWF_WIFI_SECURITY_t)atoi(p);
            
            if (RNWF_OPEN < wifi_config->security &&  wifi_config->security <= RNWF_WPA3)
            {
                p = strtok(NULL, ",");
                if (p) 
                    wifi_config->passphrase =  p;
                else
                    ret = RNWF_FAIL;
            } 
            else if (wifi_config->security == RNWF_OPEN)
                wifi_config->passphrase = NULL;
            else
                ret = RNWF_FAIL;
        }
        else
            ret = RNWF_FAIL;

        DBG_MSG_PROV("SSID:%s - PASSPHRASE:%s - AUTH:%d\r\n", 
                            wifi_config->ssid, 
                            wifi_config->passphrase, 
                            wifi_config->security);
    }
    else if(p != NULL && !strncmp(p, APP_WIFI_PROV_DONE_ID, strlen(APP_WIFI_PROV_DONE_ID)))
    {
        
        
    }
    return ret;
}


RNWF_RESULT_t RNWF_APP_PROV_Process(uint32_t socket, uint16_t rx_len) {
        
    RNWF_WIFI_PARAM_t wifiConfig;
    uint8_t prov_buf[PROV_BUF_LEN_MAX];
    
    if(RNWF_NET_TCP_SOCK_Read(socket, rx_len, (uint8_t *)prov_buf) > 0)
    {
        if(RNWF_PROV_APP_Parse(prov_buf, &wifiConfig) == RNWF_PASS)
        {
            RNWF_NET_SOCK_SrvCtrl(RNWF_NET_SOCK_CLOSE, &socket);
            wifiConfig.mode = RNWF_WIFI_MODE_STA;
            wifiConfig.autoconnect = 1;
            if(gProv_CallBack_Handler)
                gProv_CallBack_Handler(RNWF_PROV_COMPLTE, (uint8_t *)&wifiConfig);
            return RNWF_PASS;
        }
        else
        {
            if(gProv_CallBack_Handler)
                gProv_CallBack_Handler(RNWF_PROV_FAILURE, NULL);
        }
    }
    return RNWF_FAIL;
}
#else

uint32_t g_scan_socket = 0;

uint8_t http_buffer[HTTP_BUFFER_LEN];

void HTTP_CB_Index(uint32_t socket, uint8_t *input)
{
    HTTP_RESP_Send(socket, HTTP_RESPONSE_HDR, main_htm, "text/html", main_htm_len);
}

void HTTP_CB_Scan(uint32_t socket, uint8_t *input)
{
    printf("Trigger Scan\n");
    RNWF_WIFI_SrvCtrl(RNWF_WIFI_ACTIVE_SCAN, NULL);
    g_scan_socket = socket;
    http_buffer[HTTP_HDR_OFFSET] = '\0';
}


void HTTP_CB_Provision(uint32_t socket, uint8_t *input)
{
    if(input != NULL)
    {
        uint8_t *ssid = NULL, *passphrase = NULL, *security = NULL;
        printf("FORM Data = %s\n", input);
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


        HTTP_RESP_Send(socket, HTTP_RESPONSE_HDR, "Provision Success!", "text/html", strlen("Provision Success!"));

        DELAY_milliseconds(1000);

        RNWF_WIFI_PARAM_t wifi_sta_cfg = {RNWF_WIFI_MODE_STA, ssid, passphrase, *security-'0', 1};

        if(gProv_CallBack_Handler)
                gProv_CallBack_Handler(RNWF_PROV_COMPLTE, (uint8_t *)&wifi_sta_cfg);
        /* Close the HTTP server sockets and clean up the web server */
        /* Wi-Fii Connectivity */
    }
}

void HTTP_CB_Icon(uint32_t socket, uint8_t *input)
{
    HTTP_RESP_Send(socket, HTTP_RESPONSE_HDR, favicon_ico, "image/ico", favicon_ico_len);
}

void HTTP_CB_Logo(uint32_t socket, uint8_t *input)
{
    HTTP_RESP_Send(socket, HTTP_RESPONSE_HDR, mchp_gif, "image/gif", mchp_gif_len);
}

void HTTP_RESPONSE_Error(uint32_t socket, uint8_t *input)
{
    HTTP_RESP_Send(socket, HTTP_RESPONSE_NOT_FOUND, NULL, NULL, 0);
}

void RNWF_WEB_PROV_Process(uint32_t socket, uint16_t rx_len) {

    uint8_t *request = NULL, *end_str, *content = NULL, *tmpPtr;
    uint8_t page_idx = 0;
    volatile int result;
    uint16_t read_len = 0;
    static uint16_t offset = 0, content_len = 0, hdr_len;

    while(rx_len)
    {
        read_len = ((rx_len + offset) > HTTP_BUFFER_LEN)?(HTTP_BUFFER_LEN - offset):(rx_len-offset);
        if((result = RNWF_NET_TCP_SOCK_Read(socket, read_len, (uint8_t *)http_buffer+offset)) > 0)
        {            
            rx_len -= result;
            http_buffer[result+offset] = '\0';                                    
            //Calculate the Header Length
            hdr_len = ((uint32_t)(strstr(http_buffer, "\r\n\r\n") - ((uint32_t)http_buffer + offset)) + 4);
            if(!offset && (tmpPtr = (uint8_t *)strstr(http_buffer, HTTP_CONTENT_LEN)) != NULL)
            {                
                volatile char *token = strtok(tmpPtr, "\r\n");                                                
                content_len = strtol((token+sizeof(HTTP_CONTENT_LEN)), NULL, 10);
                offset = result;                
            }
            if(content_len && (content_len == result-hdr_len))
            {
                content = (uint8_t *)&http_buffer[hdr_len];
                hdr_len = 0;
                break;
            }
        }
    }    
    content_len = 0;
    offset = 0;
    request = (uint8_t *)strchr((char *)http_buffer, ' ') + 1;
    end_str = (uint8_t *)strchr((char *)request, ' ');

    if(request != NULL && (end_str != NULL))
    {
        http_buffer[end_str - http_buffer] = '\0';        
        while(g_web_pages[page_idx].url != NULL)
        {            
            if(strlen((char *)request) == strlen(g_web_pages[page_idx].url))
            {                
                for(int idx = 0; idx < strlen((char *)request); idx++)
                {
                    if(request[idx] != g_web_pages[page_idx].url[idx])
                    {                        
                        page_idx++;
                        continue;
                    }
                }
                if(g_web_pages[page_idx].handler != NULL)
                {                    
                    g_web_pages[page_idx].handler(socket, content);
                    return;
                }
            }
            page_idx++;
        }
        HTTP_RESPONSE_Error(socket, NULL);
    }
}


uint16_t HTTP_RESP_Send(uint32_t socket, const char * resp_hdr, const char *webpage, const char *page_type, uint16_t page_len) {

    uint16_t page_offset = 0;

    sprintf((char *)http_buffer, resp_hdr, page_type, page_len);
    RNWF_NET_TCP_SOCK_Write(socket, strlen((char *)http_buffer), (uint8_t *)http_buffer);
    while(page_len > 0)
    {
        if(page_len > HTTP_BUFFER_LEN)
        {
            memcpy(http_buffer, &webpage[page_offset], HTTP_BUFFER_LEN);
            RNWF_NET_TCP_SOCK_Write(socket, HTTP_BUFFER_LEN, (uint8_t *)http_buffer);
            page_offset += HTTP_BUFFER_LEN;
            page_len -= HTTP_BUFFER_LEN;
        }
        else
        {
            memcpy(http_buffer, &webpage[page_offset], page_len);
            RNWF_NET_TCP_SOCK_Write(socket, page_len, (uint8_t *)http_buffer);
            page_len = 0;
        }
    }

    return 0;
}
#endif

void RNWF_PROV_SOCKET_Callback(uint32_t sock, RNWF_NET_SOCK_EVENT_t event, uint8_t *p_str)
{
    switch(event)
    {
        case RNWF_NET_SOCK_EVENT_CONNECTED:
        {
            break;
        }
        case RNWF_NET_SOCK_EVENT_DISCONNECTED:
        {
            RNWF_NET_SOCK_SrvCtrl(RNWF_NET_SOCK_CLOSE, &sock);
            break;
        }
        case RNWF_NET_SOCK_EVENT_READ:
        {
#ifdef PROV_WEB_SERVER
        RNWF_WEB_PROV_Process(sock, *(uint16_t *)p_str);
#elif PROV_MOBILE_APP
        RNWF_APP_PROV_Process(sock, *(uint16_t *)p_str);
#endif
        }
        default:
            break;
    }
}

void RNWF_PROV_WIFI_Callback(RNWF_WIFI_EVENT_t event, uint8_t *p_str)
{            
    switch(event)
    {
        case RNWF_DHCP_DONE:
            printf("DHCP IP:%s\n", &p_str[2]);
            RNWF_NET_SOCK_SrvCtrl(RNWF_NET_SOCK_TCP_OPEN, &provision_socket);
            
            break;
        case RNWF_SCAN_INDICATION:
        {
#ifdef PROV_WEB_SERVER              
            uint16_t resp_len = strlen((char *)p_str);
            uint16_t buff_len = strlen((char *)&http_buffer[HTTP_HDR_OFFSET])+HTTP_HDR_OFFSET;
            // reuse the http buffer with a offset to hold header info 
            // sanity check for buffer over flow            
            if((buff_len + resp_len + 2) < HTTP_BUFFER_LEN)
            {
                strcat((char *)&http_buffer[HTTP_HDR_OFFSET], (char *)p_str);                
                buff_len += resp_len;                
                http_buffer[buff_len] = '|';
                http_buffer[buff_len+1] = '\0';            
            }
#endif            
            break;            
        }
        case RNWF_SCAN_DONE:            
#ifdef PROV_WEB_SERVER  
            if(g_scan_socket)             
            {
                HTTP_RESP_Send(g_scan_socket, HTTP_RESPONSE_HDR, (char *)&http_buffer[HTTP_HDR_OFFSET], "text/html", strlen((char *)&http_buffer[HTTP_HDR_OFFSET]));                
                g_scan_socket = 0;
            }
#endif
#ifdef PROV_MOBILE_APP               

#endif            
            
            break;
        default:
            break;
                    
    }    
}

RNWF_RESULT_t RNWF_PROV_SrvCtrl(RNWF_PROV_SERVICE_t request, void *input)  {
    
    switch(request)
    {
        case RNWF_PROV_ENABLE:
        {                   
            /* RNWF Application Callback register */
            RNWF_WIFI_SrvCtrl(RNWF_WIFI_SET_SRVC_CALLBACK, RNWF_PROV_WIFI_Callback);
            RNWF_NET_SOCK_SrvCtrl(RNWF_NET_SOCK_SET_SRVC_CALLBACK, RNWF_PROV_SOCKET_Callback);

            const char *dhcps_cfg[] = {"192.168.1.1/24", "192.168.1.10"};
            RNWF_NET_SOCK_SrvCtrl(RNWF_NET_DHCP_SERVER_ENABLE, dhcps_cfg);  

            /* Wi-Fii Connectivity */
            RNWF_WIFI_PARAM_t wifi_ap_cfg = {RNWF_WIFI_MODE_AP, SOFT_AP_SSID, "", SOFT_AP_SECURITY, 1};    
            RNWF_WIFI_SrvCtrl(RNWF_SET_WIFI_PARAMS, &wifi_ap_cfg);
        }
        break;
        
        case RNWF_PROV_DISABLE:
        {
            /* RNWF Application Callback register */
            RNWF_WIFI_SrvCtrl(RNWF_WIFI_SET_SRVC_CALLBACK, NULL);
            RNWF_NET_SOCK_SrvCtrl(RNWF_NET_SOCK_SET_SRVC_CALLBACK, NULL);
            RNWF_NET_SOCK_SrvCtrl(RNWF_NET_DHCP_SERVER_DISABLE, NULL);
        }
        break;
        
        case RNWF_PROV_SET_CALLBACK:
        {
            if(input != NULL)
                gProv_CallBack_Handler = (RNWF_PROV_CALLBACK_t)input;
        }    
        break;
            
        default:
            break;
    }
    
    return RNWF_PASS;
    
}



RNWF_RESULT_t RNWF_PROV_SrvInit(RNWF_PROV_MODE_t provMode)  
{
#ifdef PROV_MOBILE_APP
    if(provMode == RNWF_PROV_MOBILE_APP)
    {
    }
#endif
    return RNWF_PASS;
}
/* *****************************************************************************
 End of File
 */
