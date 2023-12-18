/*
 * MAIN Generated Driver File
 * 
 * @file rnwf_ecc_service.c
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

/*! \mainpage RNWF11 Service API User Guide

The RNWF11 AT Command service layer wraps the ASCII
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

/* This section lists the other files that are included in this file.
 */
#include <stdio.h>
#include <string.h>

#include "rnwf_interface.h"
#include "rnwf_ecc_service.h"

/* TODO:  Include other files here if needed. */


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
RNWF_RESULT_t RNWF_ECC_SrvCtrl(RNWF_ECC_SERVICE_t request, void *input) {
    RNWF_RESULT_t result = RNWF_FAIL;
    switch(request)
    {
        case RNWF_ECC_RD_SER:
        {
            *(uint8_t*)input = '\0';
            result = RNWF_CMD_SEND_OK_WAIT("+ECCRDSER:", input, RNWF_ECC_READ_SERIAL_NUM);
        }
        break;

        case RNWF_ECC_WR_DEV_TYPE:
        {
            result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_ECC_SET_DEVICE_TYPE,input);
        }
		break;

        case RNWF_ECC_RD_CERT:
        {
            char *tok;
            RNWF_ECC_CERT_CFG_t *cert_cfg = (RNWF_ECC_CERT_CFG_t *)input;
            result = RNWF_CMD_SEND_OK_WAIT("+ECCRDCERT:", cert_buffer, RNWF_ECC_READ_CERT, cert_cfg->cert_type, cert_cfg->length);
            sscanf(cert_buffer, "%lu,%lu,%s", &cert_cfg->bytes_read, &cert_cfg->remaining_bytes);
            tok = strtok(cert_buffer, ",");
            tok = strtok(NULL, ",");
            tok = strtok(NULL, ",");
            strcpy(cert_cfg->cert, tok);
        }
        break;

        case RNWF_ECC_WR_SLOT:
        {
            RNWF_ECC_SLOT_CFG_t *slot_cfg = (RNWF_ECC_SLOT_CFG_t *)input;
            char    slot_data[424] = {0};

            slot_data[0] = '"';
            strcpy(&slot_data[1], slot_cfg->data);
            slot_data[slot_cfg->length+1] = '"';
            slot_cfg->length = slot_cfg->length - (slot_cfg->length%4) + 4;
            result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_ECC_WRITE_SLOT, slot_cfg->slot_num, slot_cfg->offset, slot_cfg->length, slot_data);
        }
        break;

        case RNWF_ECC_WR_ZONE_SLOT:
        {
            RNWF_ECC_SLOT_CFG_t *slot_cfg = (RNWF_ECC_SLOT_CFG_t *)input;
            char    slot_data[424] = {0};

            slot_data[0] = '"';
            strcpy(&slot_data[1], slot_cfg->data);
            slot_data[slot_cfg->length+1] = '"';
            slot_cfg->length = slot_cfg->length - (slot_cfg->length%4) + 4;
            result = RNWF_CMD_SEND_OK_WAIT(NULL, NULL, RNWF_ECC_WRITE_ZONE_SLOT, slot_cfg->zone_type, slot_cfg->slot_num, slot_cfg->offset, slot_cfg->length, slot_cfg->data);
        }
        break;

        case RNWF_ECC_RD_SLOT:
        {
            RNWF_ECC_SLOT_CFG_t *slot_cfg = (RNWF_ECC_SLOT_CFG_t *)input;
            result = RNWF_CMD_SEND_OK_WAIT("+ECCRDCERT:", slot_cfg->data, RNWF_ECC_READ_SLOT, slot_cfg->slot_num, slot_cfg->offset, slot_cfg->length);
        }
        break;

        case RNWF_ECC_RD_ZONE_SLOT:
        {
            RNWF_ECC_SLOT_CFG_t *slot_cfg = (RNWF_ECC_SLOT_CFG_t *)input;
            result = RNWF_CMD_SEND_OK_WAIT("+ECCRDCERT:", slot_cfg->data, RNWF_ECC_READ_ZONE_SLOT, slot_cfg->zone_type, slot_cfg->slot_num, slot_cfg->offset, slot_cfg->length);
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
