"""*****************************************************************************
Copyright (C) 2020 released Microchip Technology Inc.  All rights reserved.

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
*****************************************************************************"""

################################################################################
#### Global Variables ####
################################################################################
global ecc_helpkeyword

ecc_helpkeyword = "mcc_h3_RNWF_ecc_system_service_configurations"
################################################################################
#### Business Logic ####
################################################################################

################################################################################
#### Component ####
################################################################################
def instantiateComponent(sysEccRNWFComponent):
    global ecc_helpkeyword

    rnwfSuppEccVar = sysEccRNWFComponent.createKeyValueSetSymbol("SYS_RNWF_ECC608_VARIANTS", None)
    rnwfSuppEccVar.setLabel("RNWF Supported ECC608 Variants")
    rnwfSuppEccVar.setHelp(ecc_helpkeyword)
    rnwfSuppEccVar.addKey("TNGTLS", "1", "TNG TLS Supported")
    rnwfSuppEccVar.addKey("TFLEX", "2", "Trust Flex Supported")
    rnwfSuppEccVar.setDisplayMode("Key")
    rnwfSuppEccVar.setOutputMode("Key")
    rnwfSuppEccVar.setVisible(True)
    rnwfSuppEccVar.setDefaultValue(0)

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    sysrnwfeccHeaderFile = sysEccRNWFComponent.createFileSymbol("SYS_RNWF_NET_HEADER", None)
    sysrnwfeccHeaderFile.setSourcePath("system/RNWF_ecc/templates/sys_rnwf_ecc_service.h.ftl")
    sysrnwfeccHeaderFile.setOutputName("sys_rnwf_ecc_service.h")
    sysrnwfeccHeaderFile.setDestPath("system/ecc/")
    sysrnwfeccHeaderFile.setProjectPath("config/" + configName + "/system/ecc/")
    sysrnwfeccHeaderFile.setType("HEADER")
    sysrnwfeccHeaderFile.setOverwrite(True)

    print("Network Service Component Header Path %s", sysrnwfeccHeaderFile.getProjectPath())
	
    sysrnwfeccSourceFile = sysEccRNWFComponent.createFileSymbol("SYS_RNWF_NET_SOURCE", None)
    sysrnwfeccSourceFile.setSourcePath("system/RNWF_ecc/templates/src/sys_rnwf_ecc_service.c.ftl")
    sysrnwfeccSourceFile.setOutputName("sys_rnwf_ecc_service.c")
    sysrnwfeccSourceFile.setDestPath("system/ecc/src")
    sysrnwfeccSourceFile.setProjectPath("config/" + configName + "/system/ecc/")
    sysrnwfeccSourceFile.setType("SOURCE")
    sysrnwfeccSourceFile.setOverwrite(True)

    sysrnwfeccSystemConfigFile = sysEccRNWFComponent.createFileSymbol("SYS_RNWF_CONSOLE_SYS_CONFIG", None)
    sysrnwfeccSystemConfigFile.setType("STRING")
    sysrnwfeccSystemConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_SYSTEM_SERVICE_CONFIGURATION")
    sysrnwfeccSystemConfigFile.setSourcePath("system/RNWF_ecc/templates/system/system_config.h.ftl")
    sysrnwfeccSystemConfigFile.setMarkup(True)

############################################################################
#### Dependency ####
############################################################################
#Set symbols of other components
def setVal(component, symbol, value):
    triggerSvDict = {"Component":component,"Id":symbol, "Value":value}
    if(Database.sendMessage(component, "SET_SYMBOL", triggerSvDict) == None):
        print("Set Symbol Failure" + component + ":" + symbol + ":" + str(value))
        return False
    else:
        return True

#Handle messages from other components
def handleMessage(messageID, args):
    retDict= {}
    if (messageID == "SET_SYMBOL"):
        print("handleMessage: Set Symbol")
        retDict= {"Return": "Success"}
        Database.setSymbolValue(args["Component"], args["Id"], args["Value"])
    else:
        retDict= {"Return": "UnImplemented Command"}
    return retDict

def onAttachmentConnected(source, target):
    localComponent = source["component"]
    remoteComponent = target["component"]
    remoteID = remoteComponent.getID()
    connectID = source["id"]
    targetID = target["id"]

def onAttachmentDisconnected(source, target):
    localComponent = source["component"]
    remoteComponent = target["component"]
    remoteID = remoteComponent.getID()
    connectID = source["id"]
    targetID = target["id"]

def setVisible_OnValueChanged(symbol, event):
    symbol.setVisible(event['value'])
def sysnetSubMenuVisible(symbol, event):
    print("sysnetSubMenuVisible.")
    if (event["value"] == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

def finalizeComponent(sysEccRNWFComponent):
    print("finalizeComponent.")        

def destroyComponent(component):
    print("destroyComponent")
    
