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
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/rnwf/rnwf_app.h"

#ifdef RNWF11_SERVICE
#include "mcc_generated_files/reset/rstctrl.h"
#include "mcc_generated_files/rnwf/rnwf_system_service.h"
#include "mcc_generated_files/rnwf/rnwf_ecc_service.h"
#endif

/*
    Main application
*/

#ifdef RNWF11_SERVICE
/* Application buffer */
extern uint8_t app_buf[APP_BUFFER_SIZE_MAX];

void APP_RESET_Device(void)
{        
    printf("Device is rebooting...!\r\n");
    DELAY_milliseconds(500);
    RSTCTRL_reset();
}

void APP_LED_Handler(void)
{ 
    LED_Toggle();    
}
#endif

int main(void)
{

    SYSTEM_Initialize();

#ifdef RNWF11_SERVICE
    RNWF_RESULT_t res;
    PB2_SetInterruptHandler(APP_SW_Handler);
    TCA0_Interface.TimeoutCallbackRegister(APP_SYS_Tick);     
    TCA1_Interface.TimeoutCallbackRegister(APP_LED_Handler); 

    res = RNWF_SYSTEM_Module_Init();
    if (res != RNWF_PASS)
        printf("RNWF_SYSTEM_Module_Init, fail..");
#endif
    // Main Section();
    RNWF_IF_Init();

#ifdef RNWF11_SERVICE
    printf("%s", "##################################\n");
    printf("%s", "  Welcome RNWF11 Azure Cloud Demo  \n");
    printf("%s", "##################################\n");    

    RNWF_SYSTEM_SrvCtrl(RWWF_SYSTEM_GET_WIFI_INFO, app_buf);    
    printf("\r\nWi-Fi Info:- \r\n%s\n", app_buf);
    
    RNWF_SYSTEM_SrvCtrl(RNWF_SYSTEM_GET_CERT_LIST, app_buf);    
    printf("\r\nCerts on RNWF11:- \r\n%s\n", app_buf);
    
    RNWF_SYSTEM_SrvCtrl(RNWF_SYSTEM_GET_KEY_LIST, app_buf);    
    printf("\r\nKeys on RNWF11:- \r\n%s\n", app_buf);
    
    RNWF_SYSTEM_SrvCtrl(RNWF_SYSTEM_SW_REV, app_buf);    
    printf("\r\n%s\n", app_buf);
#endif
#ifndef RNWF11_SERVICE
    RNWF_SYSTEM_SrvCtrl(RNWF_SYSTEM_DEV_INFO, app_buf);    
    printf("%s\n", app_buf);
#endif

#ifdef RNWF11_SERVICE
    RNWF_ECC_SrvCtrl(RNWF_ECC_RD_SER, app_buf);
	char * token = strtok((char *)app_buf, "\"");
    token = strtok(NULL, "\"");
    printf("\r\nSerial number:- sn%s\n", token);

    RNWF_ECC_CERT_CFG_t cert_cfg_dev[] = {1, 1500};
    RNWF_ECC_SrvCtrl(RNWF_ECC_RD_CERT, cert_cfg_dev);
    printf("\r\nDevice certificate:\n");
    for(int i = 1; cert_cfg_dev->cert[i] != '\0'; i++)
    {
        if((cert_cfg_dev->cert[i] == '\\' && cert_cfg_dev->cert[++i] == 'n') ||
            (cert_cfg_dev->cert[i] == '"'))
        {
            printf("\r\n");

        }
        else
        {
            printf("%c",cert_cfg_dev->cert[i]);
        }
    }

    RNWF_ECC_CERT_CFG_t cert_cfg_sig[] = {2, 1500};
    RNWF_ECC_SrvCtrl(RNWF_ECC_RD_CERT, cert_cfg_sig);
    printf("\r\nSigner certificate:\n");
    for(int i = 1; cert_cfg_sig->cert[i] != '\0'; i++)
    {
        if((cert_cfg_sig->cert[i] == '\\' && cert_cfg_sig->cert[++i] == 'n') ||
            (cert_cfg_sig->cert[i] == '"'))
        {
            printf("\r\n");

        }
        else
        {
            printf("%c",cert_cfg_sig->cert[i]);
        }
    }

#endif

	RNWF_APP_Initialize();    // Note: This function call never returns because of an infinite while-loop

    while(1)
    {
    }    
}