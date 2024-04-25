/*******************************************************************************
  RNWF Host Assisted Ota service Header file

  File Name:
    sys_rnwf_ota_service.h

  Summary:
    Header file for the RNWF Host Assisted Ota service implementation.

  Description:
    This header file provides a simple APIs to enable Ota service with RNWF device 
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
#ifndef SYS_RNWF_OTA_SERVICE_H
#define	SYS_RNWF_OTA_SERVICE_H

/* Ota flash buffer size */
#define SYS_RNWF_OTA_FLASH_BUFFER_SIZE       4096

/* Convert to string */
#define VAL(str)    #str
#define SYS_RNWF_OTA_TOSTR(str)  VAL(str)

/* Debug print for Ota service */
#define SYS_RNWF_OTA_DBG_MSG(args, ...)      SYS_CONSOLE_PRINT("[OTA]:"args, ##__VA_ARGS__)

/* Maximum length of ota buffer */
#define SYS_RNWF_OTA_BUF_LEN_MAX             4096

/* Ota configuration tunnel */
#define SYS_RNWF_OTA_CONF_TUNNEL             ${SYS_RNWF_OTA_TUNNEL}

/* Ota configuration port */
#define SYS_RNWF_OTA_CONF_PORT               ${SYS_RNWF_OTA_TUNNEL_PORT}

/* Ota Configuration Firmware header */
#define SYS_RNWF_OTA_CONF_FW_HDR             "firmware:"

/* Ota Configuration File system header */
#define SYS_RNWF_OTA_CONF_FS_HDR             "filesystem:"

/* Ota Configuration Format */
#define SYS_RNWF_OTA_CONF_FORMAT             "%s %d %s"

/* Maximum ota images */
#define SYS_RNWF_OTA_IMAGE_MAX               2

/* Maximum supported write size by DFU PE (Programming Executive). */
#define SYS_RNWF_OTA_MAX_PE_WRITE_SIZE         4096U

/* PE commands */
#define SYS_RNWF_OTA_PE_CMD_PAGE_ERASE         0x05
#define SYS_RNWF_OTA_PE_CMD_EXEC_VERSION       0x07
#define SYS_RNWF_OTA_PE_CMD_PGM_CLUSTER_VERIFY 0x11
#define SYS_RNWF_OTA_PE_CMD_GET_DEVICE_ID      0xA
#define SYS_RNWF_OTA_CFG_METHOD                0x1

/* PE sizes */
#define SYS_RNWF_OTA_PE_ERASE_PAGE_SIZE        4096
#define SYS_RNWF_OTA_PE_MAX_RESPONSE_SIZE         8

/* Time */
/* Values may need to be adjusted based on host platform. */
#define SYS_RNWF_OTA_TP_DELAY_USEC             100
#define SYS_RNWF_OTA_MSEC_TO_SEC               1000
#define SYS_RNWF_OTA_WRITE_DELAY_USEC          250
#define SYS_RNWF_OTA_UART_DELAY_MSEC           500

#ifndef RNWF11_SERVICE      /* RNWF02 */
/* DFU */
/* Enable to get debug ota prints */
//#define SYS_RNWF_OTA_DFU_DEBUG

/* RNWF PE version */
#define SYS_RNWF_OTA_RIO0_PE_VERSION           1

/* RNWF Chip ID */
//#define SYS_RNWF_OTA_RIO0_CHIP_ID            0x29c71053
#define SYS_RNWF_OTA_RIO0_CHIP_ID              0x29c70053

/* RNWF Flash base address  */
#define SYS_RNWF_OTA_RIO0_FLASH_BASE           0x60000000

/* SYS_RNWF_OTA_DFU_PE_WRITE_SIZE should be integer factor of 4096 and larger than 2
 * e.g. (4,8,16,32,64,128,256,512,4096).
 * Must not exceed SYS_RNWF_MAX_PE_WRITE_SIZE (4096) or DFU_PE_Write() will fail.
 */
#define SYS_RNWF_OTA_DFU_PE_WRITE_SIZE   4096
#else    /* RNWF02 */

#define SYS_RNWF_OTA_BL_CMD_UNLOCK       0xa0
#define SYS_RNWF_OTA_BL_CMD_DATA         0xa1
#define SYS_RNWF_OTA_BL_CMD_VERIFY       0xa2
#define SYS_RNWF_OTA_BL_CMD_RESET        0xa3
#define SYS_RNWF_OTA_BL_CMD_BKSWAP_RESET 0xa4
#define SYS_RNWF_OTA_BL_CMD_DEVCFG_DATA  0xa5
#define SYS_RNWF_OTA_BL_CMD_READ_VERSION 0xa6
#define SYS_RNWF_OTA_BL_CMD_ENTER_BTL    0xa7
#define SYS_RNWF_OTA_BL_CMD_ERASE_APP    0xa8

#define SYS_RNWF_OTA_BL_RESP_OK          0x50
#define SYS_RNWF_OTA_BL_RESP_ERROR       0x51
#define SYS_RNWF_OTA_BL_RESP_INVALID     0x52
#define SYS_RNWF_OTA_BL_RESP_CRC_OK      0x53
#define SYS_RNWF_OTA_BL_RESP_CRC_FAIL    0x54
#define SYS_RNWF_OTA_BL_RESP_NONE        0xFF

#define SYS_RNWF_OTA_BL_GUARD            0x5048434D

#define SYS_RNWF_OTA_CMD_SIZE                1
#define SYS_RNWF_OTA_GUARD_SIZE              4
#define SYS_RNWF_OTA_SIZE_SIZE               4
#define SYS_RNWF_OTA_ADDR_SIZE               4
#define SYS_RNWF_OTA_CMD_ONLY_EXTRA_SIZE     4
#define SYS_RNWF_OTA_HEADER_SIZE             (SYS_RNWF_OTA_GUARD_SIZE + SYS_RNWF_OTA_SIZE_SIZE + SYS_RNWF_OTA_CMD_SIZE)

#endif

/* Ota socket ID */
#define SYS_RNWF_OTA_SOCK_ID     gOta_CfgData.socket

/* SST26 Flash image start address */
#define SYS_RNWF_OTA_FLASH_IMAGE_START       (0x00000000)

/* Ota http commands */
#define SYS_RNWF_OTA_HTTP_CONTENT_OK     "200 OK"
#define SYS_RNWF_OTA_HTTP_CONTENT_LEN    "Content-Length:"

/* Host PGC,PGD and MCLR Pins */
#define SYS_RNWF_OTA_PGC_PIN            SYS_PORT_PIN_PA04
#define SYS_RNWF_OTA_PGD_PIN            SYS_PORT_PIN_PA05
#define SYS_RNWF_OTA_MCLR_PIN           SYS_PORT_PIN_PA06

#define SYS_RNWF_OTA_PGC_PORT_PIN      PORT_PIN_PA04
#define SYS_RNWF_OTA_PGD_PORT_PIN      PORT_PIN_PA05
#define SYS_RNWF_OTA_MCLR_PORT_PIN     PORT_PIN_PA06

// *****************************************************************************

/* Ota Flash data

  Summary:
 `Holds the SST26 flash information

  Remarks:
    None.
 */

typedef struct
{
    /* Driver Handle */
    DRV_HANDLE handle;
    
    /* SST26 Device Geometry */
    DRV_SST26_GEOMETRY geometry;
    
    /* Check if transfer is completed */
    bool isTransferDone;
    
    /* Write buffer of SST26 */
    uint8_t writeBuffer[SYS_RNWF_OTA_FLASH_BUFFER_SIZE];
    
    /* Read buffer of SST26 */
    uint8_t readBuffer[SYS_RNWF_OTA_FLASH_BUFFER_SIZE];

}SYS_RNWF_OTA_FLASH_DATA;


// *****************************************************************************

/* Ota Configuration parameters

  Summary:
  Identifies Ota Configuration parameter 

  Remarks:
    None.
 */
typedef enum
{
    /* OTA configuration parameter Port */
    SYS_OTA_CFG_PARAM_PORT,
    
    /* OTA configuration parameter Server */        
    SYS_OTA_CFG_PARAM_SERVER,
            
    /* OTA configuration parameter File */
    SYS_OTA_CFG_PARAM_FILE,
            
    /* OTA configuration parameter Type */
    SYS_OTA_CFG_PARAM_TYPE,
            
    /* OTA configuration parameter Max */
    SYS_OTA_CFG_PARAM_MAX,
            
}SYS_RNWF_OTA_CFG_PARAM_t;

// *****************************************************************************

/* Ota Modes

  Summary:
  Identifies Ota Image source 

  Remarks:
    None.
 */
typedef enum
{
    /**<FW file is from HTTP server */
    SYS_RNWF_OTA_MODE_HTTP,     
            
    /**<FW file is from Host USB */
    SYS_RNWF_OTA_MODE_USB,     
            
    /**<FW file is from Host UART*/
    SYS_RNWF_OTA_MODE_UART,     
            
}SYS_RNWF_OTA_MODES_t;


// *****************************************************************************

/* Ota Image Types

  Summary:
  Identifies Ota Image types 

  Remarks:
    None.
 */
typedef enum
{
    /**<FW at lower slot */
    SYS_RNWF_OTA_LOW_FW,
            
    /**<FW at higher slot */
    SYS_RNWF_OTA_HIGH_FW,       
            
    /**<Files system slot*/
    SYS_RNWF_OTA_FILESYSTEM,
            
}SYS_RNWF_OTA_IMAGE_t;

// *****************************************************************************

/* Ota Service List

  Summary:
  Identifies Ota Service list 

  Remarks:
    None.
 */
typedef enum
{
    /* Enable OTA service */
    SYS_RNWF_OTA_ENABLE,  
            
    /* Disable OTA service */
    SYS_RNWF_OTA_DISABLE,
            
    /* Send OTA server request */
    SYS_RNWF_OTA_REQUEST,               
            
    /**<Register OTA application callback*/
    SYS_RNWF_OTA_SET_CALLBACK,          
            
    /**<Register OTA Service callback*/
    SYS_RNWF_OTA_SET_SRVC_CALLBACK,
            
    /**<OTA Auto mode, triggered reqularly*/
    SYS_RNWF_OTA_AUTO_MODE,          
            
    /**<OTA Trigger, Actual programming start*/
    SYS_RNWF_OTA_DFU_INIT,           
            
    /**<OTA Write, Writes the FW max 4096 bytes*/
    SYS_RNWF_OTA_DFU_WRITE,             
            
    /**<OTA Erase, Erases the given size*/
    SYS_RNWF_OTA_DFU_ERASE,             
            
    /**<OTA Error, Error in OTA service*/
    SYS_RNWF_OTA_DFU_ERROR,
            
    /**<OTA Idle, Idle state of service*/
    SYS_RNWF_OTA_DFU_IDLE,
            
    /**<OTA Get image size */
    SYS_RNWF_OTA_GET_IMG_SIZE,
            
    /**<OTA check, if download is completed */
    SYS_RNWF_OTA_CHECK_DWLD_DONE,
    
    /**<OTA check, if DFU is completed */        
    SYS_RNWF_OTA_CHECK_DFU_DONE,

#ifdef RNWF11_SERVICE            
    SYS_RNWF_OTA_DFU_RESET,             /**<OTA Reset, Erases the given size*/
#endif          
}SYS_RNWF_OTA_SERVICE_t;

// *****************************************************************************

/* Ota Event List

  Summary:
  Identifies Ota Event list 

  Remarks:
    None.
 */
typedef enum
{
    /**<Change to UART mode */
    SYS_RNWF_OTA_EVENT_MAKE_UART,       
            
    /**<Inform Image details */
    SYS_RNWF_OTA_EVENT_IMAGE_INFO,      
            
    /**<FW Downloaded file chunk */
    SYS_RNWF_OTA_EVENT_FILE_CHUNK,      
            
    /**<FW Download start */
    SYS_RNWF_OTA_EVENT_DWLD_START,      
            
    /**<FW Download done */
    SYS_RNWF_OTA_EVENT_DWLD_DONE,
            
    /**<FW Download fail */
    SYS_RNWF_OTA_EVENT_DWLD_FAIL,
            
    /**Configuration info  */
    SYS_RNWF_OTA_EVENT_CONFIG_INFO,
     
}SYS_RNWF_OTA_EVENT_t;


// *****************************************************************************

/* Ota Program Event List

  Summary:
  Identifies Ota Program Event list 

  Remarks:
    None.
 */
typedef enum
{
    /*RNWF program init */
    SYS_RNWF_PROGRAM_INIT,
            
    /*RNWF program read from SST26 */
    SYS_RNWF_PROGRAM_FLASH_READ,
            
    /*Erase RNWF flash  */
    SYS_RNWF_PROGRAM_DFU_ERASE,
    
    /*Write to RNWF flash  */        
    SYS_RNWF_PROGRAM_DFU_WRITE,
            
    /*RNWF Program complete  */
    SYS_RNWF_PROGRAM_COMPLETE,
            
    /*RNWF program error  */
    SYS_RNWF_PROGRAM_ERROR,
            
}SYS_RNWF_OTA_PROGRAM_EVENT_t;

// *****************************************************************************

/* RNWF OTA configuration structure

  Summary:
    OTA configuration structure

  Remarks:
    None.
 */
typedef struct
{
    /**<Socket handler for HTTP link*/
    SYS_RNWF_NET_SOCKET_t socket;               
    
    /**<Active OTA mode */
    SYS_RNWF_OTA_MODES_t mode;      
    
    /**<Image type */
    SYS_RNWF_OTA_IMAGE_t type;                  
    
    /**<Image File Name */
    const char      *file;                  
    
    /**<Certificate File Name */
    const char      *certificate;
    
}SYS_RNWF_OTA_CFG_t;


// *****************************************************************************

/* RNWF OTA Download chunk structure

  Summary:
    OTA Download chunk structure

  Remarks:
    None.
 */
typedef struct
{
    /* Downloaded chunk address */
    uint32_t chunk_addr; 
    
    /* Downloaded chunk size */
    uint32_t chunk_size;    
    
    /* Downloaded chunk pointer */
    uint8_t  *chunk_ptr;    
    
}SYS_RNWF_OTA_CHUNK_t;

// *****************************************************************************

/* RNWF OTA header structure

  Summary:
    OTA header structure

  Remarks:
    None.
 */
typedef struct
{
    /* Header sequence number */
    uint32_t seq_num;
    
    /* Header Firmware version */
    uint32_t fw_ver;
    
    /* Header start address */
    uint32_t start_addr;   
    
    /* Image length */
    uint32_t img_len;
    
}SYS_RNWF_OTA_HDR_t;


// *****************************************************************************
/*  Function:
        SYS_RNWF_OTA_SrvCtrl( SYS_RNWF_OTA_SERVICE_t request, void *input);

    Summary:
        Ota Service Control function

    Description:
        This  function controls the OTA service implementation
 
    Remarks:
        None
 */
SYS_RNWF_RESULT_t SYS_RNWF_OTA_SrvCtrl( SYS_RNWF_OTA_SERVICE_t request, void *input);

// *****************************************************************************
/*  Function:
        typedef void (*RNWF_OTA_GPIO_CTRL_FUNC_t)(uint32_t pin, uint8_t output);

    Summary:
        Function Pointer to OTA GPIO Control Function

    Description:
        This  function Controls GPIO Function
 
    Remarks:
        None
 */
typedef void (*RNWF_OTA_GPIO_CTRL_FUNC_t)(uint32_t pin, uint8_t output);

// *****************************************************************************
/*  Function:
        typedef void (*RNWF_OTA_UART_TX_FUNC_t)(uint8_t *buffer, uint32_t length);

    Summary:
        Function Pointer to OTA UART Tx Function

    Description:
        This  function refers to OTA UART Tx Function
 
    Remarks:
        None
 */

typedef void (*RNWF_OTA_UART_TX_FUNC_t)(uint8_t *buffer, uint32_t length);

// *****************************************************************************
/*  Function:
        typedef void (*RNWF_OTA_FLASH_WR_FUNC_t)(uint8_t *buffer, uint32_t length);

    Summary:
        Function Pointer to OTA Flash Write Function

    Description:
        This  function refers to OTA Flash Write Function
 
    Remarks:
        None
 */
typedef void (*RNWF_OTA_FLASH_WR_FUNC_t)(uint8_t *buffer, uint32_t length);

// *****************************************************************************
/*  Function:
        typedef void (*RNWF_OTA_DLASH_RD_FUNC_t)(uint8_t *buffer, uint32_t length);

    Summary:
        Function Pointer to OTA Flash Read Function

    Description:
        This  function refers to OTA Flash Read Function
 
    Remarks:
        None
 */
typedef void (*RNWF_OTA_DLASH_RD_FUNC_t)(uint8_t *buffer, uint32_t length);

// *****************************************************************************
/*  Function:
        typedef void (*SYS_RNWF_OTA_CALLBACK_t)(SYS_RNWF_OTA_EVENT_t, void *);

    Summary:
        Function Pointer to  OTA Service Callback function

    Description:
        This  function  refers  OTA Service Callback function
 
    Remarks:
        None
 */
typedef void (*SYS_RNWF_OTA_CALLBACK_t)(SYS_RNWF_OTA_EVENT_t, void *);


// *****************************************************************************
/*  Function:
        void SYS_RNWF_OTA_DfuReset(void);

    Summary:
        Resets RNWF device

    Description:
        This  function  resets the RNWF device by toggling pins.
 
    Remarks:
        None
 */
void SYS_RNWF_OTA_DfuReset ( void ) ;

// *****************************************************************************
/*  Function:
        void SYS_RNWF_OTA_FlashEventHandler( DRV_SST26_TRANSFER_STATUS event, uintptr_t context );

    Summary:
        Event handler of SST26 flash events

    Description:
        This  function  handles SST26 events
 
    Remarks:
        None
 */
void SYS_RNWF_OTA_FlashEventHandler( DRV_SST26_TRANSFER_STATUS event, uintptr_t context );

// *****************************************************************************
/*  Function:
        bool SYS_RNWF_OTA_FlashInitialize( void );

    Summary:
        SST26 flash Initialize

    Description:
        This  function  initializes the SST26 flash
 
    Remarks:
        None
 */
bool SYS_RNWF_OTA_FlashInitialize( void);

// *****************************************************************************
/*  Function:
        bool SYS_RNWF_OTA_FlashErase( void );

    Summary:
        SST26 flash Erase

    Description:
        This  function  Erases the SST26 flash
 
    Remarks:
        None
 */
bool SYS_RNWF_OTA_FlashErase ( void );

// *****************************************************************************
/*  Function:
        bool SYS_RNWF_OTA_SectorErase( uint32_t addr );

    Summary:
        SST26 Sector Erase

    Description:
        This  function  Erases a SST26 sector
 
    Remarks:
        None
 */
bool SYS_RNWF_OTA_SectorErase( uint32_t addr );

// *****************************************************************************
/*  Function:
        bool SYS_RNWF_OTA_FlashRead(uint32_t addr,uint32_t size,uint8_t *buf);

    Summary:
        SST26 flash Read

    Description:
        This  function  Reads from the SST26 flash
 
    Remarks:
        None
 */
bool SYS_RNWF_OTA_FlashRead(uint32_t addr,uint32_t size,uint8_t *buf);

// *****************************************************************************
/*  Function:
        bool SYS_RNWF_OTA_FlashWrite(uint32_t addr,uint32_t size,uint8_t *buf);

    Summary:
        SST26 flash Write

    Description:
        This  function  Writes to the SST26 flash
 
    Remarks:
        None
 */
bool SYS_RNWF_OTA_FlashWrite ( uint32_t addr, uint32_t size, uint8_t *buf ) ;

// *****************************************************************************
/*  Function:
        void SYS_RNWF_OTA_ProgramDfu ( void ) ;

    Summary:
        Program RNWF device

    Description:
        This  function  programs the RNWF device
 
    Remarks:
        None
 */
void SYS_RNWF_OTA_ProgramDfu ( void ) ;

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
    
#endif	/* SYS_RNWF_OTA_SERVICE_H */

