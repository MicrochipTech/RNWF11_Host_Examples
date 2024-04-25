/*******************************************************************************
  System Configuration Header

  File Name:
    configuration.h

  Summary:
    Build-time configuration header for the system defined by this project.

  Description:
    An MPLAB Project may have multiple configurations.  This file defines the
    build-time options for a single configuration.

  Remarks:
    This configuration header must not define any prototypes or data
    definitions (or include any files that do).  It only provides macro
    definitions for build-time configuration options

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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
// DOM-IGNORE-END

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section Includes other configuration headers necessary to completely
    define this configuration.
*/

#include "user.h"
#include "device.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: System Configuration
// *****************************************************************************
// *****************************************************************************



// *****************************************************************************
// *****************************************************************************
// Section: System Service Configuration
// *****************************************************************************
// *****************************************************************************


#define SYS_RNWF_NET_BIND_TYPE0					SYS_RNWF_BIND_REMOTE 

#define SYS_RNWF_NET_SOCK_PORT0					6666

#define SYS_RNWF_NET_SOCK_TYPE0					SYS_RNWF_SOCK_TCP 

#define SYS_RNWF_NET_SOCK_ADDR0					""

#define SYS_RNWF_TLS_ENABLE0					0


#define SYS_RNWF_NET_SockCallbackHandler  	    APP_SOCKET_Callback
#define SYS_RNWF_MQTT_CLOUD_URL        		"aznb5n71crum1-ats.iot.us-east-1.amazonaws.com"
#define SYS_RNWF_MQTT_CLOUD_PORT			8883

#define SYS_RNWF_MQTT_QOS_TYPE					SYS_RNWF_MQTT_QOS0

#define SYS_RNWF_MQTT_RETAIN_TYPE				SYS_RNWF_NO_RETAIN
#define SYS_RNWF_MQTT_TOPIC_NAME        	    "mchp/rnwf/from"

#define SYS_RNWF_MQTT_CLIENT_ID       			"sn0123B08FE1C8740D01"
#define SYS_RNWF_MQTT_SCOPE_ID       			"0ne00B5478B"
#define SYS_RNWF_MQTT_DEVICE_TEMPLATE			"dtmi:com:Microchip:SAME54_XPRO;1"
#define SYS_RNWF_MQTT_CLOUD_USER_NAME			""
#define SYS_RNWF_MQTT_PASSWORD				""
#define SYS_RNWF_MQTT_AZURE_DPS_ENABLE			0

#define SYS_RNWF_AWS_FMT_TEMP_LIGHT      "{\\\"Temperature (C)\\\": %d,\\\"Light (lux)\\\":%d}"
#define SYS_RNWF_AWS_FMT_SHADOW_LIGHT    "{\\\"state\\\":{\\\"reported\\\":{\\\"toggle\\\": %d}}}"
#define SYS_RNWF_AWS_PUB_SHADOW_UPDATE   "$aws/things/"SYS_RNWF_MQTT_CLIENT_ID"/shadow/update"

#define SYS_RNWF_MQTT_SERVER_CERT            "AmazonRootCA1"
#define SYS_RNWF_MQTT_DEVICE_CERT            ""
#define SYS_RNWF_MQTT_DEVICE_KEY             ""
#define SYS_RNWF_MQTT_DEVICE_KEY_PSK         ""
#define SYS_RNWF_MQTT_TLS_SERVER_NAME        "aznb5n71crum1-ats.iot.us-east-1.amazonaws.com"
#define SYS_RNWF_MQTT_TLS_DOMAIN_NAME        ""

#define SYS_RNWF_MQTT_SUB_TOPIC_0            "iothub/twin/PATCH/properties/desired/#"
#define SYS_RNWF_MQTT_SUB_TOPIC_1            "$iothub/methods/POST/#"
#define SYS_RNWF_MQTT_SUB_TOPIC_2            "$iothub/twin/res/#"


#define SYS_RNWF_MQTT_CallbackHandler        APP_MQTT_Callback

#define RNWF11_SERVICE

/* TIME System Service Configuration Options */
#define SYS_TIME_INDEX_0                            (0)
#define SYS_TIME_MAX_TIMERS                         (5)
#define SYS_TIME_HW_COUNTER_WIDTH                   (16)
#define SYS_TIME_HW_COUNTER_PERIOD                  (0xFFFFU)
#define SYS_TIME_HW_COUNTER_HALF_PERIOD             (SYS_TIME_HW_COUNTER_PERIOD>>1)
#define SYS_TIME_CPU_CLOCK_FREQUENCY                (120000000)
#define SYS_TIME_COMPARE_UPDATE_EXECUTION_CYCLES    (232)


#define SYS_DEBUG_ENABLE
#define SYS_DEBUG_GLOBAL_ERROR_LEVEL       SYS_ERROR_DEBUG
#define SYS_DEBUG_BUFFER_DMA_READY
#define SYS_DEBUG_USE_CONSOLE



#define SYS_RNWF_TLS_USE_ECC608		1

#define SYS_RNWF_TLS_ECC608_DEVTYPE					2 //TFLEX 


#define SYS_CONSOLE_DEVICE_MAX_INSTANCES   			(1U)
#define SYS_CONSOLE_UART_MAX_INSTANCES 	   			(1U)
#define SYS_CONSOLE_USB_CDC_MAX_INSTANCES 	   		(0U)
#define SYS_CONSOLE_PRINT_BUFFER_SIZE        		(1024U)


#define SYS_CONSOLE_INDEX_0                       0






// *****************************************************************************
// *****************************************************************************
// Section: Driver Configuration
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Middleware & Other Library Configuration
// *****************************************************************************
// *****************************************************************************
/* RN WIFI System Service Configuration Options */

#define RNWF_WIFI_DEVMODE        			SYS_RNWF_WIFI_MODE_STA

#define SYS_RNWF_WIFI_STA_SSID				"DEMO_AP"
#define SYS_RNWF_WIFI_STA_PWD        			"password"
#define SYS_RNWF_STA_SECURITY				SYS_RNWF_WPA2 
#define SYS_RNWF_WIFI_STA_AUTOCONNECT   		true


#define SYS_RNWF_WIFI_CallbackHandler			        APP_WIFI_Callback



// *****************************************************************************
// *****************************************************************************
// Section: Application Configuration
// *****************************************************************************
// *****************************************************************************


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // CONFIGURATION_H
/*******************************************************************************
 End of File
*/
