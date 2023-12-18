/*
 * MAIN Generated Driver File
 * 
 * @file rnwf_system_service.c
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

/*! \mainpage RNWF11 Service API User Guide

The RNWF11 AT Command service layer wraps the ASCII
command implementation in a service architecture. The
service layer API's are documented here can enable 
easy and quick applciation development.

- \ref SERVICE_GRP "System Service"
- \ref WIFI_GRP "Wi-Fi Service"
- \ref WIFI_PROV_GRP "Wi-Fi Provisioning Service"
- \ref MQTT_GRP "MQTT Service"
- \ref NETSOCK_GRP "Network Socket Service"
- \ref OTA_GRP "OTA Service"
*/

/* This section lists the other files that are included in this file.
 */
#include <stdio.h>
#include <string.h>

#include "rnwf_interface.h"
#include "rnwf_system_service.h"
#ifdef RNWF11_SERVICE
#include "../system/pins.h"
#endif

/* TODO:  Include other files here if needed. */


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
#ifdef RNWF11_SERVICE
#define MODULE_START_MESSAGE    "AT Command Interface"
#define BL_RESP_OK          0x50
#define BL_CMD_ENTER_BTL    0xa7
#define BL_CMD_ENTER_APP    0xaa

#define TP_DELAY_USEC             100
#endif

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
#ifdef RNWF11_SERVICE
void Module_Hard_Reset(void)
{
    const char *MCLR_RESET = "101";

    DBG_MSG_SYSTEM("* Resetting NC * \r\n");

    MCLR_SetDigitalOutput();
    for (uint8_t i=0; i<(uint8_t)strlen(MCLR_RESET); i++)
    {
        if((MCLR_RESET[i] - '0') == '1')
            MCLR_SetHigh();
        else
            MCLR_SetLow();

        DELAY_microseconds(TP_DELAY_USEC);
    }
    MCLR_SetDigitalInput();
}
#endif

RNWF_RESULT_t RNWF_SYSTEM_SrvCtrl(RNWF_SYSTEM_SERVICE_t request, void *input) {
    RNWF_RESULT_t result = RNWF_FAIL;
    switch(request)
    {
        case RNWF_SYSTEM_RESET:
        {
            uint8_t init_msg[128];
            result = RNWF_CMD_SEND_OK_WAIT(NULL, init_msg, RNWF_RESET_TARGET);
        }
        break;
#ifdef RNWF11_SERVICE
        case RNWF_SYSTEM_TBL:
        {
            result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_TRIGGER_BOOTLOADER);
        }
#endif        
        case RNWF_SYSTEM_ECHO_OFF:
        {
            result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_SET_ECHO_OFF);
        }
        break;case RNWF_SYSTEM_GET_MAN_ID:
        {
            *(uint8_t*)input = '\0';
            result = RNWF_CMD_SEND_OK_WAIT("+GMI:", input, RNWF_GET_MAN_ID);
        }
        break;
        case RNWF_SYSTEM_SW_REV:
        {
            *(uint8_t*)input = '\0';
            result = RNWF_CMD_SEND_OK_WAIT("+GMR:", input, RNWF_GET_REV_ID);
        }
        break;
        case RNWF_SYSTEM_DEV_INFO:
        {
            *(uint8_t*)input = '\0';
            result = RNWF_CMD_SEND_OK_WAIT("+DI:", input, RNWF_GET_DEV_INFO);
        }
        break;
        case RNWF_SYSTEM_GET_CERT_LIST:
        {
            *(uint8_t*)input = '\0';
            result = RNWF_CMD_SEND_OK_WAIT("+FS:2,1,", input, RNWF_GET_CERT_LIST);
        }
        break;
        case RNWF_SYSTEM_GET_KEY_LIST:
        {
            *(uint8_t*)input = '\0';
            result = RNWF_CMD_SEND_OK_WAIT("+FS:2,2,", input, RNWF_GET_KEY_LIST);
        }
        break;
        case RNWF_SYSTEM_GET_TIME:
        {
            result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_GET_SYS_TIME);
        }
        break;
        case RNWF_SYSTEM_SET_TIME_UNIX:
        {
            result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_SET_SYS_TIME_UNIX, *(uint32_t*)input);
        }
        break;
        case RNWF_SYSTEM_SET_TIME_NTP:
        {
            result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_SET_SYS_TIME_NTP, *(uint32_t*)input);
        }
        break;
        case RNWF_SYSTEM_SET_TIME_STRING:
        {
            result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_SET_SYS_TIME_STRING, (uint8_t*)input);
        }
        break;
        case RWWF_SYSTEM_GET_WIFI_INFO:
        {
            *(uint8_t*)input = '\0';
            result = RNWF_CMD_SEND_OK_WAIT("+NETIFC:", input, RNWF_GET_WIFI_IFNO);
        }
        break;
        case RNWF_SYSTEM_SET_SNTP:
        {            
            result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_SET_SNTP_STATIC, input);
            result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_SET_SNTP_URL, input);
            result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_SET_SNTP_ENBL, input);
        }
        break;
        case RNWF_SYSTEM_GET_MQTT_INFO:
        {            
            *(uint8_t*)input = '\0';
            result = RNWF_CMD_SEND_OK_WAIT("+MQTTC:", input, RNWF_GET_MQTT_IFNO);
        }
        break;
#ifdef RNWF11_SERVICE
        case RNWF_SYSTEM_GET_MQTT_USERNAME:
        {
            *(uint8_t*)input = '\0';
            result = RNWF_CMD_SEND_OK_WAIT("+MQTTC:4", input, RNWF_GET_MQTT_IFNO);
        }
        break;
#endif
        default:
            break;
    }
    return result;
}

#ifdef RNWF11_SERVICE
RNWF_RESULT_t RNWF_SYSTEM_Module_Init(void)
{
    int num_retries = 10;
    uint8_t send_buf = BL_CMD_ENTER_APP;
    uint8_t resp_buf = 0;
    uint8_t response[100];

    Module_Hard_Reset();

    DELAY_milliseconds(500);

    while (num_retries)
    {
        /* send BL_CMD_ENTER_APP command */
        if(UART2.IsTxReady())
        {
            UART2.Write(send_buf);
            while(!UART2.IsTxDone());
        }

        DELAY_milliseconds(200);

        /* check response */
        if(UART2.IsRxReady())
        {
            resp_buf = UART2.Read();

            if (resp_buf == BL_RESP_OK)
            {
                DBG_MSG_SYSTEM(" success entering app..\r\n");
                break;
            }
        }

        num_retries --;
    }

    if (num_retries == 0)
    {
        DBG_MSG_SYSTEM("[%s] timeout..\r\n",__func__);
        return RNWF_TIMEOUT;
    }

    num_retries = 10;
    while (num_retries)
    {
        RNWF_CMD_RSP_Send(NULL, NULL, response, NULL, NULL);
        if (strstr(response, MODULE_START_MESSAGE) != 0)
            break;
        num_retries --;
    }

    if (num_retries == 0)
    {
        DBG_MSG_SYSTEM("[%s] timeout..\r\n",__func__);
        return RNWF_TIMEOUT;
    }

    return RNWF_PASS;
}

#endif
/* *****************************************************************************
 End of File
 */
