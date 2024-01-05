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
#include "rnwf_net_service.h"
#include "rnwf_ota_service.h"
#ifdef RNWF11_SERVICE
#include "../reset/rstctrl.h"
#include "../../SST26VF064B_functions.h"
#endif

/*
    Main application
*/

#ifdef RNWF11_SERVICE
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

#define ENTER_BTL_CMD_TIMEOUT       3000

#define OTA_IMAGE_WITHOUT_FLFS_SIZE  0xDF000

typedef struct Partition {
    unsigned char status;
    uint32_t imageSize;
} PARTITION_HDR;

uint32_t pb2_counter = 0;
bool valid_img = false;
#endif

extern uint8_t app_buf[OTA_BUF_LEN_MAX];

uint32_t gOta_file_size = 0;

#ifdef RNWF11_SERVICE
void APP_SW_RESET_Handler(void)
{
    if (pb2_counter > 60)
    {
        pb2_counter = 0;
    }
    else
    {
        RSTCTRL_reset();    
    }
}






unsigned char Generate_CheckSum(unsigned char *data, int size)
{
    unsigned char checksum = 0;
    
    for (int i = 0; i < size; i++) {
        checksum += data[i];
    }
    checksum = ~checksum; // one's complement of the sum
    
    return checksum;
}

void SST26_Sector_Erase(uint32_t dest)
{
    WREN();
    Small_Sector_Erase(dest);
    Wait_Busy();
}

void SST26_Chip_Erase(void)
{
    WREN();
    Chip_Erase();
    Wait_Busy();
}

int DFU(uint32_t start_address, uint32_t size)
{
    int result = -1;
    gOta_file_size = size;

    result = APP_OTA_Program(start_address);
    
    return result;    
}

int Reset_To_Bootloader(void)
{
    unsigned char value;
    int timeout = ENTER_BTL_CMD_TIMEOUT;
    
    DFU_Reset(); 
    
    while (timeout)
    {
        if(UART2.IsTxReady()) 
        {
            UART2.Write(BL_CMD_ENTER_BTL); 
            while(!UART2.IsTxDone());
        }
        if(UART2.IsRxReady())
        {      
            value = UART2.Read();
            if (value == BL_RESP_OK)
            {
                printf("[%s] success entering bootloader..\r\n",__func__);
                return APP_FUNC_RESULT_SUCCESS;
            }
        }      
        DELAY_milliseconds(200);
        timeout -= 200;
    }
    
            
    return APP_FUNC_RESULT_FAIL;
}

void EXT_FLASH_Initilize(PARTITION_HDR * fact_prt_hdr)
{
    if (PARTITION_HEADER_STATUS_FACTORY != fact_prt_hdr->status)
    {
        printf("Copy RN module FW to factory partition, Wait..\r\n");            
        Read_RNWF11_FLASH_TO_EXT_FLASH();
        printf("Complete.\r\n");  
    }
    return;
}

int Recover_RNWF11(PARTITION_HDR * fact_prt_hdr, PARTITION_HDR * first_prt_hdr, PARTITION_HDR * second_prt_hdr)
{
    int result = APP_FUNC_RESULT_FAIL;

    if (first_prt_hdr->status == PARTITION_HEADER_STATUS_OLD)
    {
        if (Reset_To_Bootloader() == APP_FUNC_RESULT_SUCCESS)
        {
            printf("Recover to partition 1\r\n"); 
            if (DFU(FIRST_PARTITION_IMG_ADDR, first_prt_hdr->imageSize) == APP_FUNC_RESULT_SUCCESS)
            {                   
                first_prt_hdr->status = PARTITION_HEADER_STATUS_ACTIVE;
                
                SST26_Sector_Erase(FIRST_PARTITION_HDR_ADDR);
                Sector_Program(FIRST_PARTITION_HDR_ADDR, first_prt_hdr, sizeof(*first_prt_hdr));

                SST26_Sector_Erase(SECOND_PARTITION_HDR_ADDR);

                result = APP_FUNC_RESULT_SUCCESS;
            }

        }
    }
    else if (second_prt_hdr->status == PARTITION_HEADER_STATUS_OLD)
    {
        if (Reset_To_Bootloader() == APP_FUNC_RESULT_SUCCESS)
        {
            printf("Recover to partition 2\r\n");   
            if (DFU(SECOND_PARTITION_IMG_ADDR, second_prt_hdr->imageSize) == APP_FUNC_RESULT_SUCCESS)
            {
                second_prt_hdr->status = PARTITION_HEADER_STATUS_ACTIVE;
             
                SST26_Sector_Erase(SECOND_PARTITION_HDR_ADDR);
                Sector_Program(SECOND_PARTITION_HDR_ADDR, second_prt_hdr, sizeof(*second_prt_hdr));

                SST26_Sector_Erase(FIRST_PARTITION_HDR_ADDR);

                result = APP_FUNC_RESULT_SUCCESS;
            }
        }
        
    }
    else if (fact_prt_hdr->status == PARTITION_HEADER_STATUS_FACTORY)
    {
        if (Reset_To_Bootloader() == APP_FUNC_RESULT_SUCCESS)
        {
            printf("Recover to factory partition\r\n"); 
            if (DFU(FACTORY_PARTITION_IMG_ADDR, fact_prt_hdr->imageSize) == APP_FUNC_RESULT_SUCCESS)
            {
                SST26_Sector_Erase(FIRST_PARTITION_HDR_ADDR);
                SST26_Sector_Erase(SECOND_PARTITION_HDR_ADDR);

                result = APP_FUNC_RESULT_SUCCESS;
            }
        }

    }  

    return result;

}

void OTA_FailSafe_Mechanism(PARTITION_HDR * fact_prt_hdr, PARTITION_HDR * first_prt_hdr, PARTITION_HDR * second_prt_hdr)
{

    if (PARTITION_HEADER_STATUS_OTA == first_prt_hdr->status)
    {
        if (Recover_RNWF11(fact_prt_hdr, first_prt_hdr, second_prt_hdr) == APP_FUNC_RESULT_SUCCESS)     
        {
            printf("Change Partition status to unknown\r\n");
            first_prt_hdr->status = PARTITION_HEADER_STATUS_UNKNOWN;

            SST26_Sector_Erase(FIRST_PARTITION_HDR_ADDR);     
            Sector_Program(FIRST_PARTITION_HDR_ADDR, first_prt_hdr, sizeof(*first_prt_hdr));

            APP_SW_RESET_Handler();
        }
        else
        {
            printf("Fail to recover..\r\n");
        }

    }
    
    
    if (PARTITION_HEADER_STATUS_OTA == second_prt_hdr->status)
    {
        if (Recover_RNWF11(fact_prt_hdr, first_prt_hdr, second_prt_hdr) == APP_FUNC_RESULT_SUCCESS)  
        {
            printf("Change Partition2 status to unknown\r\n");
            second_prt_hdr->status = PARTITION_HEADER_STATUS_UNKNOWN;

            SST26_Sector_Erase(SECOND_PARTITION_HDR_ADDR);    
            Sector_Program(SECOND_PARTITION_HDR_ADDR, second_prt_hdr, sizeof(*second_prt_hdr));

            APP_SW_RESET_Handler();

        }
        else
        {
            printf("Fail to recover..\r\n");
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
    
    SST26_Chip_Erase();

    while (size_to_read > 0)
    {
        if (RNWF_CMD_SEND_OK_WAIT(NULL, NULL, "AT+READAPPIMAGE=0x%lx,%d\r\n", src_addr, SST26_PAGE_SIZE) == RNWF_RAW)
        {
            result = RNWF_RAW_Read(buffer, SST26_PAGE_SIZE + 1);    // add 1 for checksum
            //printf("result = %d, chksum = 0x%x\r\n", result, buffer[SST26_PAGE_SIZE]);

            checksum = Generate_CheckSum(buffer, SST26_PAGE_SIZE);
            
            if (checksum != buffer[SST26_PAGE_SIZE])
            {
                printf("chksum fail...\r\n");
                return;
            }
            
            if ( result != (SST26_PAGE_SIZE + 1) )
            {
                printf("program fail...\r\n");
                return;
            }
  
            Sector_Program(des_addr_fact_part, buffer, SST26_PAGE_SIZE);

            /* erase and program the image to external flash first partition */
            if (des_addr_1st_part % SST26_SECTOR_SIZE == 0)
            {
                SST26_Sector_Erase(des_addr_1st_part);
            }
            Sector_Program(des_addr_1st_part, buffer, SST26_PAGE_SIZE);

        }
        size_to_read -= SST26_PAGE_SIZE;
        src_addr += SST26_PAGE_SIZE;
        des_addr_fact_part += SST26_PAGE_SIZE;
        des_addr_1st_part += SST26_PAGE_SIZE;       
    }
      
    SST26_Sector_Erase(FACTORY_PARTITION_HDR_ADDR);
    SST26_Sector_Erase(FIRST_PARTITION_HDR_ADDR);
    
    /* update the partition header */
    PARTITION_HDR partition_header;
    partition_header.status = PARTITION_HEADER_STATUS_FACTORY;
    partition_header.imageSize = RNWF11_IMAGE_SIZE;  
    Sector_Program(FACTORY_PARTITION_HDR_ADDR, &partition_header, sizeof(partition_header));
    
    partition_header.status = PARTITION_HEADER_STATUS_ACTIVE;
    partition_header.imageSize = RNWF11_IMAGE_SIZE;
    Sector_Program(FIRST_PARTITION_HDR_ADDR, &partition_header, sizeof(partition_header));
}
#endif

#ifdef RNWF11_SERVICE
int APP_OTA_Program(uint32_t flash_addr)
{    
    RNWF_OTA_CHUNK_t ota_chunk = { .chunk_addr = 0x90000000, .chunk_ptr = app_buf, .chunk_size = 0x100000};

    DBG_MSG_OTA("start: flash_addr = 0x%lx\r\n", flash_addr);
    if (gOta_file_size == OTA_IMAGE_WITHOUT_FLFS_SIZE)
        ota_chunk.chunk_size = gOta_file_size;
    
    if (RNWF_OTA_SrvCtrl(RNWF_OTA_DFU_INIT, (void *)NULL) == RNWF_FAIL)
    {
        DBG_MSG_OTA("Fail to enter bootloader mode\r\n");
        return APP_FUNC_RESULT_FAIL;
    }
    while(RNWF_OTA_SrvCtrl(RNWF_OTA_DFU_ERASE, (void *)&ota_chunk.chunk_addr) != RNWF_PASS);
    while(gOta_file_size)
    {                 

        ota_chunk.chunk_size = OTA_BUF_LEN_MAX;  

        HighSpeed_Read_Cont(flash_addr, ota_chunk.chunk_size, (char *)app_buf); 
        if (RNWF_OTA_SrvCtrl(RNWF_OTA_DFU_WRITE, (void *)&ota_chunk) != RNWF_PASS)
        {
            return APP_FUNC_RESULT_FAIL;
        }
        if (gOta_file_size < OTA_BUF_LEN_MAX)
        {
            gOta_file_size = 0;
        }
        else
        {
            gOta_file_size -= ota_chunk.chunk_size;
        }
        ota_chunk.chunk_addr += ota_chunk.chunk_size;
        flash_addr += ota_chunk.chunk_size;
        DBG_MSG_OTA("Flashed %lu bytes\r\n", gOta_file_size);  
    }
    
    if (RNWF_OTA_SrvCtrl(RNWF_OTA_DFU_RESET, (void *)NULL) != RNWF_PASS)
    {
        return APP_FUNC_RESULT_FAIL;
    }
    DELAY_milliseconds(2000);
    return APP_FUNC_RESULT_SUCCESS;
}
#endif

#ifdef RNWF11_SERVICE
void APP_OTA_Callback(RNWF_OTA_EVENT_t event, void *p_str)
{
    static uint32_t flash_addr = OTA_FLASH_IMAGE_START;
    static uint32_t flash_partition_addr = 0;
    static uint32_t flash_partition_hdr_addr = 0;
    PARTITION_HDR partition_header;
    uint32_t erase_addr;
    int i = 0;
    
    switch(event)
    {
        case RNWF_EVENT_MAKE_UART:
            break;
        case RNWF_EVENT_DWLD_START:
        {
            DBG_MSG_OTA("Total Size = %lu\r\n", *(uint32_t *)p_str); 

            /* determine the starting address of the external flash */   
            flash_partition_addr = 0;
            flash_partition_hdr_addr = 0;
            Read_Cont(FIRST_PARTITION_HDR_ADDR, sizeof(partition_header), &partition_header);
            if (partition_header.status != PARTITION_HEADER_STATUS_ACTIVE)
            {
                flash_partition_addr = FIRST_PARTITION_IMG_ADDR;
                flash_partition_hdr_addr = FIRST_PARTITION_HDR_ADDR;
                flash_addr = FIRST_PARTITION_IMG_ADDR;
                DBG_MSG_OTA("partition 1 is used for OTA\r\n"); 
            }
            else 
            {
                Read_Cont(SECOND_PARTITION_HDR_ADDR, sizeof(partition_header), &partition_header);
                if (partition_header.status !=  PARTITION_HEADER_STATUS_ACTIVE)
                {
                    flash_partition_addr = SECOND_PARTITION_IMG_ADDR;
                    flash_partition_hdr_addr = SECOND_PARTITION_HDR_ADDR;
                    flash_addr = SECOND_PARTITION_IMG_ADDR;
                    DBG_MSG_OTA("partition 2 is used for OTA\r\n");
                }
                else
                {
                    DBG_MSG_OTA("No available partition is found for OTA\r\n");
                }          
            }
            
            if (flash_partition_addr)
            {          
                DBG_MSG_OTA("erase partition addr = 0x%lx\r\n", flash_partition_addr);

                /* update partition header */
                PARTITION_HDR partition_header;
                
                SST26_Sector_Erase(flash_partition_hdr_addr);
                Read_Cont(flash_partition_hdr_addr, sizeof(partition_header), &partition_header);
                partition_header.status = PARTITION_HEADER_STATUS_UNKNOWN;
                 
                Sector_Program(flash_partition_hdr_addr, &partition_header, sizeof(partition_header));

                /* erase the used partition */
                erase_addr = flash_partition_addr;
                for (i = 0; i < (1024/4); i++)
                {
                    SST26_Sector_Erase(erase_addr);
                    erase_addr = erase_addr + 4096;
                }
            }
        }
        break;
        case RNWF_EVENT_DWLD_DONE:
        {                                                
            gOta_file_size = *(uint32_t *)p_str;  
            DBG_MSG_OTA("Download Success!= %lu bytes\r\n", gOta_file_size);

            if (flash_partition_addr)
            {
                /* update the partition header */
                if (flash_partition_addr == FIRST_PARTITION_IMG_ADDR)
                {
                    Read_Cont(SECOND_PARTITION_HDR_ADDR, sizeof(partition_header), &partition_header);
                    if (partition_header.status == PARTITION_HEADER_STATUS_ACTIVE)
                    {
                        partition_header.status = PARTITION_HEADER_STATUS_OLD;
                        SST26_Sector_Erase(SECOND_PARTITION_HDR_ADDR);
                        Sector_Program(SECOND_PARTITION_HDR_ADDR, &partition_header, sizeof(partition_header));
                    }
                }
                else if (flash_partition_addr == SECOND_PARTITION_IMG_ADDR)
                {
                    Read_Cont(FIRST_PARTITION_HDR_ADDR, sizeof(partition_header), &partition_header);
                    if (partition_header.status == PARTITION_HEADER_STATUS_ACTIVE)
                    {
                        partition_header.status = PARTITION_HEADER_STATUS_OLD;
                        SST26_Sector_Erase(FIRST_PARTITION_HDR_ADDR);
                        Sector_Program(FIRST_PARTITION_HDR_ADDR, &partition_header, sizeof(partition_header));
                    }
                }

                SST26_Sector_Erase(flash_partition_hdr_addr);
                
                /* update partition header */
                PARTITION_HDR partition_header;
                Read_Cont(flash_partition_hdr_addr, sizeof(partition_header), &partition_header);

                partition_header.status = PARTITION_HEADER_STATUS_OTA;
                partition_header.imageSize = gOta_file_size; 
                 
                Sector_Program(flash_partition_hdr_addr, &partition_header, sizeof(partition_header));   
            }


            /* program the module firwmare */
            if (flash_partition_addr != 0)
            {
                if (APP_OTA_Program(flash_partition_addr) == APP_FUNC_RESULT_SUCCESS)
                {
                    /* update the partition header */
                    SST26_Sector_Erase(flash_partition_hdr_addr);
                    
                    PARTITION_HDR partition_header;
                    partition_header.status = PARTITION_HEADER_STATUS_ACTIVE;
        
                    Sector_Program(flash_partition_hdr_addr, &partition_header, sizeof(partition_header));
                }
            }
            else
            {
                DBG_MSG_OTA("No available partition is found for OTA\r\n");
            }

    
            APP_SW_RESET_Handler();
        }
        break;        
        case RNWF_EVENT_FILE_CHUNK:
        {
            if (flash_partition_addr)
            {
                volatile RNWF_OTA_CHUNK_t *ota_chunk = (RNWF_OTA_CHUNK_t *)p_str;     
                Sector_Program(flash_addr, ota_chunk->chunk_ptr, ota_chunk->chunk_size);     

                flash_addr += ota_chunk->chunk_size;
            }
        }    
        break;                    
        default:
            break;
    }
    
}
#endif

void APP_WIFI_Callback(RNWF_WIFI_EVENT_t event, uint8_t *p_str)
{
    static ota_enable = 0;
    switch(event)
    {
        case RNWF_CONNECTED:
        {
            printf("Wi-Fi Connected\n");
            break;
        }
        case RNWF_DISCONNECTED:
        {
            printf("Wi-Fi Disconnected\nReconnecting... \n");
            RNWF_WIFI_SrvCtrl(RNWF_STA_CONNECT, NULL);
            break;
        }
        case RNWF_DHCP_DONE:
        {
            printf("DHCP IP:%s\r\n", &p_str[2]);    
            if (ota_enable == 0)  // avoid multiple invoke OTA enable twice if the device get both IPv4 and IPv6 IP addr 
            {
                // Enable OTA by passing the OTA buffer space
                if(RNWF_OTA_SrvCtrl(RNWF_OTA_ENABLE, (void *)app_buf) == RNWF_PASS)
                {
                    printf("Successfully Enabled the OTA\r\n");
                }
                else
                {
                printf("Failed to enable the OTA\r\n");
                }
            }
            ota_enable = 1;
            break;
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
#ifdef RNWF11_SERVICE   
    PARTITION_HDR fact_prt_hdr, first_prt_hdr, second_prt_hdr;
    
    PB2_SetInterruptHandler(APP_SW_RESET_Handler);
    
    Read_Cont(FACTORY_PARTITION_HDR_ADDR, sizeof(fact_prt_hdr), &fact_prt_hdr);
    Read_Cont(FIRST_PARTITION_HDR_ADDR, sizeof(first_prt_hdr), &first_prt_hdr);
    Read_Cont(SECOND_PARTITION_HDR_ADDR, sizeof(second_prt_hdr), &second_prt_hdr);
    
    printf("\r\nSPI Flash:\r\n");
    printf("Factory prt: status = %d, size = 0x%lx\r\n", fact_prt_hdr.status, fact_prt_hdr.imageSize);
    printf("1st prt    : status = %d, size = 0x%lx\r\n", first_prt_hdr.status, first_prt_hdr.imageSize);
    printf("2nd prt    : status = %d, size = 0x%lx\r\n\r\n", second_prt_hdr.status, second_prt_hdr.imageSize);

    /* Check if need to read RNWF11 flash to SPI flash */
    EXT_FLASH_Initilize(&fact_prt_hdr);

    OTA_FailSafe_Mechanism(&fact_prt_hdr, &first_prt_hdr, &second_prt_hdr);
#endif    
    /* Wi-Fii Connectivity */
    RNWF_WIFI_PARAM_t wifi_sta_cfg = {RNWF_WIFI_MODE_STA, HOME_AP_SSID, HOME_AP_PASSPHRASE, HOME_AP_SECURITY, 1};    
    printf("Connecting to %s\r\n", HOME_AP_SSID);
    RNWF_WIFI_SrvCtrl(RNWF_WIFI_SET_CALLBACK, APP_WIFI_Callback);
    RNWF_WIFI_SrvCtrl(RNWF_SET_WIFI_PARAMS, &wifi_sta_cfg);

    /* RNWF Application Callback register */
    RNWF_OTA_SrvCtrl(RNWF_OTA_SET_CALLBACK, (void *)APP_OTA_Callback);
    

    while(1)
    {   
#ifdef RNWF11_SERVICE        
        if (IO_PB2_GetValue() == 0)
        {
            pb2_counter++;
            DELAY_milliseconds(100);
        }
        if (pb2_counter > 60)
        {
            printf("Erase external flash..\r\n");
            SST26_Chip_Erase();
        }
#endif        
        RNWF_EVENT_Handler();
    }      
}