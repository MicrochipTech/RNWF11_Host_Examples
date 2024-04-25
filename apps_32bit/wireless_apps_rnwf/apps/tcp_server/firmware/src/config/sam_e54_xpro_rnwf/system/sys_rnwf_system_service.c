/*******************************************************************************
  RNWF Host Assisted System Service Implementation

  File Name:
    sys_rnwf_system_service.c

  Summary:
    Source code for the RNWF Host Assisted System Service implementation.

  Description:
    This file contains the source code for the RNWF Host Assisted System Service
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
#include <string.h>

/* This section lists the other files that are included in this file.
 */
#include "system/inf/sys_rnwf_interface.h"
#include "system/sys_rnwf_system_service.h"
#include "system/console/sys_console.h"

#ifdef RNWF11_SERVICE
#include "peripheral/port/plib_port.h"
#include "peripheral/dmac/plib_dmac.h"
#include "peripheral/sercom/usart/plib_sercom0_usart.h"
#include "system/time/sys_time.h"
#endif

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

void SYS_RNWF_SYS_DelayMs(uint32_t ms)
{
    SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;

    if (SYS_TIME_DelayMS(ms, &timer) != SYS_TIME_SUCCESS)
    {
        DBG_MSG_SYSTEM("SYS_TIME error");
    }
    else if(SYS_TIME_DelayIsComplete(timer) != true)
    {
        while (SYS_TIME_DelayIsComplete(timer) == false){};
    } else {}
}

static void Module_Hard_Reset(void)
{
    const char *MCLR_RESET = "101";

    DBG_MSG_SYSTEM("* Resetting NC * \r\n");

    MCLR_OutputEnable();
    for (uint8_t i=0; i<(uint8_t)strlen(MCLR_RESET); i++)
    {
        if((MCLR_RESET[i] - '0') == '1')
            MCLR_Set();
        else
            MCLR_Clear();

        SYS_RNWF_SYS_DelayMs(TP_DELAY_USEC);
    }
    MCLR_InputEnable();
}
#endif

/* RNWF System Service Conttrol Function */
SYS_RNWF_RESULT_t SYS_RNWF_SYSTEM_SrvCtrl(SYS_RNWF_SYSTEM_SERVICE_t request, void *input)
{
    SYS_RNWF_RESULT_t result = SYS_RNWF_FAIL;
    
    switch(request)
    {
        /* RNWF Reset */
        case SYS_RNWF_SYSTEM_RESET:
        {
            uint8_t init_msg[128];
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, init_msg, SYS_RNWF_RESET_TARGET);
            break;
        }
        
#ifdef RNWF11_SERVICE
        case SYS_RNWF_SYSTEM_TBL:
        {
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, SYS_RNWF_TRIGGER_BOOTLOADER);
        }
#endif        
        /* RNWF Echo Off  */
        case SYS_RNWF_SYSTEM_ECHO_OFF:
        {
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, SYS_RNWF_SET_ECHO_OFF);
            break;
        }
        
        /* RNWF Manufacturer ID */
        case SYS_RNWF_SYSTEM_GET_MAN_ID:
        {
            *(uint8_t*)input = '\0';
            result = SYS_RNWF_CMD_SEND_OK_WAIT("+GMI:", input, SYS_RNWF_GET_MAN_ID);
            break;
        }
        
        /* RNWF Software Revision */
        case SYS_RNWF_SYSTEM_SW_REV:
        {
            *(uint8_t*)input = '\0';
            result = SYS_RNWF_CMD_SEND_OK_WAIT("+GMR:", input, SYS_RNWF_GET_REV_ID);
            break;
        }
        
        /* RNWF Device Info  */
        case SYS_RNWF_SYSTEM_DEV_INFO:
        {
            *(uint8_t*)input = '\0';
            result = SYS_RNWF_CMD_SEND_OK_WAIT("+DI:", input, SYS_RNWF_GET_DEV_INFO);
            break;
        }
        
        /* RNWF Get Certificates List */
        case SYS_RNWF_SYSTEM_GET_CERT_LIST:
        {
            *(uint8_t*)input = '\0';
            result = SYS_RNWF_CMD_SEND_OK_WAIT("+FS:2,1,", input, SYS_RNWF_GET_CERT_LIST);
            break;
        }
        
        /* RNWF Get Key List*/
        case SYS_RNWF_SYSTEM_GET_KEY_LIST:
        {
            *(uint8_t*)input = '\0';
            result = SYS_RNWF_CMD_SEND_OK_WAIT("+FS:2,2,", input, SYS_RNWF_GET_KEY_LIST);
            break;
        }
        
        /* RNWF Get Time */
        case SYS_RNWF_SYSTEM_GET_TIME:
        {
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, SYS_RNWF_GET_SYS_TIME);
            break;
        }
        
        /* RNWF Get Time Unix*/
        case SYS_RNWF_SYSTEM_SET_TIME_UNIX:
        {
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, SYS_RNWF_SET_SYS_TIME_UNIX, *(uint32_t*)input);
            break;
        }
        
        /* RNWF Set time ntp */
        case SYS_RNWF_SYSTEM_SET_TIME_NTP:
        {
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, SYS_RNWF_SET_SYS_TIME_NTP, *(uint32_t*)input);
            break;
        }
        
        /* RNWF Set time string */
        case SYS_RNWF_SYSTEM_SET_TIME_STRING:
        {
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, SYS_RNWF_SET_SYS_TIME_STRING, (uint8_t*)input);
            break;
        }
        
        /* RNWF Get Wifi Info */
        case SYS_RWWF_SYSTEM_GET_WIFI_INFO:
        {
            *(uint8_t*)input = '\0';
            result = SYS_RNWF_CMD_SEND_OK_WAIT("+NETIFC:", input, SYS_RNWF_GET_WIFI_IFNO);
            break;
        }
        
        /* RNWF Set SNTP */
        case SYS_RNWF_SYSTEM_SET_SNTP:
        {            
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, SYS_RNWF_SET_SNTP_STATIC, input);
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, SYS_RNWF_SET_SNTP_URL, input);
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, SYS_RNWF_SET_SNTP_ENBL, input);
            break;
        }
        
        /* RNWF GET Mqtt info */
        case SYS_RNWF_SYSTEM_GET_MQTT_INFO:
        {            
            *(uint8_t*)input = '\0';
            result = SYS_RNWF_CMD_SEND_OK_WAIT("+MQTTC:", input, SYS_RNWF_GET_MQTT_IFNO);
            break;
        }
        
        default:
        {
            break;
        }
    }
    
    return result;
}
#ifdef RNWF11_SERVICE

SYS_RNWF_RESULT_t SYS_RNWF_SYSTEM_Module_Init(SYS_RNWF_SYSTEM_OPERATION_MODE_t mode)
{
    int num_retries = 10;
    uint8_t send_buf = BL_CMD_ENTER_APP;
    uint8_t resp_buf = 0;
    uint8_t response[100];

    if ( mode == SYS_RNWF_SYSTEM_BTL_MODE )
        send_buf = BL_CMD_ENTER_BTL;
    else
        send_buf = BL_CMD_ENTER_APP;
        
    Module_Hard_Reset();

    SYS_RNWF_SYS_DelayMs(500);

    while (num_retries)
    {
        if(true == DMAC_ChannelTransfer(DMAC_CHANNEL_0, &send_buf, \
                (const void * ) & (SERCOM0_REGS -> USART_INT.SERCOM_DATA), \
                1))
        {
            while (!SERCOM0_USART_TransmitComplete());
        }

        SYS_RNWF_SYS_DelayMs(200);

        if(SERCOM0_USART_Read(&resp_buf, 1) == 1)
        {
            if ( mode == SYS_RNWF_SYSTEM_BTL_MODE )
            {
                if (resp_buf == BL_RESP_OK)
                {
                    DBG_MSG_SYSTEM(" success entering BTL..\r\n");
                    break;
                }
            }
            else
            {
                if (resp_buf == BL_RESP_OK)
                {
                    DBG_MSG_SYSTEM(" success entering APP..\r\n");
                    break;
                }
            }
            
        }
            
        num_retries --;
    }

    if (num_retries == 0)
    {
        DBG_MSG_SYSTEM("[%s] timeout..\r\n",__func__);
        return SYS_RNWF_TIMEOUT;
    }
    
    if ( mode == SYS_RNWF_SYSTEM_APP_MODE )
    {
        num_retries = 10;
        while (num_retries)
        {
            SYS_RNWF_IF_CmdRspSend(NULL, NULL, response, NULL, NULL);
            if (strstr((char *)response, MODULE_START_MESSAGE) != 0)
                break;
            num_retries --;
        }

        if (num_retries == 0)
        {
            DBG_MSG_SYSTEM("[%s] timeout..\r\n",__func__);
            return SYS_RNWF_TIMEOUT;
        }
    }
    DBG_MSG_SYSTEM(" SYS_RNWF_SYSTEM_Module_Init log3..\r\n");
    return SYS_RNWF_PASS;
}

#endif


/* *****************************************************************************
 End of File
 */
