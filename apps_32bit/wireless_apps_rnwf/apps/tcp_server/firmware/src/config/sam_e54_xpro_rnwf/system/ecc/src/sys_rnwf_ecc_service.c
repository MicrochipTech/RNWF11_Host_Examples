/*******************************************************************************
  RNWF Host Assisted Net Service Implementation

  File Name:
    sys_rnwf_ecc_service.c

  Summary:
    Source code for the RNWF Host Assisted ECC608 Service implementation.

  Description:
    This file contains the source code for the RNWF Host Assisted ECC608 Service
    implementation.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (C) 2024 released Microchip Technology Inc.  All rights reserved.

 
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

/*! \mainpage RNWF Service API User Guide

The RNWF AT Command service layer wraps the ASCII
command implementation in a service architecture. The
service layer API's are documented here can enable 
easy and quick applciation development.

- \ref SERVICE_GRP "System Service"
- \ref WIFI_GRP "Wi-Fi Service"
- \ref WIFI_PROV_GRP "Wi-Fi Provisioning Service"
- \ref MQTT_GRP "MQTT Service"
- \ref NETSOCK_GRP "Network Socket Service"
- \ref OTA_GRP "OTA Service"
- \ref ECC_GRP "ECC Service"
*/

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>

/* This section lists the other files that are included in this file.
 */
#include "system/inf/sys_rnwf_interface.h"
#include "system/ecc/sys_rnwf_ecc_service.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

char cert_buffer[1500] = {0};

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/* This function is used for ECC608 service Control*/
SYS_RNWF_RESULT_t SYS_RNWF_ECC_SrvCtrl(SYS_RNWF_ECC_SERVICE_t request, void *input) {
    SYS_RNWF_RESULT_t result = SYS_RNWF_FAIL;
    switch(request)
    {
        case SYS_RNWF_ECC_RD_SER:
        {
            *(uint8_t*)input = '\0';
            result = SYS_RNWF_CMD_SEND_OK_WAIT("+ECCRDSER:", input, SYS_RNWF_ECC_READ_SERIAL_NUM);
        }
        break;
        
        case SYS_RNWF_ECC_WR_DEV_TYPE:
        {
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, SYS_RNWF_ECC_SET_DEVICE_TYPE,input);
        }
		break;
        
        case SYS_RNWF_ECC_RD_CERT:
        {
            char *tok;
            SYS_RNWF_ECC_CERT_CFG_t *cert_cfg = (SYS_RNWF_ECC_CERT_CFG_t *)input;
            result = SYS_RNWF_CMD_SEND_OK_WAIT("+ECCRDCERT:", (uint8_t *)cert_buffer, SYS_RNWF_ECC_READ_CERT, cert_cfg->cert_type, cert_cfg->length);
            sscanf(cert_buffer, "%hu,%hu", &cert_cfg->bytes_read, &cert_cfg->remaining_bytes);
            tok = strtok(cert_buffer, ",");
            tok = strtok(NULL, ",");
            tok = strtok(NULL, ",");
            strcpy(cert_cfg->cert, tok);            
        }
        break;
        
        case SYS_RNWF_ECC_WR_SLOT:
        {
            SYS_RNWF_ECC_SLOT_CFG_t *slot_cfg = (SYS_RNWF_ECC_SLOT_CFG_t *)input;
            char    slot_data[424] = {0};

            slot_data[0] = '"';
            strcpy(&slot_data[1], slot_cfg->data);
            slot_data[slot_cfg->length+1] = '"';
            slot_cfg->length = slot_cfg->length - (slot_cfg->length%4) + 4;
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, SYS_RNWF_ECC_WRITE_SLOT, slot_cfg->slot_num, slot_cfg->offset, slot_cfg->length, slot_data);            
        }
        break;
        
        case SYS_RNWF_ECC_WR_ZONE_SLOT:
        {
            SYS_RNWF_ECC_SLOT_CFG_t *slot_cfg = (SYS_RNWF_ECC_SLOT_CFG_t *)input;
            char    slot_data[424] = {0};

            slot_data[0] = '"';
            strcpy(&slot_data[1], slot_cfg->data);
            slot_data[slot_cfg->length+1] = '"';
            slot_cfg->length = slot_cfg->length - (slot_cfg->length%4) + 4;
            result = SYS_RNWF_CMD_SEND_OK_WAIT(NULL, NULL, SYS_RNWF_ECC_WRITE_ZONE_SLOT, slot_cfg->zone_type, slot_cfg->slot_num, slot_cfg->offset, slot_cfg->length, slot_cfg->data);
        }
        break;
      
        case SYS_RNWF_ECC_RD_SLOT:
        {
            SYS_RNWF_ECC_SLOT_CFG_t *slot_cfg = (SYS_RNWF_ECC_SLOT_CFG_t *)input;
            result = SYS_RNWF_CMD_SEND_OK_WAIT("+ECCRDCERT:", (uint8_t *)(slot_cfg->data), SYS_RNWF_ECC_READ_SLOT, slot_cfg->slot_num, slot_cfg->offset, slot_cfg->length);
        }
        break;
        
        case SYS_RNWF_ECC_RD_ZONE_SLOT:
        {
            SYS_RNWF_ECC_SLOT_CFG_t *slot_cfg = (SYS_RNWF_ECC_SLOT_CFG_t *)input;
            result = SYS_RNWF_CMD_SEND_OK_WAIT("+ECCRDCERT:", (uint8_t *)(slot_cfg->data), SYS_RNWF_ECC_READ_ZONE_SLOT, slot_cfg->zone_type, slot_cfg->slot_num, slot_cfg->offset, slot_cfg->length);
        }
        break;

        default:
            break;
    }
    return result;
}

/* *****************************************************************************
 End of File
 */
