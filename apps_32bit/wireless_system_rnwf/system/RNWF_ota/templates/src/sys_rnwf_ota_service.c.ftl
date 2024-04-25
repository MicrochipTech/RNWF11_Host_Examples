/*******************************************************************************
  RNWF Host Assisted OTA Service Implementation

  File Name:
    sys_rnwf_ota_service.c

  Summary:
    Source code for the RNWF Host Assisted OTA Service implementation.

  Description:
    This file contains the source code for the RNWF Host Assisted OTA Service
    implementation.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
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

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This section lists the other files that are included in this file.
 */
#include "app.h"
#include "configuration.h"
#include "system/time/sys_time.h"
#include "system/debug/sys_debug.h"
#include "peripheral/tc/plib_tc0.h"
#include "driver/sst26/drv_sst26.h"
#include "system/debug/sys_debug.h"
#include "peripheral/dmac/plib_dmac.h"
#include "system/inf/sys_rnwf_interface.h"
#include "system/sys_rnwf_system_service.h"
#include "system/net/sys_rnwf_net_service.h"
#include "system/ota/sys_rnwf_ota_service.h"
#include "system/time/sys_time_definitions.h"
#include "peripheral/sercom/usart/plib_sercom0_usart.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

/* Structure variable to hold flash data */
static SYS_RNWF_OTA_FLASH_DATA g_flashData;

/* Variable to hold Callback handler address */
static SYS_RNWF_OTA_CALLBACK_t g_otaCallBackHandler;

/* Ota buffer  */
static uint8_t *g_otaBuf = NULL;

/* Variable to hold image size  */
static uint32_t g_otaFileSize = 0;

/* Variable to hold DFU complete status */
static bool g_otaDfuComplete = false;

/* Variable to hold Download complete status*/
static bool g_otaDwldDone = false;

/* Buffer to hold Downloaded data */
static uint8_t g_otaBuffer[SYS_RNWF_OTA_BUF_LEN_MAX];

#ifdef RNWF11_SERVICE

//#define OTA_ERASE_BEFORE_WRITE
#define SST26_PAGE_SIZE     256    
static uint32_t g_crc_tab[256];
static uint32_t g_crc = 0xffffffff;

#endif

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

#ifdef RNWF11_SERVICE
SYS_RNWF_RESULT_t SYS_RNWF_OTA_CRC32_Init(uint32_t crc_tab[256])
{
    uint32_t   i, j, value;

    for (i = 0; i < 256; i++)
    {
        value = i;
        for (j = 0; j < 8; j++)
        {
            if (value & 1)
            {
                value = (value >> 1) ^ 0xEDB88320;
            }
            else
            {
                value >>= 1;
            }
        }
        crc_tab[i] = value;
    }

    return SYS_RNWF_PASS;
}

static SYS_RNWF_RESULT_t SYS_RNWF_OTA_CRC32_Generate(uint8_t* data_buf, uint32_t size, uint32_t* crc, uint32_t crc_tab[256])
{
    uint32_t   i;

    for (i = 0; i < size; i++)
    {
        *crc = crc_tab[(*crc ^ data_buf[i]) & 0xff] ^ (*crc >> 8);
    }

    return SYS_RNWF_PASS;
}
#endif


/* To set pin as Digital output pin */
static inline void SYS_RNWF_OTA_PinSetDigitalOutput(SYS_PORT_PIN pin)
{
    SYS_PORT_PinOutputEnable(pin);
}


/* To set pin as Digital Input pin */
static inline void SYS_RNWF_OTA_PinSetDigitalInput(SYS_PORT_PIN pin)
{
    SYS_PORT_PinInputEnable(pin);
}

/* To set pin HIGH */
static inline void SYS_RNWF_OTA_PinSetHigh(SYS_PORT_PIN pin)
{
    SYS_PORT_PinSet(pin);;
}


/* To set pin LOW */
static inline void SYS_RNWF_OTA_PinSetLow(SYS_PORT_PIN pin)
{
    SYS_PORT_PinClear(pin);;
}

/* To Initialize PINs as UART RX and Tx*/
static inline void SYS_RNWF_OTA_PortUartInitialize(void)
{
    /************************** GROUP 0 Initialization *************************/
    PORT_REGS->GROUP[0].PORT_PINCFG[4] = 0x3U;
    PORT_REGS->GROUP[0].PORT_PINCFG[5] = 0x3U;
    PORT_REGS->GROUP[0].PORT_PMUX[6]   = 0x22U;
}


/* Delay Milliseconds */
static inline void SYS_RNWF_OTA_DelayMs(uint32_t ms)
{
    SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;

    if (SYS_TIME_DelayMS(ms, &timer) != SYS_TIME_SUCCESS)
    {
        SYS_RNWF_OTA_DBG_MSG("SYS_TIME error");
    }
    else if(SYS_TIME_DelayIsComplete(timer) != true)
    {
        while (SYS_TIME_DelayIsComplete(timer) == false){};
    } else {}
}
 

/* Delay Microseconds */
static inline void SYS_RNWF_OTA_DelayUs(uint32_t us)
{
    SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;

    if (SYS_TIME_DelayUS(us, &timer) != SYS_TIME_SUCCESS)
    {
        SYS_RNWF_OTA_DBG_MSG("SYS_TIME error");
    }
    else if(SYS_TIME_DelayIsComplete(timer) != true)
    {
        while (SYS_TIME_DelayIsComplete(timer) == false){};
    } else {}
}


/* To Configure PINs as GPIO Digital pins*/
static inline void SYS_RNWF_OTA_ConfigureGpioPins ( void )
{
    /* MCLR pin*/
    PORT_PinGPIOConfig(SYS_RNWF_OTA_MCLR_PORT_PIN);
    SYS_RNWF_OTA_PinSetDigitalOutput(SYS_RNWF_OTA_MCLR_PIN);
    
    /* PGD data pin  */
    PORT_PinGPIOConfig(SYS_RNWF_OTA_PGD_PORT_PIN);
    SYS_RNWF_OTA_PinSetDigitalOutput(SYS_RNWF_OTA_PGD_PIN);
    
    /* PGC clock pin */
    PORT_PinGPIOConfig(SYS_RNWF_OTA_PGC_PORT_PIN);
    SYS_RNWF_OTA_PinSetDigitalOutput(SYS_RNWF_OTA_PGC_PIN); 
    
    return;
}


/* To Download data from server to SST26 Flash */
static SYS_RNWF_RESULT_t SYS_RNWF_OTA_DownloadProcess
(
    uint32_t socket,
    uint16_t rx_len
)
{
    int16_t read_size = 0;
    SYS_RNWF_OTA_CHUNK_t ota_chunk = {.chunk_ptr = g_otaBuf};
    static uint16_t result = 0;
    static volatile uint32_t total_rx = 0;
    char *tmpPtr = NULL;
    
    if((total_rx > 0) && (total_rx == g_otaFileSize))
    {
        return SYS_RNWF_PASS;
    }
    
    while(rx_len > 0)
    {
#ifndef RNWF11_SERVICE      /* RNWF02 */
        
        uint16_t readCnt = ((rx_len + result) > SYS_RNWF_OTA_BUF_LEN_MAX)?(SYS_RNWF_OTA_BUF_LEN_MAX-result):rx_len;  
        
        if((read_size = SYS_RNWF_NET_TcpSockRead(socket, readCnt, (uint8_t *)&g_otaBuf[result])) > 0 )
        {
            if(!g_otaFileSize)
            {
                tmpPtr = (char *)strstr((const char *)g_otaBuf, (const char *)SYS_RNWF_OTA_HTTP_CONTENT_OK);
                if(tmpPtr)
                {
                    SYS_RNWF_OTA_DBG_MSG("%s\r\n",g_otaBuf);
                    tmpPtr = (char *)strstr((const char *)g_otaBuf, (const char *)SYS_RNWF_OTA_HTTP_CONTENT_LEN);
                    if(tmpPtr == NULL)
                    {
                        break;
                    }
                    char *token = strtok(tmpPtr, "\r\n");
                    g_otaFileSize = strtol((token+sizeof(SYS_RNWF_OTA_HTTP_CONTENT_LEN)), NULL, 10);
                    g_otaCallBackHandler(SYS_RNWF_OTA_EVENT_DWLD_START, (uint8_t *)&g_otaFileSize);
                    break;
                } 
                else
                {
                    SYS_RNWF_OTA_DBG_MSG("File Not Found!\r\n");
                    SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_CLOSE, &socket);
                    g_otaCallBackHandler(SYS_RNWF_OTA_EVENT_DWLD_FAIL, (uint8_t *)NULL);
                }
                break;
            }
            
            result += read_size;
            total_rx = total_rx + read_size;
            rx_len -= read_size;

            /* Buffer is full, Initiate Write to SST26 callback */
            if(result == SYS_RNWF_OTA_BUF_LEN_MAX)
            {
                ota_chunk.chunk_size = SYS_RNWF_OTA_BUF_LEN_MAX;  
                SYS_RNWF_OTA_DBG_MSG("Downloaded : %lu\r\n", total_rx);
                g_otaCallBackHandler(SYS_RNWF_OTA_EVENT_FILE_CHUNK, (uint8_t *)&ota_chunk);                
                result = 0;
            }

            /* Downloading of image is completed , initiate callback */
            if(total_rx == g_otaFileSize)
            {
                ota_chunk.chunk_size = result; 
                g_otaCallBackHandler(SYS_RNWF_OTA_EVENT_FILE_CHUNK, (uint8_t *)&ota_chunk);
                g_otaCallBackHandler(SYS_RNWF_OTA_EVENT_DWLD_DONE, (uint8_t *)&total_rx);
                g_otaDwldDone = true;
                
                return SYS_RNWF_PASS;
            }
        }
        else
        {
            SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_CLOSE, &socket);
            g_otaCallBackHandler(SYS_RNWF_OTA_EVENT_DWLD_FAIL, (uint8_t *)NULL);
            return SYS_RNWF_FAIL;
        }
#else   /* RNWF11 */
        
        uint16_t readCnt = ((rx_len + result) > SYS_RNWF_OTA_BUF_LEN_MAX)?(SYS_RNWF_OTA_BUF_LEN_MAX-result):rx_len;  
        //check to not reading extra bytes
        if ((g_otaFileSize) && (g_otaFileSize - total_rx) < readCnt)
        {
            readCnt = (g_otaFileSize - total_rx);
        }
        if((read_size = SYS_RNWF_NET_TcpSockRead(socket, readCnt, (uint8_t *)&g_otaBuf[result])) > 0 )
        {
            //Parse the First HTTP Response
            if(!g_otaFileSize)
            {
                SYS_RNWF_OTA_DBG_MSG("HTTP Response :%s\r\n",&g_otaBuf[result]);
                if((tmpPtr = (char *)strstr((const char *)g_otaBuf, (const char *)SYS_RNWF_OTA_HTTP_CONTENT_LEN)) != NULL)
                {
                    char *token = strtok(tmpPtr, "\r\n");
                    g_otaFileSize = strtol((token+sizeof(SYS_RNWF_OTA_HTTP_CONTENT_LEN)), NULL, 10);
                    g_otaCallBackHandler(SYS_RNWF_OTA_EVENT_DWLD_START, (uint8_t *)&g_otaFileSize);
                }
                SYS_RNWF_OTA_DBG_MSG("otaFileSize :%d\r\n",g_otaFileSize);
                break;
            }
            
            result += read_size;
            total_rx = total_rx + read_size;
            rx_len -= read_size;
            
            /* Buffer is full, Initiate Write to SST26 callback */
            if(result == SYS_RNWF_OTA_BUF_LEN_MAX)
            {
                ota_chunk.chunk_size = SYS_RNWF_OTA_BUF_LEN_MAX;  
                SYS_RNWF_OTA_DBG_MSG("Downloaded : %lu\r\n", total_rx);
                g_otaCallBackHandler(SYS_RNWF_OTA_EVENT_FILE_CHUNK, (uint8_t *)&ota_chunk);                
                result = 0;
            }
            
            /* Downloading of image is completed , initiate callback */
            if(total_rx == g_otaFileSize)
            {
                SYS_RNWF_OTA_DBG_MSG("Total Downloaded :%lu ,OTA file size :%d\r\n", total_rx, g_otaFileSize);
                SYS_RNWF_OTA_DBG_MSG("Download Success!= %lu bytes\r\n", total_rx); 
                
                ota_chunk.chunk_size = result; 
                g_otaCallBackHandler(SYS_RNWF_OTA_EVENT_FILE_CHUNK, (uint8_t *)&ota_chunk);
                g_otaCallBackHandler(SYS_RNWF_OTA_EVENT_DWLD_DONE, (uint8_t *)&total_rx);
                g_otaDwldDone = true;
                
                return SYS_RNWF_PASS;
            }
        }
        else
        {
            SYS_RNWF_OTA_DBG_MSG("read fail :%d\r\n", read_size);
            break;
        }
#endif
    }
    return SYS_RNWF_PASS;
}


/* OTA Server Configuration Process */
static SYS_RNWF_RESULT_t SYS_RNWF_OTA_ConfProcess
(
    uint32_t socket,
    uint16_t rx_len
)
{
    int32_t read_size = 0, result = 0;
    uint8_t *tmpPtr, *buffer = &g_otaBuf[SYS_RNWF_OTA_BUF_LEN_MAX/2];
    SYS_RNWF_OTA_CFG_t otaCfg;
    while(rx_len > 0)
    {
        if((result = SYS_RNWF_NET_TcpSockRead(socket, rx_len, buffer)) > 0 )
        {
            rx_len -= result;
            read_size += result;
        }
        else
        {
            SYS_RNWF_OTA_DBG_MSG("TCP Read error\r\n");
            break;
        }
    }
    
    if(strncmp((char *)buffer, SYS_RNWF_OTA_CONF_FW_HDR, strlen(SYS_RNWF_OTA_CONF_FW_HDR)) || strncmp((char *)buffer, SYS_RNWF_OTA_CONF_FS_HDR, strlen(SYS_RNWF_OTA_CONF_FS_HDR)))
    {
        char *args[SYS_OTA_CFG_PARAM_MAX] = {0, 0, 0, 0}, *token;
        uint8_t idx = 0;
        tmpPtr = buffer + strlen(SYS_RNWF_OTA_CONF_FW_HDR);
        token = (char *)strtok((char *)tmpPtr, ", ");
        
        while(token != NULL)
        {
            SYS_CONSOLE_PRINT("%s\r\n", token);
            args[idx++] = token;
            token = (char *)strtok(NULL, ", ");
        }
        
        /* Configure Socket parameters */
        otaCfg.socket.tls_conf = SYS_RNWF_TLS_ENABLE1;
        otaCfg.socket.bind_type = SYS_RNWF_NET_BIND_TYPE1;
        otaCfg.socket.sock_type = SYS_RNWF_NET_SOCK_TYPE1;
        otaCfg.socket.sock_port = atoi(args[SYS_OTA_CFG_PARAM_PORT]);
        otaCfg.socket.sock_addr = args[SYS_OTA_CFG_PARAM_SERVER];
        
        otaCfg.file = args[SYS_OTA_CFG_PARAM_FILE];
        otaCfg.type = strtol(args[SYS_OTA_CFG_PARAM_TYPE], NULL, 10);
        
        g_otaCallBackHandler(SYS_RNWF_OTA_EVENT_IMAGE_INFO, (uint8_t *)&otaCfg);  
        SYS_RNWF_OTA_SrvCtrl(SYS_RNWF_OTA_REQUEST, (void *)&otaCfg);
    }
    
    return SYS_RNWF_PASS;
}

/* To Reset RNWF module using pin toggling */
void SYS_RNWF_OTA_DfuReset
(
   void
)
{
    const char *MCLR_RESET = "101";

#ifndef RNWF11_SERVICE      /* RNWF02 */
    SYS_RNWF_OTA_DBG_MSG("Resetting RNWF02 Module\r\n");
#else
    SYS_RNWF_OTA_DBG_MSG("Resetting RNWF11 Module\r\n");
#endif    
    /* Configure MCLR pin as GPIO */
    PORT_PinGPIOConfig(SYS_RNWF_OTA_MCLR_PORT_PIN);
    SYS_RNWF_OTA_PinSetDigitalOutput(SYS_RNWF_OTA_MCLR_PIN);
    
    /* Delay */
    SYS_RNWF_OTA_DelayMs(SYS_RNWF_OTA_MSEC_TO_SEC);
    
    /* Pin Toggling */
    for (uint8_t i=0; i<(uint8_t)strlen(MCLR_RESET); i++)
    {
        PORT_PinWrite(SYS_RNWF_OTA_MCLR_PORT_PIN, (MCLR_RESET[i] - '0'));
        
        SYS_RNWF_OTA_DelayUs(SYS_RNWF_OTA_TP_DELAY_USEC);
    }
    
    /* Set MCLR pin as Input */
    SYS_RNWF_OTA_PinSetDigitalInput(SYS_RNWF_OTA_MCLR_PIN);
}

#ifndef RNWF11_SERVICE      /* RNWF02 */
/* To Read response from RNWF in DFU mode */
static uint8_t *SYS_RNWF_OTA_PeReadResponse
(
    uint32_t timeout, 
    uint8_t respLen
)
{
    /* Buffer to store 4 byte response */
    static uint8_t resp[SYS_RNWF_OTA_PE_MAX_RESPONSE_SIZE] = {0};
    
    /* Timeout time in msec */
    uint32_t timeoutTime = SYS_TIME_CountToMS(SYS_TIME_CounterGet()) + timeout;
    
    while (SERCOM0_USART_ReadCountGet() < respLen)
    {
        if (((timeoutTime - SYS_TIME_CountToMS(SYS_TIME_CounterGet())) & 0x8000000) != 0)
        {
           SYS_RNWF_OTA_DBG_MSG("Error: PE response not found within timeout\r\n");
           /* Response will be returned as initialisation value and checked
              by DFU_PE_Erase() and DFU_PE_Write() */
           return resp;
        }
    }
        
    SYS_RNWF_IF_Read((uint8_t  *)&resp, respLen);

    #ifdef SYS_RNWF_OTA_DFU_DEBUG    
    SYS_RNWF_OTA_DBG_MSG("\r\nResponse: ");
    for (int i =0; i<respLen; i++)
    {
        SYS_RNWF_OTA_DBG_MSG("%02x", resp[i]);
    }
    #endif    
    return resp;
}
#endif

#ifdef SYS_RNWF_OTA_DFU_DEBUG
static uint32_t SYS_RNWF_OTA_DfuPeHtonl 
(
    uint32_t x
)
{
    uint32_t ret = x;
    
    if (BYTE_ORDER == BIG_ENDIAN)
    {   
    }
    else if (BYTE_ORDER == LITTLE_ENDIAN)
    {
      ret = __builtin_bswap32 (x);
    }
    else
    {
        SYS_RNWF_OTA_DBG_MSG("ERROR: Unknown system\r\n");
        while(1);
    }
    
    return ret;
}
#endif

#ifndef RNWF11_SERVICE      /* RNWF02 */
/* To Inject pattern to enter into RNWF DFU mode*/
static void SYS_RNWF_OTA_DfuPeInjectTestPattern
(
    void
)
{
    /* Programming Executive (PE) test pattern 'MCHP' PGC->Tx PGC->Rx*/
    const char *PE_TP_MCLR = "100000000000000000000000000000000000000000000000000000000000000001";
    const char *PE_TP_PGC  = "001010101010101010101010101010101010101010101010101010101010101011";
    const char *PE_TP_PGD  = "000110000111100110011000000001111001100001100000000110011000000000";
    
    SYS_RNWF_OTA_DBG_MSG("* Sending test pattern *\r\n%s\r\n%s\r\n%s\r\n", PE_TP_MCLR, PE_TP_PGC, PE_TP_PGD);
    
    /* Disable UART*/
    SERCOM0_USART_Disable();
        
    /* Configure UART Rx, Tx and MCLR PINs as GPIO output pins */
    SYS_RNWF_OTA_ConfigureGpioPins();
    
    /* Delay */
    SYS_RNWF_OTA_DelayMs(SYS_RNWF_OTA_MSEC_TO_SEC);
    
    /* Sending pattern */
    for (int i=0; i<(int)strlen(PE_TP_MCLR); i++)
    {
        /* MCLR */
        PORT_PinWrite(SYS_RNWF_OTA_MCLR_PORT_PIN, (PE_TP_MCLR[i] - '0'));
        /* PGC */
        PORT_PinWrite(SYS_RNWF_OTA_PGC_PORT_PIN, (PE_TP_PGC[i] - '0'));
        /* PGD */
        PORT_PinWrite(SYS_RNWF_OTA_PGD_PORT_PIN, (PE_TP_PGD[i] - '0'));
        
        SYS_RNWF_OTA_DelayUs(SYS_RNWF_OTA_TP_DELAY_USEC);
    }
    
    /* Initialize UART */
    SERCOM0_USART_Initialize();
    SERCOM0_USART_Enable();

    /*Configure PINs from GPIO -> UART */
    SYS_RNWF_OTA_PortUartInitialize();
    return;
}

/* To read PE version of RNWF module*/
static uint8_t SYS_RNWF_OTA_DfuPeVersion
(
    void
)
{ 
    uint32_t data = 0;
    uint8_t  peVersion = 0;
    uint8_t  *byteResp = NULL;
    
    data = SYS_RNWF_OTA_PE_CMD_EXEC_VERSION << 16;
    data |= 0x1;
    
    SYS_RNWF_OTA_DBG_MSG("Sending PE version request\r\n");
    #ifdef SYS_RNWF_OTA_DFU_DEBUG
    SYS_RNWF_OTA_DBG_MSG("%08x\r\n", (unsigned int)SYS_RNWF_OTA_DfuPeHtonl(data));
    #endif
    
    SYS_RNWF_IF_Write((uint8_t  *)&data, 4);
    byteResp = SYS_RNWF_OTA_PeReadResponse(SYS_RNWF_OTA_MSEC_TO_SEC, 4);
    peVersion = byteResp[0];
        
    SYS_RNWF_OTA_DBG_MSG("PE version: %d\r\n\r\n", (unsigned int)peVersion);
    
    return peVersion;
}

/* To read ChipId of module*/
static uint32_t SYS_RNWF_OTA_DfuPeChipID
(
    void
)
{
    uint32_t data = 0;
    uint32_t chipID = 0;
    uint8_t  *byteResp = NULL;
    
    data = SYS_RNWF_OTA_PE_CMD_GET_DEVICE_ID << 16;
    data |= 0x1;
    
    SYS_RNWF_OTA_DBG_MSG("Sending PE chip ID request\r\n");
    #ifdef SYS_RNWF_OTA_DFU_DEBUG
    SYS_RNWF_OTA_DBG_MSG("%08x\r\n", (unsigned int)SYS_RNWF_OTA_DfuPeHtonl(data));
    #endif
    SYS_RNWF_IF_Write((uint8_t  *)&data, 4);
    
    /* Response */
    byteResp = SYS_RNWF_OTA_PeReadResponse(SYS_RNWF_OTA_MSEC_TO_SEC, 8);
    
    memcpy(&chipID, byteResp+4, 4);
    
    SYS_RNWF_OTA_DBG_MSG("Chip ID: %08x\r\n\r\n", (unsigned int)chipID);
    
    return chipID;
}

/* To Erase RNWF module flash*/
static bool SYS_RNWF_OTA_DfuPeErase
(
    const uint32_t address,
    const uint32_t length
)
{ 
    uint32_t data = 0; 
    uint32_t pages = length / (uint32_t)SYS_RNWF_OTA_PE_ERASE_PAGE_SIZE;
    uint8_t  *byteResp = NULL;
    
    if (length % (uint32_t)SYS_RNWF_OTA_PE_ERASE_PAGE_SIZE > (uint32_t)0)
    {
        pages += (uint32_t)1;
    }
    
    data = pages &= 0x0000ffff;
    data |= (uint32_t)SYS_RNWF_OTA_PE_CMD_PAGE_ERASE << 16;

    SYS_RNWF_OTA_DBG_MSG("Sending PE erase\r\n");
    #ifdef SYS_RNWF_OTA_DFU_DEBUG
    SYS_RNWF_OTA_DBG_MSG("%08x\r\n", (unsigned int)SYS_RNWF_OTA_DfuPeHtonl(data));
    #endif

    while(false != DMAC_ChannelIsBusy(DMAC_CHANNEL_0));
    if(false == DMAC_ChannelTransfer(DMAC_CHANNEL_0, &data, \
            (const void * ) & (SERCOM0_REGS -> USART_INT.SERCOM_DATA),4))
    {
        return false;
    }
    SYS_RNWF_OTA_DelayUs(SYS_RNWF_OTA_WRITE_DELAY_USEC);
    
    #ifdef SYS_RNWF_OTA_DFU_DEBUG    
    SYS_RNWF_OTA_DBG_MSG("%08x\r\n", (unsigned int)SYS_RNWF_OTA_DfuPeHtonl(address));
    #endif

    while(false != DMAC_ChannelIsBusy(DMAC_CHANNEL_0));
    if(false == DMAC_ChannelTransfer(DMAC_CHANNEL_0, &address, \
            (const void * ) & (SERCOM0_REGS -> USART_INT.SERCOM_DATA),4))
    {
        return false;
    }
    SYS_RNWF_OTA_DelayUs(SYS_RNWF_OTA_WRITE_DELAY_USEC);
    
    /* Response */
    byteResp = SYS_RNWF_OTA_PeReadResponse(5 * SYS_RNWF_OTA_MSEC_TO_SEC, 4);
    
    if (((char)byteResp[2] != (char)SYS_RNWF_OTA_PE_CMD_PAGE_ERASE) || ((char)byteResp[0] != (char)0) || ((char)byteResp[1] != (char)0))
    {
        SYS_RNWF_OTA_DBG_MSG("Error: PE erase failed\r\n");
        return false;
    }
    
    SYS_RNWF_OTA_DBG_MSG("Erase done!\r\n");
    return true;
}

/* To write to RNWF  module in DFU mode*/
static bool SYS_RNWF_OTA_DfuPeWrite
(
    const uint32_t address,
    const uint32_t length, 
    const uint8_t *PE_writeBuffer
)
{
    /* The address must be 32-bit aligned, and the number of bytes (length) must be a
    multiple of a 32-bit word. */
    uint32_t data = 0;
    uint32_t checksumValue = 0;
    uint8_t *byteResp = NULL;
    
    if (length>(uint16_t)SYS_RNWF_OTA_MAX_PE_WRITE_SIZE) 
    {
        SYS_RNWF_OTA_DBG_MSG("ERROR: Length exceeds SYS_RNWF_OTA_MAX_PE_WRITE_SIZE\r\n");
        return false;
    }
    
    /* Length should be integer factor of 4096 and divisible by 4 */
    if ((((uint16_t)SYS_RNWF_OTA_MAX_PE_WRITE_SIZE % length) != (uint16_t)0) || ((length % (uint16_t)4) != (uint16_t)0))
    {
        SYS_RNWF_OTA_DBG_MSG("ERROR: Length should be integer factor of 4096 and divisible by 4\r\n");
        return false;
    } 
    
    /* Assemble PE write command */
    data |= ((uint32_t)0x0000ffff & (uint32_t)SYS_RNWF_OTA_PE_CMD_PGM_CLUSTER_VERIFY) << 16; 
    data |= (SYS_RNWF_OTA_CFG_METHOD & 0x0000ffff);
    #ifdef SYS_RNWF_OTA_DFU_DEBUG
        SYS_RNWF_OTA_DBG_MSG("ID:\r\n");
        SYS_RNWF_OTA_DBG_MSG("%08x\r\n", (unsigned int)SYS_RNWF_OTA_DfuPeHtonl(data));
    #endif

    while(false != DMAC_ChannelIsBusy(DMAC_CHANNEL_0));
    if(false == DMAC_ChannelTransfer(DMAC_CHANNEL_0,(const void *) &data, \
        (const void * ) & (SERCOM0_REGS -> USART_INT.SERCOM_DATA), sizeof(data)))
    {
        SYS_RNWF_OTA_DBG_MSG("ERROR: DFU write 1\r\n");
    }
    SYS_RNWF_OTA_DelayUs(SYS_RNWF_OTA_WRITE_DELAY_USEC);
    
    #ifdef SYS_RNWF_OTA_DFU_DEBUG
        /* Address */
        SYS_RNWF_OTA_DBG_MSG("Address:\r\n");
        SYS_RNWF_OTA_DBG_MSG("%08x\r\n", (unsigned int)SYS_RNWF_OTA_DfuPeHtonl(address));
    #endif

    while(false != DMAC_ChannelIsBusy(DMAC_CHANNEL_0));
    if(false == DMAC_ChannelTransfer(DMAC_CHANNEL_0, (const void*)&address, \
        (const void * ) & (SERCOM0_REGS -> USART_INT.SERCOM_DATA), sizeof(address)))
    {
        SYS_RNWF_OTA_DBG_MSG("ERROR: DFU write 2\r\n");
    }
    SYS_RNWF_OTA_DelayUs(SYS_RNWF_OTA_WRITE_DELAY_USEC);
    
    #ifdef SYS_RNWF_OTA_DFU_DEBUG
        /* Length */
        SYS_RNWF_OTA_DBG_MSG("Length: %d\r\n", (unsigned int)length);
        SYS_RNWF_OTA_DBG_MSG("%08x\r\n", (unsigned int)SYS_RNWF_OTA_DfuPeHtonl(length));
    #endif

    while(false != DMAC_ChannelIsBusy(DMAC_CHANNEL_0));
    if(false == DMAC_ChannelTransfer(DMAC_CHANNEL_0,(const void*) &length, \
        (const void * ) & (SERCOM0_REGS -> USART_INT.SERCOM_DATA), sizeof(length)))
    {
        SYS_RNWF_OTA_DBG_MSG("ERROR: DFU write\r\n");
    }
    SYS_RNWF_OTA_DelayUs(SYS_RNWF_OTA_WRITE_DELAY_USEC);
                
    /* Checksum */
    for (uint16_t i=0; i<length; i++)
    {
        checksumValue += PE_writeBuffer[i];
    }
    #ifdef SYS_RNWF_OTA_DFU_DEBUG
        SYS_RNWF_OTA_DBG_MSG("Checksum:\r\n");
        SYS_RNWF_OTA_DBG_MSG("%08x\r\n", (unsigned int)SYS_RNWF_OTA_DfuPeHtonl(checksumValue));
    #endif
      
    while(false != DMAC_ChannelIsBusy(DMAC_CHANNEL_0));
    if(false == DMAC_ChannelTransfer(DMAC_CHANNEL_0,(const void*) &checksumValue, \
        (const void * ) & (SERCOM0_REGS -> USART_INT.SERCOM_DATA), 4))
    {
        SYS_RNWF_OTA_DBG_MSG("ERROR: DFU write\r\n");
    }
    SYS_RNWF_OTA_DelayUs(SYS_RNWF_OTA_WRITE_DELAY_USEC);
    
    #ifdef SYS_RNWF_OTA_DFU_DEBUG
        /* Data */
        SYS_RNWF_OTA_DBG_MSG("PE_writeBuffer:\r\n");
    #endif
    for (uint16_t i=0; i<length; i++)
    {
        //SYS_RNWF_OTA_DBG_MSG("%02x ", PE_writeBuffer[i]);
        while(false != DMAC_ChannelIsBusy(DMAC_CHANNEL_0));
        if(false == DMAC_ChannelTransfer(DMAC_CHANNEL_0, (const void*)&PE_writeBuffer[i], \
            (const void * ) & (SERCOM0_REGS -> USART_INT.SERCOM_DATA),1))
        {
            SYS_RNWF_OTA_DBG_MSG("ERROR: DFU write\r\n");
        }
        
        SYS_RNWF_OTA_DelayUs(SYS_RNWF_OTA_WRITE_DELAY_USEC);
    }
    
    /* Response */
    byteResp = SYS_RNWF_OTA_PeReadResponse(SYS_RNWF_OTA_MSEC_TO_SEC, 4);
        
    /* Verify response for errors */
    if (((char)byteResp[2] != (char)SYS_RNWF_OTA_PE_CMD_PGM_CLUSTER_VERIFY) || ((char)byteResp[0] != (char)0) || ((char)byteResp[1] != (char)0))
    {
        SYS_RNWF_OTA_DBG_MSG("Error: PE write failed\r\n");
        return false;
    }
    
    return true;
}

#else   /* RNWF11_SERVICE */

static uint8_t send_request_cmd_data(uint8_t cmd, uint8_t *data, uint32_t dataLen, uint32_t dataAddr) {
    uint8_t req[SYS_RNWF_OTA_HEADER_SIZE + SYS_RNWF_OTA_ADDR_SIZE];
    int resp;
    uint8_t byteResp;
    
    if ( SYS_RNWF_OTA_BL_CMD_DATA == cmd )
    {
        dataLen += SYS_RNWF_OTA_ADDR_SIZE;
    }
    
    memset (req, 0, sizeof(req));
    
    req[0] = (uint8_t)(SYS_RNWF_OTA_BL_GUARD & 0xFF);        // store the least significant byte
    req[1] = (uint8_t)((SYS_RNWF_OTA_BL_GUARD >> 8) & 0xFF); 
    req[2] = (uint8_t)((SYS_RNWF_OTA_BL_GUARD >> 16) & 0xFF);
    req[3] = (uint8_t)((SYS_RNWF_OTA_BL_GUARD >> 24) & 0xFF);
    req[4] = (uint8_t)(dataLen & 0xFF);        // store the least significant byte
    req[5] = (uint8_t)((dataLen >> 8) & 0xFF); 
    req[6] = (uint8_t)((dataLen >> 16) & 0xFF);
    req[7] = (uint8_t)((dataLen >> 24) & 0xFF);
    req[8] = cmd;

    if (0 == dataLen)
    {
        req[9] = 0;
        req[10] = 0;
        req[11] = 0;
        req[12] = 0;
        SYS_RNWF_IF_Write(req, SYS_RNWF_OTA_HEADER_SIZE + SYS_RNWF_OTA_CMD_ONLY_EXTRA_SIZE);
    }
    else
    {
        req[9] = (uint8_t)(dataAddr & 0xFF);        // store the least significant byte
        req[10] = (uint8_t)((dataAddr >> 8) & 0xFF); 
        req[11] = (uint8_t)((dataAddr >> 16) & 0xFF);
        req[12] = (uint8_t)((dataAddr >> 24) & 0xFF);
        
        SYS_RNWF_IF_Write((uint8_t *)&req, SYS_RNWF_OTA_HEADER_SIZE + SYS_RNWF_OTA_ADDR_SIZE);

        for (uint16_t i=0; i<(dataLen - SYS_RNWF_OTA_ADDR_SIZE); i++)
        {
            //SYS_RNWF_OTA_DBG_MSG("%02x ", PE_writeBuffer[i]);
            while(false != DMAC_ChannelIsBusy(DMAC_CHANNEL_0));
            if(false == DMAC_ChannelTransfer(DMAC_CHANNEL_0, (const void*)&data[i], \
                (const void * ) & (SERCOM0_REGS -> USART_INT.SERCOM_DATA),1))
            {
                SYS_RNWF_OTA_DBG_MSG("ERROR: DFU write\r\n");
            }
        
        }

    }

    resp = SYS_RNWF_IF_Read(&byteResp, 1);
    
    if (resp == 0)
    {
        SYS_RNWF_OTA_DBG_MSG("ERROR: no response after write\r\n");
        byteResp = SYS_RNWF_OTA_BL_RESP_NONE;
    }

    return byteResp;
}


static uint8_t send_request(uint8_t cmd, uint8_t *data, int dataLen) {
    uint8_t req[SYS_RNWF_OTA_HEADER_SIZE + SYS_RNWF_OTA_CMD_ONLY_EXTRA_SIZE + dataLen];
    int resp;
    int cnt = 10;
    uint8_t byteResp;
    
    memset (req, 0, sizeof(req));
    
    req[0] = (uint8_t)(SYS_RNWF_OTA_BL_GUARD & 0xFF);        // store the least significant byte
    req[1] = (uint8_t)((SYS_RNWF_OTA_BL_GUARD >> 8) & 0xFF); 
    req[2] = (uint8_t)((SYS_RNWF_OTA_BL_GUARD >> 16) & 0xFF);
    req[3] = (uint8_t)((SYS_RNWF_OTA_BL_GUARD >> 24) & 0xFF);
    req[4] = (uint8_t)(dataLen & 0xFF);        // store the least significant byte
    req[5] = (uint8_t)((dataLen >> 8) & 0xFF); 
    req[6] = (uint8_t)((dataLen >> 16) & 0xFF);
    req[7] = (uint8_t)((dataLen >> 24) & 0xFF);
    req[8] = cmd;

    if ( 0 == dataLen)
    {
        req[9] = 0;
        req[10] = 0;
        req[11] = 0;
        req[12] = 0;
        SYS_RNWF_IF_Write((uint8_t *)&req, SYS_RNWF_OTA_HEADER_SIZE + SYS_RNWF_OTA_CMD_ONLY_EXTRA_SIZE);
    }
    else
    {
        memcpy(&req[9], data, dataLen);
        SYS_RNWF_IF_Write((uint8_t *)&req, SYS_RNWF_OTA_HEADER_SIZE + dataLen);
    }
 
    resp = 0;
    while (resp == 0 && cnt > 0)
    {
        resp = SYS_RNWF_IF_Read(&byteResp, 1);
        
        if (resp == 0)
        {
            SYS_RNWF_OTA_DBG_MSG("ERROR: no response after write, %d\r\n", cnt);
            byteResp = SYS_RNWF_OTA_BL_RESP_NONE;
        }
        cnt--;
    }
    
    return byteResp;
}


bool SYS_RNWF_OTA_DfuUartWrite(uint32_t address, const uint32_t length, uint8_t *writeBuffer)
{
    uint8_t resp;
    
    resp = send_request_cmd_data(SYS_RNWF_OTA_BL_CMD_DATA, writeBuffer, length, address);
    
    if ( SYS_RNWF_OTA_BL_RESP_OK == resp)
        return true;
    else
        return false;
}

static bool SYS_RNWF_OTA_DfuUartVerify(uint32_t crc)
{
    uint8_t resp;
    uint8_t dataBuf[4];
    bool res = false;
    
    memcpy(dataBuf, &crc, sizeof(crc));
    resp = send_request(SYS_RNWF_OTA_BL_CMD_VERIFY, dataBuf, sizeof(dataBuf));
    
    if ( SYS_RNWF_OTA_BL_RESP_CRC_OK == resp)
    {
        SYS_RNWF_OTA_DBG_MSG("crc32 verify success\r\n");
        res = true;
    }
    else
    {
        SYS_RNWF_OTA_DBG_MSG("crc32 verify fail\r\n");
    }

    return res;
}

#ifdef OTA_ERASE_BEFORE_WRITE
static bool SYS_RNWF_OTA_DfuUartEraseApp(void)
{
    uint8_t resp;
    uint8_t dataBuf[16];
    bool res = false;
    
    memset(dataBuf, 0, sizeof(dataBuf));

    resp = send_request(SYS_RNWF_OTA_BL_CMD_ERASE_APP, dataBuf, 16);
    
    if ( SYS_RNWF_OTA_BL_RESP_OK == resp)
    {
        res = true;
    }
    return res;
}
#endif
static bool SYS_RNWF_OTA_DfuUartReset(void)
{
    uint8_t resp;
    uint8_t dataBuf[16];
    bool res = false;
    
    memset(dataBuf, 0, sizeof(dataBuf));

    resp = send_request(SYS_RNWF_OTA_BL_CMD_RESET, dataBuf, 16);
    
    if ( SYS_RNWF_OTA_BL_RESP_OK == resp)
    {
        res = true;
    }
    return res;
}

static bool SYS_RNWF_OTA_DfuUartUnlock(uint32_t address, uint32_t size)
{
    uint8_t resp;
    uint8_t dataBuf[8];
    bool res = false;
    

    dataBuf[0] = (uint8_t)(address & 0xFF);        // store the least significant byte
    dataBuf[1] = (uint8_t)((address >> 8) & 0xFF); 
    dataBuf[2] = (uint8_t)((address >> 16) & 0xFF);
    dataBuf[3] = (uint8_t)((address >> 24) & 0xFF);
    
    dataBuf[4] = (uint8_t)(size & 0xFF);        // store the least significant byte
    dataBuf[5] = (uint8_t)((size >> 8) & 0xFF); 
    dataBuf[6] = (uint8_t)((size >> 16) & 0xFF);
    dataBuf[7] = (uint8_t)((size >> 24) & 0xFF);
    
    
    resp = send_request(SYS_RNWF_OTA_BL_CMD_UNLOCK, dataBuf, 8);
    
    if ( SYS_RNWF_OTA_BL_RESP_OK == resp)
    {
        res = true;
    }
    return res;
}

static uint16_t SYS_RNWF_OTA_DfuUartReadBootloaderVersion(void)
{
    uint8_t resp, majorVer, minorVer;
    uint16_t version = 0;

    resp = send_request(SYS_RNWF_OTA_BL_CMD_READ_VERSION, NULL, 0);
    
    if ( SYS_RNWF_OTA_BL_RESP_OK == resp)
    {
        SYS_RNWF_IF_Read(&majorVer, 1);
        SYS_RNWF_IF_Read(&minorVer, 1);

        version = (uint16_t)majorVer << 8 | minorVer;
    }
    else
    {
        SYS_RNWF_OTA_DBG_MSG("ERROR: Fail to read the bootloader version\r\n");
    }
    return version;
}

#endif

/* SST26 Flash Event Handler */
void SYS_RNWF_OTA_FlashEventHandler
(   
    DRV_SST26_TRANSFER_STATUS event,
    uintptr_t context
)
{
    switch(event)
    {
        case DRV_SST26_TRANSFER_COMPLETED:
        case DRV_SST26_TRANSFER_ERROR_UNKNOWN:
        {
            g_flashData.isTransferDone = true;
            break;
        }
        
        default:
        {
            break;
        }
    }
}

/* To Initialize SST26 Flash */
bool SYS_RNWF_OTA_FlashInitialize
(
    void
)
{
    if (DRV_SST26_Status(DRV_SST26_INDEX) == SYS_STATUS_READY)
    {
        g_flashData.handle = DRV_SST26_Open(DRV_SST26_INDEX, DRV_IO_INTENT_READWRITE);
        if (g_flashData.handle != DRV_HANDLE_INVALID)
        {
            DRV_SST26_EventHandlerSet(g_flashData.handle, SYS_RNWF_OTA_FlashEventHandler, (uintptr_t)NULL);

            if (DRV_SST26_GeometryGet(g_flashData.handle, &g_flashData.geometry) == true)
            {
                /* Read Jedec ID of SST26 Flash */
                uint8_t jedec_id[4];

                if (DRV_SST26_ReadJedecId (g_flashData.handle,(void*) jedec_id ) == true)
                {

                    uint8_t Device_Id = jedec_id[3];
                    uint8_t Device_Type = jedec_id[2];    
                    uint8_t Manufacturer_Id = jedec_id[1];

                    SYS_CONSOLE_PRINT("SPI Manufacturer ID = 0x%02X\r\n", Manufacturer_Id);
                    SYS_CONSOLE_PRINT("SPI Device Type = 0x%02X\r\n", Device_Type);
                    SYS_CONSOLE_PRINT("SPI Device ID = 0x%02X\r\n\r\n", Device_Id); 
                    
                    return true;
                }
            }
        }
    }
    return false;
}

/* To Erase SST26 Flash */
bool SYS_RNWF_OTA_FlashErase
(
    void
)
{
    DRV_SST26_ChipErase( g_flashData.handle );
    while(DRV_SST26_TRANSFER_COMPLETED != DRV_SST26_TransferStatusGet(g_flashData.handle) );
    return true;
}

/* To Erase SST26 Flash */
bool SYS_RNWF_OTA_SectorErase
(
    uint32_t addr
)
{
    DRV_SST26_SectorErase( g_flashData.handle, addr);
    while(DRV_SST26_TRANSFER_COMPLETED != DRV_SST26_TransferStatusGet(g_flashData.handle) );
    return true;
}

/* To write to SST26 Flash */
bool SYS_RNWF_OTA_FlashWrite
(
    uint32_t addr,
    uint32_t size,
    uint8_t *buf
)
{
#ifdef RNWF11_SERVICE
    int16_t pages = (size/g_flashData.geometry.write_blockSize);
    int8_t bytes = (size%g_flashData.geometry.write_blockSize);   
    char page_data[SST26_PAGE_SIZE];
    
#endif
    DRV_SST26_TRANSFER_STATUS transferStatus = DRV_SST26_TRANSFER_ERROR_UNKNOWN;
    uint32_t write_index = 0;
    g_flashData.isTransferDone = false;
    
#ifdef RNWF11_SERVICE
    while(pages--)
    {
        if (DRV_SST26_PageWrite(g_flashData.handle, &buf[write_index], (addr + write_index)) == true)
        {
            do
            {
                transferStatus = DRV_SST26_TransferStatusGet(g_flashData.handle);
            }while (transferStatus == DRV_SST26_TRANSFER_BUSY);
            
            write_index += g_flashData.geometry.write_blockSize;
        }
        else 
        {
            SYS_CONSOLE_PRINT("\tError\r\n");
            return false;
        }
        
    } 
    if (bytes)
    {
        memset(page_data, 0xFF, sizeof(page_data));
        memcpy(page_data, &buf[write_index], bytes);
        DRV_SST26_PageWrite(g_flashData.handle, &buf[write_index], (addr + write_index));
        while(DRV_SST26_TransferStatusGet(g_flashData.handle) == DRV_SST26_TRANSFER_BUSY);
    }
#else       
    while(size)
    {
        if (DRV_SST26_PageWrite(g_flashData.handle, &buf[write_index], (addr + write_index)) == true)
        {
            do
            {
                transferStatus = DRV_SST26_TransferStatusGet(g_flashData.handle);
            }while (transferStatus == DRV_SST26_TRANSFER_BUSY);
            
            write_index += g_flashData.geometry.write_blockSize;
            size = size - g_flashData.geometry.write_blockSize;
        }
        else 
        {
            SYS_RNWF_OTA_DBG_MSG("\tError\r\n");
            return false;
        }
        
    }
#endif
    return true;
}


/* To Read from SST26 Flash */
bool SYS_RNWF_OTA_FlashRead
(
    uint32_t addr,
    uint32_t size,
    uint8_t *buf
)
{
    DRV_SST26_TRANSFER_STATUS transferStatus = DRV_SST26_TRANSFER_ERROR_UNKNOWN;
    if (DRV_SST26_Read(g_flashData.handle, buf, size, addr) == true)
    {
        do
        {
            transferStatus = DRV_SST26_TransferStatusGet(g_flashData.handle);
        }
        while (transferStatus == DRV_SST26_TRANSFER_BUSY);
        return true;
        
    }
    return false;
}

/* Net Socket Callback function */
static void SYS_RNWF_OTA_SocketCallback
(
    uint32_t sock, 
    SYS_RNWF_NET_SOCK_EVENT_t event,
    uint8_t *p_str
)
{
    static uint32_t cfg_client_id = 0;
    switch(event)
    {
        
        case SYS_RNWF_NET_SOCK_EVENT_CONNECTED:
        {
            if(!cfg_client_id && strstr((const char *)p_str, SYS_RNWF_OTA_TOSTR(SYS_RNWF_OTA_CONF_PORT)))
            {
                cfg_client_id = sock;
                SYS_RNWF_OTA_DBG_MSG("Connection received on configuration tunnel %d\r\n", cfg_client_id);
                SYS_RNWF_SYSTEM_SrvCtrl(SYS_RNWF_SYSTEM_DEV_INFO, g_otaBuf);
            }
            SYS_RNWF_NET_TcpSockWrite(sock, strlen((char *)g_otaBuf), (uint8_t *)g_otaBuf);
            break;
        }

        case SYS_RNWF_NET_SOCK_EVENT_DISCONNECTED:
        {
            SYS_RNWF_OTA_DBG_MSG("Close OTA Socket!\r\n");
            cfg_client_id = 0;
            
            SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_CLOSE, &sock);
            break;
        }
        
        case SYS_RNWF_NET_SOCK_EVENT_READ:
        {
            uint16_t rx_len = *(uint16_t *)p_str;
            if(cfg_client_id == sock)
            {
                SYS_RNWF_OTA_ConfProcess(sock, rx_len);
            }
            else
            {
                SYS_RNWF_OTA_DownloadProcess(sock, rx_len);
            }
            break;
        }
        
        case SYS_RNWF_NET_SOCK_EVENT_ERROR:
        {
            SYS_RNWF_OTA_DBG_MSG("Socket Error! Re Trigger OTA operation!\r\n");
            break;
        }
        
        default:
        {
            break;
        }
    }
}

/* To Program RNWF module*/
void SYS_RNWF_OTA_ProgramDfu
(
    void
)
{    
    static SYS_RNWF_OTA_PROGRAM_EVENT_t program_event = SYS_RNWF_PROGRAM_INIT;
    static uint32_t flash_addr = SYS_RNWF_OTA_FLASH_IMAGE_START;
    static SYS_RNWF_OTA_CHUNK_t ota_chunk = { .chunk_addr = 0x60000000, .chunk_ptr = g_otaBuffer, .chunk_size = 0x84000};
    
    switch(program_event)
    {
        case SYS_RNWF_PROGRAM_INIT:
        {
            SYS_RNWF_OTA_DBG_MSG("Triggering DFU\r\n");
            SYS_RNWF_OTA_SrvCtrl(SYS_RNWF_OTA_DFU_INIT, (void *)NULL);
            program_event = SYS_RNWF_PROGRAM_DFU_ERASE;
            break;
        }
        
        case SYS_RNWF_PROGRAM_DFU_ERASE:
        {
            if(SYS_RNWF_OTA_SrvCtrl(SYS_RNWF_OTA_DFU_ERASE, (void *)&ota_chunk.chunk_addr) == SYS_RNWF_PASS)
            {
                program_event = SYS_RNWF_PROGRAM_FLASH_READ;
            }
            break;
        }
        
        case SYS_RNWF_PROGRAM_FLASH_READ:
        {
            ota_chunk.chunk_size = (g_otaFileSize < SYS_RNWF_OTA_BUF_LEN_MAX)?g_otaFileSize:SYS_RNWF_OTA_BUF_LEN_MAX;                 
            if(false == SYS_RNWF_OTA_FlashRead(flash_addr, ota_chunk.chunk_size, (uint8_t *)g_otaBuffer))
            {
                SYS_RNWF_OTA_DBG_MSG("ERROR : Flash read error\r\n");
                program_event = SYS_RNWF_PROGRAM_ERROR;
                break;
            }
            program_event = SYS_RNWF_PROGRAM_DFU_WRITE;
            break;
        }
        
        case SYS_RNWF_PROGRAM_DFU_WRITE:
        {
            /* Write to RNWF Flash */
            SYS_RNWF_OTA_SrvCtrl(SYS_RNWF_OTA_DFU_WRITE, (void *)&ota_chunk);
            
            g_otaFileSize -= ota_chunk.chunk_size;
            ota_chunk.chunk_addr += ota_chunk.chunk_size;
            flash_addr += ota_chunk.chunk_size;
            SYS_RNWF_OTA_DBG_MSG("Remaining %lu bytes\r\n", g_otaFileSize);
            
            if(!g_otaFileSize)
            {
                SYS_RNWF_OTA_DBG_MSG("RNWF Program Completed\r\n");
                program_event = SYS_RNWF_PROGRAM_COMPLETE;
                break;
            }
            program_event = SYS_RNWF_PROGRAM_FLASH_READ;
            break;
        }
        
        case SYS_RNWF_PROGRAM_ERROR:
        {
            SYS_RNWF_OTA_DBG_MSG("ERROR : Flash read error\r\n");
            break;
        }
        
        case SYS_RNWF_PROGRAM_COMPLETE:
        {
            g_otaDfuComplete = true;
            program_event = SYS_RNWF_PROGRAM_INIT;
            break;
        }
        
        default:
        {
            break;
        }
    }
}

/* OTA Service Control Message */
SYS_RNWF_RESULT_t SYS_RNWF_OTA_SrvCtrl
( 
    SYS_RNWF_OTA_SERVICE_t request,
    void *input
)
{
    SYS_RNWF_RESULT_t result = SYS_RNWF_PASS;    

    switch(request)
    {
        case SYS_RNWF_OTA_ENABLE:
        {
            // Bind a lock port to set the OTA configuration
            SYS_RNWF_NET_SOCKET_t ota_cfgSock;            
            if(input == NULL)
            {
                result = SYS_RNWF_FAIL;
                break;                
            }
            
            g_otaBuf = (uint8_t *)input;
            
            /* Configure Socket parameters manually through Net service Configuration*/
            ota_cfgSock.bind_type = SYS_RNWF_NET_BIND_TYPE0;
            ota_cfgSock.sock_port = SYS_RNWF_NET_SOCK_PORT0;
            ota_cfgSock.sock_type = SYS_RNWF_NET_SOCK_TYPE0;
            ota_cfgSock.tls_conf = SYS_RNWF_TLS_ENABLE0;
            
            /* RNWF Application Callback register */
            if(g_otaCallBackHandler != NULL)
            {
                SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_SET_SRVC_CALLBACK, SYS_RNWF_OTA_SocketCallback);
                result = SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_TCP_OPEN, &ota_cfgSock);
            }
            break;
        }
        
        /* Send http request to ota server */
        case SYS_RNWF_OTA_REQUEST:
        {
            SYS_RNWF_OTA_CFG_t *otaCfg = (SYS_RNWF_OTA_CFG_t *)input;
            
            char temp_buff[100] = {'\0'};
            char str1[] = "GET /";
            char str2[] = " HTTP/1.1\r\n Connection: Keep-Alive\r\n\r\n";
            memcpy(temp_buff, str1,strlen(str1));
            memcpy(&temp_buff[strlen(str1)], otaCfg->file,strlen(otaCfg->file) );
            memcpy(&temp_buff[strlen(str1) + strlen(otaCfg->file)], str2, strlen(str2) );
            
            memcpy(g_otaBuf, temp_buff, sizeof(temp_buff));
            #if SYS_RNWF_OTA_DFU_DEBUG
            SYS_RNWF_OTA_DBG_MSG("HTTP request : :%s\r\n",g_otaBuf);
            #endif
            if(g_otaCallBackHandler != NULL)
            {
                result = SYS_RNWF_NET_SockSrvCtrl(SYS_RNWF_NET_SOCK_TCP_OPEN, &otaCfg->socket);
            }
            break;
        }
        
        /* Set ota callback */
        case SYS_RNWF_OTA_SET_CALLBACK:
        {
            g_otaCallBackHandler = (SYS_RNWF_OTA_CALLBACK_t)(input);
            break;
        }
        
        /* Initialize RNWF for DFU */
        case SYS_RNWF_OTA_DFU_INIT:
        {   
#ifndef RNWF11_SERVICE      /* RNWF02 */         
            uint8_t  peVersion = 0;
            uint32_t chipID = 0;
            
            SYS_RNWF_OTA_DfuPeInjectTestPattern();
            
            SYS_RNWF_OTA_DelayMs(SYS_RNWF_OTA_UART_DELAY_MSEC);
            
            /* Verify PE version */
            peVersion = SYS_RNWF_OTA_DfuPeVersion();
            
            if(peVersion != SYS_RNWF_OTA_RIO0_PE_VERSION)
            {
                SYS_RNWF_OTA_DBG_MSG("PE version didn't match");
            }
            
            /* Verify chip ID */
            chipID = SYS_RNWF_OTA_DfuPeChipID();
            
            if (chipID != SYS_RNWF_OTA_RIO0_CHIP_ID)
            {
                SYS_RNWF_OTA_DBG_MSG("Chip ID didn't match");
                result =  SYS_RNWF_FAIL;
                break;
            }
            result =  SYS_RNWF_PASS;
#else   /* RNWF11_SERVUCE */
            uint16_t BTL_Ver = 0;
            int8_t btl_chk_cnt = 5;

            SYS_RNWF_OTA_CRC32_Init(g_crc_tab);
            g_crc = 0xffffffff;

            SYS_RNWF_SYSTEM_SrvCtrl(SYS_RNWF_SYSTEM_TBL, NULL);
                    
            SYS_RNWF_OTA_DelayMs(SYS_RNWF_OTA_UART_DELAY_MSEC);

            // Verify bootloader version
            while (btl_chk_cnt)
            {
                BTL_Ver = SYS_RNWF_OTA_DfuUartReadBootloaderVersion();
                
                if (BTL_Ver != 0)
                {
                    SYS_RNWF_OTA_DBG_MSG("BTL_Ver = 0x%x\r\n", BTL_Ver);
                    break;
                }
                
                btl_chk_cnt--;
                
                SYS_RNWF_OTA_DelayMs(SYS_RNWF_OTA_UART_DELAY_MSEC);
            }
            if (btl_chk_cnt == 0)
            {
                result = SYS_RNWF_FAIL;
            }
#endif
            break;
        }
        
        /* Erase RNWF Flash */
        case SYS_RNWF_OTA_DFU_ERASE:
        {
#ifndef RNWF11_SERVICE      /* RNWF02 */            
            SYS_RNWF_OTA_CHUNK_t *otaChunk = (SYS_RNWF_OTA_CHUNK_t *)input;             
            /* Erase */
            if (SYS_RNWF_OTA_DfuPeErase(otaChunk->chunk_addr, otaChunk->chunk_size) == false)
            {
                /* Error handling */
                SYS_RNWF_OTA_DBG_MSG("PE erase reported error\r\n");
                result = SYS_RNWF_FAIL;
            }  
#else   /* RNWF11_SERVUCE */
            SYS_RNWF_OTA_CHUNK_t *otaChunk = (SYS_RNWF_OTA_CHUNK_t *)input; 
            bool result = false;
            // Unlock the RNWF11 flash
            result = SYS_RNWF_OTA_DfuUartUnlock(otaChunk->chunk_addr, otaChunk->chunk_size);
#ifdef OTA_ERASE_BEFORE_WRITE            
            result = SYS_RNWF_OTA_DfuUartEraseApp();
#endif
            SYS_RNWF_OTA_DBG_MSG("DFU_UART_ERASE_APP, res=%d\r\n", result);
            
#endif             
            break;
        }
        
        /* Write to RNWF Flash */
        case SYS_RNWF_OTA_DFU_WRITE:
        {
            SYS_RNWF_OTA_CHUNK_t *otaChunk = (SYS_RNWF_OTA_CHUNK_t *)input;
#ifndef RNWF11_SERVICE      /* RNWF02 */            
            if(SYS_RNWF_OTA_DfuPeWrite(otaChunk->chunk_addr, otaChunk->chunk_size, otaChunk->chunk_ptr))
            {                
                result = SYS_RNWF_FAIL;
            }
#else   /* RNWF11_SERVUCE */
            
            SYS_RNWF_OTA_CRC32_Generate(otaChunk->chunk_ptr, otaChunk->chunk_size, &g_crc, g_crc_tab);
            if(SYS_RNWF_OTA_DfuUartWrite(otaChunk->chunk_addr, otaChunk->chunk_size, otaChunk->chunk_ptr) == false)
            {                
                result = SYS_RNWF_FAIL;
            }
#endif 
            break;
        }
        
        /* Error */
        case SYS_RNWF_OTA_DFU_ERROR:
        {
            SYS_RNWF_OTA_DBG_MSG("ERROR : DFU OTA Service Ctrl\r\n");
            break;
        }
        
        /* Get total image size */
        case SYS_RNWF_OTA_GET_IMG_SIZE:
        {
            bool *val = (bool *)input;
            *val = g_otaFileSize;
            break;
        }
        
        /* Check Download status */
        case SYS_RNWF_OTA_CHECK_DWLD_DONE:
        {
            bool *val = (bool *)input;
            *val = g_otaDwldDone;
            break;
        }
        
        /* Check if DFU is completed */
        case SYS_RNWF_OTA_CHECK_DFU_DONE:
        {
            bool *val = (bool *)input;
            *val = g_otaDfuComplete;
            break;
        }
        
#ifdef RNWF11_SERVICE
        case SYS_RNWF_OTA_DFU_RESET:
        {
            if (SYS_RNWF_OTA_DfuUartVerify(g_crc) == true)
            {
                SYS_CONSOLE_PRINT("crc32 verify pass\r\n");
                SYS_RNWF_OTA_DfuUartReset();
            }
            
        }
        break;
#endif                
        default:
        {
            break;
        }
    }
    
    return result;
}

/* *****************************************************************************
 End of File
 */