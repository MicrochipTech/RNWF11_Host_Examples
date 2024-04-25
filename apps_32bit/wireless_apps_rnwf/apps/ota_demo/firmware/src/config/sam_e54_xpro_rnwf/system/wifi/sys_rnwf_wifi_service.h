/*******************************************************************************
  RNWF Host Assisted Wifi Service Header file 

  File Name:
    sys_rnwf_wifi_service.h

  Summary:
    Header file for the RNWF Host Assisted Wifi Service implementation.

  Description:
    This file contains the header file for the RNWF Host Assisted Wifi Service
    implementation.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (C) 2020 released Microchip Technology Inc.  All rights reserved.

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
#ifndef SYS_RNWF_WIFI_SERVICE_H
#define	SYS_RNWF_WIFI_SERVICE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "system/inf/sys_rnwf_interface.h"

/*RNWF Wi-Fi Commands*/
#define SYS_RNWF_WIFI_GET_STA_STATE     "AT+WSTA\r\n"
#define SYS_RNWF_WIFI_CONNECT           "AT+WSTA=1\r\n"
#define SYS_RNWF_WIFI_DISCONNECT        "AT+WSTA=0\r\n"
#define SYS_RNWF_WIFI_SET_STA_SSID      "AT+WSTAC=1,\"%s\"\r\n"
#define SYS_RNWF_WIFI_SET_STA_SEC       "AT+WSTAC=2,%d\r\n"
#define SYS_RNWF_WIFI_SET_STA_PWD       "AT+WSTAC=3,\"%s\"\r\n"

#define SYS_RNWF_WIFI_SET_STA_CHANNEL   "AT+WSTAC=4,%d\r\n"
#define SYS_RNWF_WIFI_SET_STA_BSSID     "AT+WSTAC=5,%s\r\n"
#define SYS_RNWF_WIFI_SET_STA_TIMEOUT   "AT+WSTAC=7,%d\r\n"

#define SYS_RNWF_WIFI_GET_AP_STATE   "AT+WAP\r\n"
#define SYS_RNWF_WIFI_SOFTAP_ENABLE  "AT+WAP=1\r\n"
#define SYS_RNWF_WIFI_SOFTAP_DISABLE "AT+WAP=0\r\n"
#define SYS_RNWF_WIFI_SET_AP_SSID    "AT+WAPC=1,\"%s\"\r\n"
#define SYS_RNWF_WIFI_SET_AP_SEC     "AT+WAPC=2,%d\r\n"
#define SYS_RNWF_WIFI_SET_AP_PWD     "AT+WAPC=3,\"%s\"\r\n"
#define SYS_RNWF_WIFI_SET_AP_CHANNEL "AT+WAPC=4,%d\r\n"
#define SYS_RNWF_WIFI_SET_AP_HIDDEN  "AT+WAPC=5,%d\r\n"


#define SYS_RNWF_WIFI_PSV_SCAN      "AT+WSCN=0\r\n"
#define SYS_RNWF_WIFI_ACT_SCAN      "AT+WSCN=1\r\n"

#define SYS_RNWF_SSID_LEN_MAX       33
#define SYS_RNWF_BSSID_LEN_MAX      32
#define SYS_RNWF_PWD_LEN_MAX        128

/*Wi-Fi max Callback service*/
#define SYS_RNWF_WIFI_SERVICE_CB_MAX     2



/**
 * @brief Wi-Fi Service List
 * 
 */
typedef enum 
{
    /**<Request/Trigger Wi-Fi connect */
    SYS_RNWF_STA_CONNECT,          
            
    /**<Request/Trigger Wi-Fi disconnect */         
    SYS_RNWF_STA_DISCONNECT,       
            
    /**<Request/Trigger SoftAP disable */        
    SYS_RNWF_AP_DISABLE,           
            
    /**<Configure the Wi-Fi parameters */          
    SYS_RNWF_SET_WIFI_PARAMS,          
            
    /**<Configure the Wi-Fi channel */        
    SYS_RNWF_SET_WIFI_AP_CHANNEL,   
            
    /**<Configure the Access point's BSSID */        
    SYS_RNWF_SET_WIFI_BSSID,        
            
    /**<Configure Wi-Fi connection timeout */        
    SYS_RNWF_SET_WIFI_TIMEOUT,      
            
    /**<Configure Hidden mode SSID in SoftAP mode*/        
    SYS_RNWF_SET_WIFI_HIDDEN,       
            
    /**<Request/Trigger Wi-Fi passive scan */       
    SYS_RNWF_WIFI_PASSIVE_SCAN,     
            
    /**<Request/Trigger Wi-Fi active scan */
    SYS_RNWF_WIFI_ACTIVE_SCAN,      
            
    /**<Regester the call back for async events */        
    SYS_RNWF_WIFI_SET_CALLBACK,     
            
    /**<Get Callback functions data*/        
    SYS_RNWF_WIFI_GET_CALLBACK,
            
    /**<Regester the call back for async events */
    SYS_RNWF_WIFI_SET_SRVC_CALLBACK,     
}SYS_RNWF_WIFI_SERVICE_t;

/**
 * @brief Wi-Fi Event Code List
 * 
 */
typedef enum 
{
    /**<Wi-Fi connected event code*/
    SYS_RNWF_CONNECTED,
            
    /**<Wi-Fi disconnected event code*/
    SYS_RNWF_DISCONNECTED,          
            
    /**<Wi-Fi connection failure event code*/       
    SYS_RNWF_CONNECT_FAILED,        
     
    /**<Wi-Fi DHCP complete event code*/
    SYS_RNWF_DHCP_DONE,             
    
    /**<Scan indication event to report each scan list */
    SYS_RNWF_SCAN_INDICATION, 
            
    /**<Scan complete event code*/
    SYS_RNWF_SCAN_DONE,      
    
    /**<SNTP up event*/
    SYS_RNWF_SNTP_UP,

}SYS_RNWF_WIFI_EVENT_t;

/**
 * @brief Wi-Fi security modes
 * 
 */
typedef enum 
{
    /**<OPEN mode, no security*/
    SYS_RNWF_OPEN,              
           
    /**<RSVD mode*/
    SYS_RNWF_RSVD,                  
            
    /**<WPA2 Mixed mode TKIP/CCMP*/
    SYS_RNWF_WPA2_MIXED,            
            
    /**<WPA2 CCMP*/
    SYS_RNWF_WPA2,                  
            
    /**<WPA3 Transition Mode*/
    SYS_RNWF_WPA3_TRANS,            
            
    /**<WPA3 mode*/
    SYS_RNWF_WPA3,   

}SYS_RNWF_WIFI_SECURITY_t;

/**
 * @brief Wi-Fi operation modes
 * 
 */
typedef enum 
{
    /**<Station (STA) mode of WiFi operation*/
    SYS_RNWF_WIFI_MODE_STA, 
            
    /**<Software Access Point (SoftAP) mode of WiFi operation*/
    SYS_RNWF_WIFI_MODE_AP,

}SYS_RNWF_WIFI_MODE_t;

/**
 * @brief       Wi-Fi operation modes
 * 
 */
typedef struct
{
    /**<Wi-Fi operation mode ::SYS_RNWF_WIFI_MODE_t either STA or SoftAP */
    SYS_RNWF_WIFI_MODE_t mode;      
    
    /**<Wi-Fi SSID of Home AP or SoftAP */
    const char  *ssid;              
    
    /**<Wi-Fi Passphrase of Home AP or SoftAP*/
    const char  *passphrase;   
    
    /**<Wi-Fi Secrity mode ::SYS_RNWF_WIFI_SECURITY_t */
    SYS_RNWF_WIFI_SECURITY_t security;  
    
    /**<Wi-Fi autoconnect, SoftAP */
    uint8_t autoconnect;   
             
}SYS_RNWF_WIFI_PARAM_t;


/**
 * @brief       Wi-Fi callback function type
 * 
 * @param[out]  event One of the ::SYS_RNWF_WIFI_EVENT_t event   
 * @param[out]  msg Received data related to the passed event   
 * 
 */
typedef void (*SYS_RNWF_WIFI_CALLBACK_t)(SYS_RNWF_WIFI_EVENT_t event, uint8_t *msg);

/**
 * @brief Wi-Fi Service Layer API to handle STA and SoftAP mode operations.
 * 
 * @note The asynchronous events are reported through callback, make sure that
 *  the application registers the callback using the ::SYS_RNWF_WIFI_SET_CALLBACK
 *  service request
 *
 * @param[in] request   Service request ::SYS_RNWF_WIFI_SERVICE_t
 * @param[in] input     Parameters for the requested service
 * 
 * @return ::SYS_RNWF_PASS Requested service is handled successfully
 * @return ::SYS_RNWF_FAIL Requested service has failed
 */
SYS_RNWF_RESULT_t SYS_RNWF_WIFI_SrvCtrl( SYS_RNWF_WIFI_SERVICE_t request, void *input);

#endif	/* RNWF_WIFI_SERVICE_H */

/** @}*/