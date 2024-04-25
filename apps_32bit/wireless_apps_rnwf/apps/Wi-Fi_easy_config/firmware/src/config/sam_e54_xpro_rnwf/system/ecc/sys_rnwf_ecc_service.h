/*******************************************************************************
  RNWF Host Assisted Net Service Header file 

  File Name:
    sys_rnwf_ecc_service.h

  Summary:
    Header file for the RNWF Host Assisted ECC608 Service implementation.

  Description:
    This file contains the header file for the RNWF Host Assisted ECC608 Service
    implementation.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (C) 2024 released Microchip Technology Inc.  All rights reserved.

 * Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SYS_RNWF_ECC_SERVICE_H
#define	SYS_RNWF_ECC_SERVICE_H

#include <xc.h> // include processor files - each processor file is guarded.  


#define SYS_RNWF_ECC_SET_DEVICE_TYPE     	"AT+ECCWRDEVTYPE=%d\r\n"
#define SYS_RNWF_ECC_READ_SERIAL_NUM     	"AT+ECCRDSER\r\n"
#define SYS_RNWF_ECC_READ_CERT     			"AT+ECCRDCERT=%d,%d\r\n"
#define SYS_RNWF_ECC_WRITE_ZONE_SLOT     	"AT+ECCWRSLOT=%d,%d,%d,%d,%s\r\n"
#define SYS_RNWF_ECC_WRITE_SLOT     		"AT+ECCWRSLOT=%d,%d,%d,%s\r\n"
#define SYS_RNWF_ECC_READ_ZONE_SLOT     	"AT+ECCRDSLOT=%d,%d,%d,%d\r\n"
#define SYS_RNWF_ECC_READ_SLOT     			"AT+ECCRDSLOT=%d,%d,%d\r\n"

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
}SYS_RNWF_ECC_CERT_CFG_t;

typedef struct 
{    
    uint8_t     zone_type;          /**<Zone Type - Config, OTP, Data*/
    uint16_t    slot_num;           /**<Slot Number */
    uint16_t    offset;             /**<Offset from where to read/ write */
    uint16_t    length;             /**<bytes to be read/ written */
    char        data[512];          /**<Data Buffer */
}SYS_RNWF_ECC_SLOT_CFG_t;

/**
 @brief System Service List
 
 */
typedef enum{
    SYS_RNWF_ECC_RD_SER,            /**<Read Serial Number */
    SYS_RNWF_ECC_RD_CERT,           /**<Read Certificate */
    SYS_RNWF_ECC_WR_DEV_TYPE,       /**<Write Device Type */
    SYS_RNWF_ECC_WR_SLOT,        	/**<Write into Slot */
    SYS_RNWF_ECC_WR_ZONE_SLOT,      /**<Write into Slot (Giving Zone info also) */
    SYS_RNWF_ECC_RD_ZONE_SLOT,      /**<Read from Slot (Giving Zone info also) */
    SYS_RNWF_ECC_RD_SLOT,        	/**<Read from Slot */
}SYS_RNWF_ECC_SERVICE_t;


/**
 * @brief ECC Service Layer API to handle ECC operations.
 * 
 *
 * @param[in] request   Service request ::SYS_RNWF_ECC_SERVICE_t
 * @param[in] input     Parameters for the requested service
 * 
 * @return ::SYS_RNWF_PASS Requested service is handled successfully
 * @return ::SYS_RNWF_FAIL Requested service has failed
 */
SYS_RNWF_RESULT_t SYS_RNWF_ECC_SrvCtrl(SYS_RNWF_ECC_SERVICE_t request, void *input);

#endif	/* SYS_RNWF_ECC_SERVICE_H */

/** @}*/
