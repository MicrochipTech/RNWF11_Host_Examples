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
� [2023] Microchip Technology Inc. and its subsidiaries.

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
#include "mcc_generated_files/rnwf/rnwf_system_service.h"
#include "mcc_generated_files/reset/rstctrl.h"
#endif
/*
    Main application
*/

#ifdef RNWF11_SERVICE
void APP_SW_RESET_Handler(void)
{
    RNWF_SYSTEM_SrvCtrl(RNWF_SYSTEM_RESET, NULL);    
    DELAY_milliseconds(3500);    
    RSTCTRL_reset();    
}
#endif
int main(void)
{
#ifdef RNWF11_SERVICE
    uint8_t man_id[32];
    RNWF_RESULT_t res = RNWF_FAIL;
#endif    
    SYSTEM_Initialize();
    // Main Section();
#ifdef RNWF11_SERVICE    
    res = RNWF_SYSTEM_Module_Init();
    if (res != RNWF_PASS)
        printf("RNWF_SYSTEM_Module_Init, fail..");
#endif

    RNWF_IF_Init();
    
#ifdef RNWF11_SERVICE
    printf("%s", "##################################\n");
    printf("%s", "  Welcome RNWF TCP Client Demo  \n");
    printf("%s", "##################################\n");
        
    PB2_SetInterruptHandler(APP_SW_RESET_Handler);
    
    RNWF_SYSTEM_SrvCtrl(RNWF_SYSTEM_GET_MAN_ID, man_id);    
    printf("Manufacturer = %s\n", man_id);
#endif
	RNWF_APP_Initialize();    // Note: This function call never returns because of an infinite while-loop

    while(1)
    {
    }    
}