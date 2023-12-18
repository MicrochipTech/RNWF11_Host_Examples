 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
© [2023] Microchip Technology Inc. and its subsidiaries.

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
#include <string.h>
#include <stdlib.h>

#include "SST26VF064B_functions.h"

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/rnwf/rnwf_interface.h"
#include "mcc_generated_files/rnwf/rnwf_wifi_service.h"
#include "mcc_generated_files/rnwf/rnwf_net_service.h"
#include "mcc_generated_files/rnwf/rnwf_system_service.h"
#include "mcc_generated_files/rnwf/rnwf_ota_service.h"


uint8_t app_buf[OTA_BUF_LEN_MAX];
/*
    Main application
*/

int main(void)
{                  
    RNWF_RESULT_t res;
    int Device_Id=0x00;
    int Device_Type=0x00;    
    int Manufacturer_Id=0x00;    
    
    SYSTEM_Initialize();
    
    printf("%s", "##############################################\r\n");
    printf("%s", "  Welcome RNWF11 WiFi Host Assisted OTA Demo  \r\n");
    printf("%s", "##############################################\r\n");            

    Chip_Reset();
    GlobalWriteProtectionUnlock();
    Jedec_ID_Read(&Manufacturer_Id, &Device_Type, &Device_Id); 
    printf("SPI Manufacturer ID = 0x%02X\r\n", Manufacturer_Id);
    printf("SPI Device Type = 0x%02X\r\n", Device_Type);
    printf("SPI Device ID = 0x%02X\r\n", Device_Id);
    
    res = RNWF_SYSTEM_Module_Init();
    if (res != RNWF_PASS)
        printf("RNWF_SYSTEM_Module_Init, fail..");
    
    res = RNWF_IF_Init();
    if (res != RNWF_PASS)
        printf("RNWF_IF_Init, fail..");

    if(RNWF_SYSTEM_SrvCtrl(RNWF_SYSTEM_SW_REV, app_buf) != RNWF_PASS)
    {
        printf("Fail to read sw rev..\r\n");  
    }
    else
    {
        printf("Software Revision: %s\r\n", app_buf);
    }
              
    RNWF_SYSTEM_SrvCtrl(RWWF_SYSTEM_GET_WIFI_INFO, app_buf);    
    printf("%s\n", app_buf);
    
    RNWF_APP_Initialize();    // Note: This function call never returns because of an infinite while-loop

    while(1)
    {
    }
}