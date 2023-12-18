/*
 * MAIN Generated Driver File
 * 
 * @file rnwf_ecc_service.h
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



/*! \page system_service System Service Layer
This page introduces the user to the topic.
*/


// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef RNWF_ECC_SERVICE_H
#define	RNWF_ECC_SERVICE_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define RNWF_ECC_SET_DEVICE_TYPE     	"AT+ECCWRDEVTYPE=%d\r\n"
#define RNWF_ECC_READ_SERIAL_NUM     	"AT+ECCRDSER\r\n"
#define RNWF_ECC_READ_CERT     			"AT+ECCRDCERT=%d,%d\r\n"
#define RNWF_ECC_WRITE_ZONE_SLOT     	"AT+ECCWRSLOT=%d,%d,%d,%d,%s\r\n"
#define RNWF_ECC_WRITE_SLOT     		"AT+ECCWRSLOT=%d,%d,%d,%s\r\n"
#define RNWF_ECC_READ_ZONE_SLOT     	"AT+ECCRDSLOT=%d,%d,%d,%d\r\n"
#define RNWF_ECC_READ_SLOT     			"AT+ECCRDSLOT=%d,%d,%d\r\n"

#ifdef RNWF11_SERVICE
#define DBG_MSG_SYSTEM(args, ...)      printf("[SYSTEM]:"args, ##__VA_ARGS__)
#endif
/**
 @defgroup ECC_GRP ECC Service API
 @{
 */

/**
 @brief Network and Socket service List
 
 */
typedef struct
{
    uint8_t     cert_type;          /**<Cert Type - Device, Signer, Root */
    uint16_t    length;             /**<Num of Bytes of Cert to read */
    uint16_t    bytes_read;         /**<No of Bytes read from the cert */
    uint16_t    remaining_bytes;    /**<Remaining bytes to read in the cert */
    char        cert[1500];         /**<Cert that is read into it */
}RNWF_ECC_CERT_CFG_t;

typedef struct
{
    uint8_t     zone_type;          /**<Zone Type - Config, OTP, Data*/
    uint16_t    slot_num;           /**<Slot Number */
    uint16_t    offset;             /**<Offset from where to read/ write */
    uint16_t    length;             /**<bytes to be read/ written */
    char        data[512];          /**<Data Buffer */
}RNWF_ECC_SLOT_CFG_t;

/**
 @brief System Service List
 
 */
typedef enum{
    RNWF_ECC_RD_SER,            /**<Read Serial Number */
    RNWF_ECC_RD_CERT,           /**<Read Certificate */
    RNWF_ECC_WR_DEV_TYPE,       /**<Write Device Type */
    RNWF_ECC_WR_SLOT,        	/**<Write into Slot */
    RNWF_ECC_WR_ZONE_SLOT,      /**<Write into Slot (Giving Zone info also) */
    RNWF_ECC_RD_ZONE_SLOT,      /**<Read from Slot (Giving Zone info also) */
    RNWF_ECC_RD_SLOT,        	/**<Read from Slot */
}RNWF_ECC_SERVICE_t;


/**
 * @brief ECC Service Layer API to handle ECC operations.
 * 
 *
 * @param[in] request   Service request ::RNWF_ECC_SERVICE_t
 * @param[in] input     Parameters for the requested service
 * 
 * @return ::RNWF_PASS Requested service is handled successfully
 * @return ::RNWF_FAIL Requested service has failed
 */
RNWF_RESULT_t RNWF_ECC_SrvCtrl(RNWF_ECC_SERVICE_t request, void *input);

#endif	/* RNWF_ECC_SERVICE_H */

/** @}*/
