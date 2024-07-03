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
 ******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <stddef.h>                     
#include <stdbool.h>                   
#include <stdlib.h>                
#include <string.h>

/* This section lists the other files that are included in this file.
 */
#include "app.h"
#include "user.h"
#include "definitions.h" 
#include "configuration.h"
#include "system/debug/sys_debug.h"
#include "system/inf/sys_rnwf_interface.h"
#include "system/sys_rnwf_system_service.h"
#include "system/net/sys_rnwf_net_service.h"
#include "system/ota/sys_rnwf_ota_service.h"
#include "system/wifi/sys_rnwf_wifi_service.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Buffer

  Summary:
    Application Buffer array 

  Description:
    This array holds the application's buffer.

  Remarks:
    None
*/

static uint8_t g_appBuf[SYS_RNWF_OTA_BUF_LEN_MAX];

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

static APP_DATA g_appData;

// *****************************************************************************
/* Application Image size

  Summary:
    Holds size of the Image downloaded by Ota 

  Description:
    This variable size of the Image downloaded by Ota 

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/
uint8_t app_buf[SYS_RNWF_OTA_BUF_LEN_MAX];
uint32_t gOta_file_size = 0;

#define OTA_FLASH_IMAGE_START       (0x00000000)

#define RNWF11_IMAGE_ADDR           (0x90000000)
#define RNWF11_IMAGE_SIZE           (1048576)
#define SST26_PAGE_SIZE             256
#define SST26_SECTOR_SIZE             4096

#define PARTITION_HDR_STRUCT_SIZE       10
#define PARTITION_HDR_SIZE          SST26_SECTOR_SIZE
#define PARTITION_SIZE              (RNWF11_IMAGE_SIZE + PARTITION_HDR_SIZE)
#define FACTORY_PARTITION_HDR_ADDR  (0x00000000)
#define FACTORY_PARTITION_IMG_ADDR  (FACTORY_PARTITION_HDR_ADDR + PARTITION_HDR_SIZE)
#define FIRST_PARTITION_HDR_ADDR    FACTORY_PARTITION_HDR_ADDR + PARTITION_SIZE
#define FIRST_PARTITION_IMG_ADDR    FIRST_PARTITION_HDR_ADDR + PARTITION_HDR_SIZE
#define SECOND_PARTITION_HDR_ADDR   FIRST_PARTITION_HDR_ADDR + PARTITION_SIZE
#define SECOND_PARTITION_IMG_ADDR   SECOND_PARTITION_HDR_ADDR + PARTITION_HDR_SIZE


#define PARTITION_HEADER_STATUS_INIT        0
#define PARTITION_HEADER_STATUS_FACTORY     1
#define PARTITION_HEADER_STATUS_ACTIVE      2
#define PARTITION_HEADER_STATUS_OTA         3
#define PARTITION_HEADER_STATUS_OLD         4
#define PARTITION_HEADER_STATUS_UNKNOWN     5

#define APP_FUNC_RESULT_SUCCESS     0
#define APP_FUNC_RESULT_FAIL        (-1)

#define OTA_IMAGE_WITHOUT_FLFS_SIZE  0xDF000
#define SST26_FLASH_ERASE_TIMEOUT_COUNTER   12

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

int APP_OTA_Program(uint32_t flash_addr)
{    
    SYS_RNWF_OTA_CHUNK_t ota_chunk = { .chunk_addr = 0x90000000, .chunk_ptr = app_buf, .chunk_size = 0x100000};

    SYS_CONSOLE_PRINT("start: flash_addr = 0x%lx\r\n", flash_addr);
    
    if (gOta_file_size == OTA_IMAGE_WITHOUT_FLFS_SIZE)
        ota_chunk.chunk_size = gOta_file_size;
    
    if (SYS_RNWF_OTA_SrvCtrl(SYS_RNWF_OTA_DFU_INIT, (void *)NULL) == SYS_RNWF_FAIL)
    {
        SYS_CONSOLE_PRINT("Fail to enter bootloader mode\r\n");
        return APP_FUNC_RESULT_FAIL;
    }
    
    while(SYS_RNWF_OTA_SrvCtrl(SYS_RNWF_OTA_DFU_ERASE, (void *)&ota_chunk.chunk_addr) != SYS_RNWF_PASS);

    while(gOta_file_size)
    {                
        //ota_chunk.chunk_size = (gOta_file_size < OTA_BUF_LEN_MAX)?gOta_file_size:OTA_BUF_LEN_MAX; 
        ota_chunk.chunk_size = SYS_RNWF_OTA_BUF_LEN_MAX;
        
        memset(app_buf, 0, sizeof(app_buf));
        SYS_RNWF_OTA_FlashRead(flash_addr, ota_chunk.chunk_size, (uint8_t *) &app_buf);
      
        if (SYS_RNWF_OTA_SrvCtrl(SYS_RNWF_OTA_DFU_WRITE, (void *)&ota_chunk) != SYS_RNWF_PASS)
        {
            return APP_FUNC_RESULT_FAIL;
        }

        if (gOta_file_size < SYS_RNWF_OTA_BUF_LEN_MAX)
        {
            gOta_file_size = 0;
        }
        else
        {
            gOta_file_size -= ota_chunk.chunk_size;
        }
        ota_chunk.chunk_addr += ota_chunk.chunk_size;
        flash_addr += ota_chunk.chunk_size;

        SYS_CONSOLE_PRINT("Flashed %lu bytes\r\n", gOta_file_size);  
    }

    if (SYS_RNWF_OTA_SrvCtrl(SYS_RNWF_OTA_DFU_RESET, (void *)NULL) != SYS_RNWF_PASS)
    {
        return APP_FUNC_RESULT_FAIL;
    }

    return APP_FUNC_RESULT_SUCCESS;
}


/* Wifi Callback Handler Function */
static void SYS_RNWF_WIFI_CallbackHandler
(
    SYS_RNWF_WIFI_EVENT_t event, 
    uint8_t *p_str
)
{
	static int ota_enable = 0;
    switch(event)
    {
        /* Wifi Connected */
        case SYS_RNWF_CONNECTED:
        {
            SYS_CONSOLE_PRINT("Wi-Fi Connected\r\n");
            break;
        }
        
        /* Wifi Disconnected */
        case SYS_RNWF_DISCONNECTED:
        {    
            SYS_CONSOLE_PRINT("WiFi Disconnected\nReconnecting... \r\n");
            SYS_RNWF_WIFI_SrvCtrl(SYS_RNWF_STA_CONNECT, NULL);
            break;
        }
            
        /* DHCP IP allocated */
        case SYS_RNWF_DHCP_DONE:
        {
            SYS_CONSOLE_PRINT("DHCP IP:%s\r\n", &p_str[2]);
			if (ota_enable == 0)  // avoid multiple invoke OTA enable twice if the device get both IPv4 and IPv6 IP addr 
            {
                /* Enable OTA by passing the OTA buffer space */
                if(SYS_RNWF_OTA_SrvCtrl(SYS_RNWF_OTA_ENABLE, (void *)app_buf) == SYS_RNWF_PASS)
                {
                    ota_enable = 1;
                    SYS_CONSOLE_PRINT("Successfully Enabled the OTA\r\n");
                }
                else
                {
                    SYS_CONSOLE_PRINT("ERROR!!! Failed to enable the OTA\r\n");
                }
            }
            break;
        }
        
        /* Wifi Scan Indication */
        case SYS_RNWF_SCAN_INDICATION:
        {
            break;
        }
            
        /* Wifi Scan Done */
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

/* OTA Callback Handler Function */
static void SYS_RNWF_OTA_CallbackHandler
(
    SYS_RNWF_OTA_EVENT_t event,
    void *p_str
)
{
    static uint32_t flash_addr = SYS_RNWF_OTA_FLASH_IMAGE_START;
	static uint32_t flash_partition_addr = 0;
    static uint32_t flash_partition_hdr_addr = 0;
    PARTITION_HDR partition_header;
    uint32_t erase_addr;
    int i = 0;
    
    switch(event)
    {
        /* Change to UART mode */
        case SYS_RNWF_OTA_EVENT_MAKE_UART:
        {
            break;
        }
            
        /* FW Download start */
        case SYS_RNWF_OTA_EVENT_DWLD_START:
        {
            SYS_CONSOLE_PRINT("Total Size = %lu\r\n", *(uint32_t *)p_str); 
            /* determine the starting address of the external flash */   
            flash_partition_addr = 0;
            flash_partition_hdr_addr = 0;
            SYS_RNWF_OTA_FlashRead(FIRST_PARTITION_HDR_ADDR, sizeof(partition_header), (uint8_t *) &partition_header);

            if (partition_header.status != PARTITION_HEADER_STATUS_ACTIVE)
            {
                flash_partition_addr = FIRST_PARTITION_IMG_ADDR;
                flash_partition_hdr_addr = FIRST_PARTITION_HDR_ADDR;
                flash_addr = FIRST_PARTITION_IMG_ADDR;
                SYS_CONSOLE_PRINT("partition 1 is used for OTA\r\n"); 
            }
            else 
            {
                SYS_RNWF_OTA_FlashRead(SECOND_PARTITION_HDR_ADDR, sizeof(partition_header), (uint8_t *) &partition_header);

                if (partition_header.status !=  PARTITION_HEADER_STATUS_ACTIVE)
                {
                    flash_partition_addr = SECOND_PARTITION_IMG_ADDR;
                    flash_partition_hdr_addr = SECOND_PARTITION_HDR_ADDR;
                    flash_addr = SECOND_PARTITION_IMG_ADDR;
                    SYS_CONSOLE_PRINT("partition 2 is used for OTA\r\n");
                }
                else
                {
                    SYS_CONSOLE_PRINT("No available partition is found for OTA\r\n");
                }          
            }
            
            if (flash_partition_addr)
            {          
                SYS_CONSOLE_PRINT("erase partition addr = 0x%lx\r\n", flash_partition_addr);

                /* update partition header */
                PARTITION_HDR partition_header;
                
                SYS_RNWF_OTA_SectorErase(flash_partition_hdr_addr);
                SYS_RNWF_OTA_FlashRead(flash_partition_hdr_addr, sizeof(partition_header), (uint8_t *) &partition_header);

                partition_header.status = PARTITION_HEADER_STATUS_UNKNOWN;
                SYS_RNWF_OTA_FlashWrite(flash_partition_hdr_addr,sizeof(PARTITION_HDR) ,(uint8_t *) &partition_header);

                /* erase the used partition */
                erase_addr = flash_partition_addr;
                for (i = 0; i < (1024/4); i++)
                {
                    SYS_RNWF_OTA_SectorErase(erase_addr);
                    erase_addr = erase_addr + 4096;
                }
                //SYS_CONSOLE_PRINT("log\r\n");
            }
            break;
        }
        
        
        /* FW Download done */
        case SYS_RNWF_OTA_EVENT_DWLD_DONE:
        {       
            gOta_file_size = *(uint32_t *)p_str;  
            SYS_CONSOLE_PRINT("Download Success!= %lu bytes\r\n", gOta_file_size);  
			
			if (flash_partition_addr)
            {
                /* update the partition header */
                if (flash_partition_addr == FIRST_PARTITION_IMG_ADDR)
                {
                    SYS_RNWF_OTA_FlashRead(SECOND_PARTITION_HDR_ADDR, sizeof(partition_header), (uint8_t *) &partition_header);

                    if (partition_header.status == PARTITION_HEADER_STATUS_ACTIVE)
                    {
                        partition_header.status = PARTITION_HEADER_STATUS_OLD;
                        SYS_RNWF_OTA_SectorErase(SECOND_PARTITION_HDR_ADDR);
                        
                        SYS_RNWF_OTA_FlashWrite(SECOND_PARTITION_HDR_ADDR,sizeof(PARTITION_HDR), (uint8_t *) &partition_header);
                    }
                }
                else if (flash_partition_addr == SECOND_PARTITION_IMG_ADDR)
                {
                    SYS_RNWF_OTA_FlashRead(FIRST_PARTITION_HDR_ADDR, sizeof(partition_header), (uint8_t *) &partition_header);

                    if (partition_header.status == PARTITION_HEADER_STATUS_ACTIVE)
                    {
                        partition_header.status = PARTITION_HEADER_STATUS_OLD;
                        SYS_RNWF_OTA_SectorErase(FIRST_PARTITION_HDR_ADDR);
                        
                        SYS_RNWF_OTA_FlashWrite(FIRST_PARTITION_HDR_ADDR,sizeof(PARTITION_HDR) ,(uint8_t *) &partition_header);
                    }
                }

                SYS_RNWF_OTA_SectorErase(flash_partition_hdr_addr);
                
                /* update partition header */
                PARTITION_HDR partition_header;
                SYS_RNWF_OTA_FlashRead(flash_partition_hdr_addr, sizeof(partition_header), (uint8_t *) &partition_header);

                partition_header.status = PARTITION_HEADER_STATUS_OTA;
                partition_header.imageSize = gOta_file_size; 
                
                SYS_RNWF_OTA_FlashWrite(flash_partition_hdr_addr,sizeof(PARTITION_HDR) , (uint8_t *) &partition_header);
            }


            /* program the module firwmare */
            if (flash_partition_addr != 0)
            {
                SYS_CONSOLE_PRINT("Program RNWF11\r\n");

                if (APP_OTA_Program(flash_partition_addr) == APP_FUNC_RESULT_SUCCESS)
                {
                    /* update the partition header */
                    SYS_RNWF_OTA_SectorErase(flash_partition_hdr_addr);
                    
                    PARTITION_HDR partition_header;
                    partition_header.status = PARTITION_HEADER_STATUS_ACTIVE;
                    
                    SYS_RNWF_OTA_FlashWrite(flash_partition_hdr_addr,sizeof(PARTITION_HDR) ,(uint8_t *) &partition_header);
                }

            }
            else
            {
                SYS_CONSOLE_PRINT("No available partition is found for OTA\r\n");
            }
            
            /* Host Reset */
            SYS_RESET_SoftwareReset();
            
            break; 
        }
              
        /* Write to SST26 */
        case SYS_RNWF_OTA_EVENT_FILE_CHUNK:
        {
            volatile SYS_RNWF_OTA_CHUNK_t *ota_chunk = (SYS_RNWF_OTA_CHUNK_t *)p_str;               
            SYS_RNWF_OTA_FlashWrite(flash_addr,ota_chunk->chunk_size ,ota_chunk->chunk_ptr);
            flash_addr += ota_chunk->chunk_size;
            break; 
        }    
                           
        default:
        {
            break;
        }
    }
    
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************
static unsigned char Generate_CheckSum(unsigned char *data, int size)
{
    unsigned char checksum = 0;
    
    for (int i = 0; i < size; i++) {
        checksum += data[i];
    }
    checksum = ~checksum; // one's complement of the sum
    
    return checksum;
}


static int DFU(uint32_t start_address, uint32_t size)
{
    int result = -1;
    gOta_file_size = size;

    result = APP_OTA_Program(start_address);
    
    return result;    
}

static int Recover_RNWF11(PARTITION_HDR * fact_prt_hdr, PARTITION_HDR * first_prt_hdr, PARTITION_HDR * second_prt_hdr)
{
    int result = APP_FUNC_RESULT_FAIL;
    char page_data[SST26_PAGE_SIZE];
    SYS_CONSOLE_PRINT("Recover_RNWF11 1\r\n"); 
    if (first_prt_hdr->status == PARTITION_HEADER_STATUS_OLD)
    {
        SYS_CONSOLE_PRINT("Recover_RNWF11 2\r\n"); 
        if (SYS_RNWF_SYSTEM_Module_Init(SYS_RNWF_SYSTEM_BTL_MODE) == SYS_RNWF_PASS)
        {
            SYS_CONSOLE_PRINT("Recover_RNWF11 3\r\n"); 
            //SYS_CONSOLE_PRINT("Recover to partition 1\r\n"); 
            if (DFU(FIRST_PARTITION_IMG_ADDR, first_prt_hdr->imageSize) == APP_FUNC_RESULT_SUCCESS)
            {                        

                first_prt_hdr->status = PARTITION_HEADER_STATUS_ACTIVE;

                memset(page_data, 0xFF, sizeof(page_data));
                memcpy(page_data, first_prt_hdr, sizeof(PARTITION_HDR)); 
                
                SYS_RNWF_OTA_SectorErase(FIRST_PARTITION_HDR_ADDR);
                SYS_RNWF_OTA_FlashWrite(FIRST_PARTITION_HDR_ADDR, SST26_PAGE_SIZE, (uint8_t *) page_data);
                SYS_RNWF_OTA_SectorErase(SECOND_PARTITION_HDR_ADDR);

                result = APP_FUNC_RESULT_SUCCESS;
            }

        }
    }
    else if (second_prt_hdr->status == PARTITION_HEADER_STATUS_OLD)
    {
        if (SYS_RNWF_SYSTEM_Module_Init(SYS_RNWF_SYSTEM_BTL_MODE) == SYS_RNWF_PASS)
        {
            SYS_CONSOLE_PRINT("Recover to partition 2\r\n");   
            if (DFU(SECOND_PARTITION_IMG_ADDR, second_prt_hdr->imageSize) == APP_FUNC_RESULT_SUCCESS)
            {
                second_prt_hdr->status = PARTITION_HEADER_STATUS_ACTIVE;
                
                memset(page_data, 0xFF, sizeof(page_data));
                memcpy(page_data, second_prt_hdr, sizeof(PARTITION_HDR)); 
                
                SYS_RNWF_OTA_SectorErase(SECOND_PARTITION_HDR_ADDR);
                SYS_RNWF_OTA_FlashWrite(SECOND_PARTITION_HDR_ADDR, SST26_PAGE_SIZE, (uint8_t *) page_data);

                SYS_RNWF_OTA_SectorErase(FIRST_PARTITION_HDR_ADDR);

                result = APP_FUNC_RESULT_SUCCESS;
            }
        }
        
    }
    else if (fact_prt_hdr->status == PARTITION_HEADER_STATUS_FACTORY)
    {
        if (SYS_RNWF_SYSTEM_Module_Init(SYS_RNWF_SYSTEM_BTL_MODE) == SYS_RNWF_PASS)
        {
            SYS_CONSOLE_PRINT("Recover to factory partition\r\n"); 
            if (DFU(FACTORY_PARTITION_IMG_ADDR, fact_prt_hdr->imageSize) == APP_FUNC_RESULT_SUCCESS)
            {
                SYS_RNWF_OTA_SectorErase(FIRST_PARTITION_HDR_ADDR);
                SYS_RNWF_OTA_SectorErase(SECOND_PARTITION_HDR_ADDR);
                result = APP_FUNC_RESULT_SUCCESS;

            }
        }
    }  
    return result;

}

static void OTA_FailSafe_Mechanism(PARTITION_HDR * fact_prt_hdr, PARTITION_HDR * first_prt_hdr, PARTITION_HDR * second_prt_hdr)
{
    char page_data[SST26_PAGE_SIZE];

    if (PARTITION_HEADER_STATUS_OTA == first_prt_hdr->status)
    {
        if (Recover_RNWF11(fact_prt_hdr, first_prt_hdr, second_prt_hdr) == APP_FUNC_RESULT_SUCCESS)     
        {

            SYS_CONSOLE_PRINT("Change Partition status to unknown\r\n");
            first_prt_hdr->status = PARTITION_HEADER_STATUS_UNKNOWN;

            SYS_RNWF_OTA_SectorErase(FIRST_PARTITION_HDR_ADDR );

            memset(page_data, 0xFF, sizeof(page_data));
            memcpy(page_data, first_prt_hdr, sizeof(PARTITION_HDR));

            SYS_RNWF_OTA_FlashWrite(FIRST_PARTITION_HDR_ADDR,sizeof(page_data) , (uint8_t *) &page_data);

            NVIC_SystemReset();

        }
        
        else
        {
            SYS_CONSOLE_PRINT("Fail to recover..\r\n");
        }

    }
    
    
    if (PARTITION_HEADER_STATUS_OTA == second_prt_hdr->status)
    {
        if (Recover_RNWF11(fact_prt_hdr, first_prt_hdr, second_prt_hdr) == APP_FUNC_RESULT_SUCCESS)  
        {

            SYS_CONSOLE_PRINT("Change Partition2 status to unknown\r\n");
            second_prt_hdr->status = PARTITION_HEADER_STATUS_UNKNOWN;

            SYS_RNWF_OTA_SectorErase(SECOND_PARTITION_HDR_ADDR );

            memset(page_data, 0xFF, sizeof(page_data));
            memcpy(page_data, second_prt_hdr, sizeof(PARTITION_HDR));
            SYS_RNWF_OTA_FlashWrite(SECOND_PARTITION_HDR_ADDR,sizeof(page_data) , (uint8_t *) &page_data);

            NVIC_SystemReset();


        }
        else
        {
            SYS_CONSOLE_PRINT("Fail to recover..\r\n");
        }
    }
   
    return;
}

void Read_RNWF11_FLASH_TO_EXT_FLASH(void)
{
    uint32_t src_addr, des_addr_fact_part, des_addr_1st_part;
    int32_t size_to_read;
    uint8_t buffer[SST26_PAGE_SIZE + 1];
    int result = 0;
    unsigned char checksum;
    
    src_addr = RNWF11_IMAGE_ADDR;
    des_addr_fact_part = FACTORY_PARTITION_IMG_ADDR;
    des_addr_1st_part = FIRST_PARTITION_IMG_ADDR;
    size_to_read = RNWF11_IMAGE_SIZE;

    SYS_RNWF_OTA_FlashErase();

    while (size_to_read > 0)
    {
        if (SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, "AT+READAPPIMAGE=0x%lx,%d\r\n", src_addr, SST26_PAGE_SIZE) == SYS_RNWF_RAW)
        {
            result = SYS_RNWF_IF_RawRead(buffer, SST26_PAGE_SIZE + 1);    // add 1 for checksum
            //printf("result = %d, chksum = 0x%x\r\n", result, buffer[SST26_PAGE_SIZE]);

            checksum = Generate_CheckSum(buffer, SST26_PAGE_SIZE);

            if (checksum != buffer[SST26_PAGE_SIZE])
            {
                SYS_CONSOLE_PRINT("chksum fail...\r\n");
                return;
            }

            if ( result != (SST26_PAGE_SIZE + 1) )
            {
                SYS_CONSOLE_PRINT("program fail...\r\n");
                return;
            }

            SYS_RNWF_OTA_FlashWrite(des_addr_fact_part, SST26_PAGE_SIZE, buffer);

            /* erase and program the image to external flash first partition */
            if (des_addr_1st_part % SST26_SECTOR_SIZE == 0)
            {
                SYS_RNWF_OTA_SectorErase(des_addr_1st_part);
            }
            SYS_RNWF_OTA_FlashWrite(des_addr_1st_part, SST26_PAGE_SIZE, buffer);


        }
        size_to_read -= SST26_PAGE_SIZE;
        src_addr += SST26_PAGE_SIZE;
        des_addr_fact_part += SST26_PAGE_SIZE;
        des_addr_1st_part += SST26_PAGE_SIZE;       
    }

    SYS_RNWF_OTA_SectorErase(FACTORY_PARTITION_HDR_ADDR );
    SYS_RNWF_OTA_SectorErase(FIRST_PARTITION_HDR_ADDR );

    /* update the partition header */
    PARTITION_HDR partition_header;
    char page_data[SST26_PAGE_SIZE];
    partition_header.status = PARTITION_HEADER_STATUS_FACTORY;
    partition_header.imageSize = RNWF11_IMAGE_SIZE;
    memset(page_data, 0xFF, sizeof(page_data));
    memcpy(page_data, &partition_header, sizeof(partition_header));
    SYS_RNWF_OTA_FlashWrite(FACTORY_PARTITION_HDR_ADDR, SST26_PAGE_SIZE, (uint8_t *) page_data);

    partition_header.status = PARTITION_HEADER_STATUS_ACTIVE;
    partition_header.imageSize = RNWF11_IMAGE_SIZE;
    memset(page_data, 0xFF, sizeof(page_data));
    memcpy(page_data, &partition_header, sizeof(partition_header));
    SYS_RNWF_OTA_FlashWrite(FIRST_PARTITION_HDR_ADDR, SST26_PAGE_SIZE, (uint8_t *) page_data);

}

void EXT_FLASH_Initilize(PARTITION_HDR * fact_prt_hdr)
{
    if (PARTITION_HEADER_STATUS_FACTORY != fact_prt_hdr->status)
    {
        SYS_CONSOLE_PRINT("Copy RN module FW to factory partition, Wait..\r\n");            
        Read_RNWF11_FLASH_TO_EXT_FLASH();
        SYS_CONSOLE_PRINT("Complete.\r\n");  
    }
    return;
}
/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize 
( 
    void 
)
{
    /* Place the App state machine in its initial state. */
    g_appData.state = APP_STATE_INITIALIZE;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

void APP_RNWF_SwResetHandler
(
    void
)
{
    /* RNWF Reset */
    SYS_RNWF_OTA_DfuReset();
    
    /* Manual Delay to synchronise host reset. 
     * User can change according to their Host reset timing*/
    for(int i=0; i< 0xFFFFF; i++)
    {
        SYS_CONSOLE_PRINT("");
    }
    
    /* Host Reset */
    SYS_RESET_SoftwareReset();
}


/* Maintain the application's state machine. */
void APP_Tasks 
( 
    void 
)
{
    PARTITION_HDR fact_prt_hdr, first_prt_hdr, second_prt_hdr;
    static int switch_counter = 0;
    
    switch(g_appData.state)
    {
        /* Initialize Flash and RNWF device */
        case APP_STATE_INITIALIZE:
        {
            SYS_CONSOLE_PRINT("%s", "##############################################\r\n");
            SYS_CONSOLE_PRINT("%s", "  Welcome RNWF11 WiFi Host Assisted OTA Demo  \r\n");
            SYS_CONSOLE_PRINT("%s", "##############################################\r\n\r\n"); 
            
            SYS_RNWF_SYSTEM_Module_Init(SYS_RNWF_SYSTEM_APP_MODE);
                    
            if(false == SYS_RNWF_OTA_FlashInitialize())
            {
                SYS_CONSOLE_PRINT("ERROR : No valid SPI Flash found!\r\n\tConnect SPI MikroBus(SST26) to EXT2 and reset!\r\n");
                g_appData.state = APP_STATE_ERROR;
                break;
            }
            
            SYS_RNWF_OTA_FlashRead(FACTORY_PARTITION_HDR_ADDR, sizeof(fact_prt_hdr), (uint8_t *) &fact_prt_hdr);
            SYS_RNWF_OTA_FlashRead(FIRST_PARTITION_HDR_ADDR, sizeof(first_prt_hdr), (uint8_t *) &first_prt_hdr);
            SYS_RNWF_OTA_FlashRead(SECOND_PARTITION_HDR_ADDR, sizeof(second_prt_hdr), (uint8_t *) &second_prt_hdr);
            
            SYS_CONSOLE_PRINT("\r\nfact prt: status = %d, size = 0x%lx\r\n", fact_prt_hdr.status, fact_prt_hdr.imageSize);
            SYS_CONSOLE_PRINT("first prt: status = %d, size = 0x%lx\r\n", first_prt_hdr.status, first_prt_hdr.imageSize);
            SYS_CONSOLE_PRINT("second prt: status = %d, size = 0x%lx\r\n\r\n", second_prt_hdr.status, second_prt_hdr.imageSize);
  
            /* Check if need to read RNWF11 flash to SPI flash */
            EXT_FLASH_Initilize(&fact_prt_hdr);
            
            OTA_FailSafe_Mechanism(&fact_prt_hdr, &first_prt_hdr, &second_prt_hdr);
            
            /* Initialize RNWF Module */
            SYS_RNWF_IF_Init();
            
            g_appData.state = APP_STATE_GET_DEV_INFO;

            break;
        }
        
        /* Get RNWF device Information */
        case APP_STATE_GET_DEV_INFO:
        {
            
            if (SYS_RNWF_SYSTEM_SrvCtrl( SYS_RNWF_SYSTEM_SW_REV, g_appBuf) != SYS_RNWF_PASS)
            {
                SYS_CONSOLE_PRINT("Software Revision -> %s\r\n",app_buf);
        
            }
            else
            {
                if(g_appBuf[0] == '\0')
                {
                    SYS_CONSOLE_PRINT("ERROR : No RNWF11 module found\r\n\tConnect RNWF11 module to EXT1 and reset\r\n");
                    g_appData.state = APP_STATE_ERROR;
                    break;
                }
                
                SYS_CONSOLE_PRINT("Software Revision: %s\r\n",g_appBuf);
            }

            /* Get RNWF device Wi-Fi Information*/
            SYS_RNWF_SYSTEM_SrvCtrl(SYS_RWWF_SYSTEM_GET_WIFI_INFO, g_appBuf);
            SYS_CONSOLE_PRINT("%s\r\n\n", g_appBuf);
            
            g_appData.state = APP_STATE_REGISTER_CALLBACK;
            break;
        }
        
        /* Register the Callbacks with Services */
        case APP_STATE_REGISTER_CALLBACK:
        {
            SYS_CONSOLE_PRINT("Connecting to AP -> %s\r\n",SYS_RNWF_WIFI_STA_SSID);
            SYS_RNWF_WIFI_SrvCtrl(SYS_RNWF_WIFI_SET_CALLBACK, SYS_RNWF_WIFI_CallbackHandler);  
    
            SYS_RNWF_WIFI_PARAM_t wifi_sta_cfg = {SYS_RNWF_WIFI_MODE_STA, SYS_RNWF_WIFI_STA_SSID, SYS_RNWF_WIFI_STA_PWD, SYS_RNWF_STA_SECURITY, 1};
            SYS_RNWF_WIFI_SrvCtrl(SYS_RNWF_SET_WIFI_PARAMS, &wifi_sta_cfg);
    
            SYS_RNWF_OTA_SrvCtrl(SYS_RNWF_OTA_SET_CALLBACK, (void *)SYS_RNWF_OTA_CallbackHandler);
            
            g_appData.state = APP_STATE_IDLE;
            break;
        }

        /* Application Error State */
        case APP_STATE_ERROR:
        {
            SYS_CONSOLE_PRINT("ERROR : APP Error\r\n");
            g_appData.state = APP_STATE_IDLE;
            break;
        }
        
        /* Application Idle state */
        case APP_STATE_IDLE:
        {
            if ( SWITCH_Get() == SWITCH_STATE_PRESSED )
            {
                switch_counter ++;
                if (switch_counter > SST26_FLASH_ERASE_TIMEOUT_COUNTER )
                {
                    SYS_CONSOLE_PRINT("Erase external flash...\r\n");
                    SYS_RNWF_OTA_FlashErase();
                    switch_counter = 0;
                }
            }
            else
            {
                switch_counter = 0;
            }
            SYS_RNWF_IF_EventHandler();
            break;
        }
        
        /* Default state */
        default:
        {
            break;
        }
    }
    
    /* Console Tasks */
    SYS_CONSOLE_Tasks(sysObj.sysConsole0);

}

/*******************************************************************************
 End of File
 */
