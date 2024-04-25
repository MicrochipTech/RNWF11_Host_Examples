/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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

/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_H
#define _APP_H


#define AZURE_CLOUD     1
//#define AWS_CLOUD     1

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/* This section lists the other files that are included in this file.*/
#include <xc.h> 
#include "configuration.h"
#include "system/console/sys_console.h"
#include "system/inf/sys_rnwf_interface.h"
#include "system/mqtt/sys_rnwf_mqtt_service.h"
#include "system/net/sys_rnwf_net_service.h"
#include "system/wifi/sys_rnwf_wifi_service.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************


#define AZURE_DEVICE_ID         "rnwf02_device_01"
#define AZURE_SCOPE_ID          "0ne00B5478B"
#define AZURE_DPS_ENABLE        1

#define AZURE_PUB_TELEMETRY     "devices/"AZURE_DEVICE_ID"/messages/events/"
#define AZURE_PUB_PROPERTY      "$iothub/twin/PATCH/properties/reported/?$rid=1"
#define AZURE_PUB_TWIN_GET      "$iothub/twin/GET/?$rid=getTwin"
#define AZURE_PUB_CMD_RESP      "$iothub/methods/res/200/?$%s" 

#define AZURE_SUB_METHODS_POST  "$iothub/methods/POST/#"
#define AZURE_SUB_TWIN_PATCH    "$iothub/twin/PATCH/properties/desired/#"
#define AZURE_SUB_TWIN_RES      "$iothub/twin/res/#"

#define AZURE_MSG_IPADDRESS     "\\\"ipAddress\\\":\\\"%s\\\""
#define AZURE_MSG_REP_RATE      "\\\"reportRate\\\":%d"


/* Properties and Telemetry reporting format */
#define AZURE_FMT_BUTTON_TEL    "{\\\"buttonEvent\\\": {\\\"button_name\\\":\\\"SW0\\\", \\\"press_count\\\":%lu}"
#define AZURE_FMT_COUNTER_TEL   "{\\\"counter\\\": \\\"%lu\\\"}"
#define AZURE_FMT_RATE_PROP     "{\\\"reportRate\\\":{\\\"ac\\\":200,\\\"av\\\":%s,\\\"ad\\\":\\\"success\\\",\\\"value\\\":%s}}"
#define AZURE_FMT_DELAY_RSP     "{\\\"status\\\":\\\"Success\\\",\\\"delay\\\":%lu}"
#define AZURE_FMT_ECHO_RSP      "{\\\"echoString\\\":\\\"%s\\\"}"

/* Parsing Tags for the Azure messages */
#define AZURE_VERSION_TAG       "\"$version\\\":"
#define AZURE_RATE_TAG          "\"reportRate\\\":"
#define AZURE_DEALY_TAG         "\"delay\\\":\\\"PT"
#define AZURE_ECHO_TAG          "\"echoString\\\":\\\""
    
/* System Tick timer tick for 1Sec */
#define APP_SYS_TICK_COUNT_1SEC 10
    
/* APP Cloud Telemetry Rate in seconds */
#define APP_CLOUD_REPORT_INTERVAL  2 * APP_SYS_TICK_COUNT_1SEC

    
 /* Application generic purpose buffer size */
#define APP_BUFFER_SIZE_MAX     512


// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_STATE_INITIALIZE=0,
            
    /*Callback register state*/
    APP_STATE_REGISTER_CALLBACK,
            
    /*Wi-Fi Configuration State*/
    APP_STATE_WIFI_CONFIG,
    
    /*Task State*/
    APP_STATE_TASK,
} APP_STATES;


/*Shows the he application's state*/
typedef enum {
    /*InitializaTION State*/
    APP_SYS_INIT,
            
    /*Cloud up state*/
    APP_CLOUD_UP
} APP_STATE_t;


typedef struct
{
    /* The application's current state */
    APP_STATES state;
}APP_DATA1;

/*function to get IP address*/
uint8_t* APP_GET_IP_Address(void);

/*Azure subscribe handler function*/
void SYS_RNWF_APP_AZURE_SUB_Handler(char *);
// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    APP_STATES state;

    SYS_CONSOLE_HANDLE consoleHandle;

} APP_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_Tasks( void );



#endif /* _APP_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

