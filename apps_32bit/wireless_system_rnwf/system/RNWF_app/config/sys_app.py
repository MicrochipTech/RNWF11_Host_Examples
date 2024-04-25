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
global app_helpkeyword


app_helpkeyword = "mcc_h3_RNWF_app_configurations"
################################################################################
#### Business Logic ####
################################################################################

################################################################################
#### Component ####
################################################################################
def instantiateComponent(sysAppRNWFComponent):
    global app_helpkeyword

#    rnwfSuppVar = sysAppRNWFComponent.createBooleanSymbol("SYS_RNWF11_ENABLE", None)
#    rnwfSuppVar.setHelp(app_helpkeyword)
#    rnwfSuppVar.setVisible(True)
#    rnwfSuppVar.setDefaultValue(False)

    rnwfSuppVar = sysAppRNWFComponent.createKeyValueSetSymbol("SYS_RNWF_VARIANTS", None)
    rnwfSuppVar.setLabel("RNWF Supported Variants")
    rnwfSuppVar.setHelp(app_helpkeyword)
    rnwfSuppVar.addKey("RNWF02", "0", "RNWF02 Supported")
    rnwfSuppVar.addKey("RNWF11", "1", "RNWF11 Supported")
    rnwfSuppVar.setDisplayMode("Key")
    rnwfSuppVar.setOutputMode("Key")
    rnwfSuppVar.setVisible(True)
    rnwfSuppVar.setDefaultValue(0)
    rnwfSuppVar.setDependencies(rnwfSuppVarAutoMenu, ["SYS_RNWF_VARIANTS"])
    
    ############################################################################
    #### Code Generation ####
    ############################################################################

    sysrnwfAppConfigFile = sysAppRNWFComponent.createFileSymbol("SYS_RNWF_APP_CONFIG", None)
    sysrnwfAppConfigFile.setType("STRING")
    sysrnwfAppConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_SYSTEM_SERVICE_CONFIGURATION")
    sysrnwfAppConfigFile.setSourcePath("system/RNWF_app/templates/system/system_config.h.ftl")
    sysrnwfAppConfigFile.setMarkup(True)

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

def rnwfSuppVarAutoMenu(symbol, event):
    if (event["value"] == 1):
        print("RNWF component is RNWF11")
        res = Database.activateComponents(["sysEccRNWF"])
        res = Database.activateComponents(["sys_time"])
        res = Database.activateComponents(["tc0"])
        autoConnectTabletc0 = [["tc0","TC0_TMR" , "sys_time", "sys_time_TMR_dependency"]]
        res = Database.connectDependencies(autoConnectTabletc0)
    else:
        print("RNWF component is RNWF02")
        res = Database.deactivateComponents(["tc0"])
        res = Database.deactivateComponents(["sys_time"])
        res = Database.deactivateComponents(["sysEccRNWF"])


def finalizeComponent(sysAppRNWFComponent):
    print("finalizeComponent")

def destroyComponent(component):
    print("destroyComponent")
    res = Database.deactivateComponents(["tc0"])
    res = Database.deactivateComponents(["sys_time"])
    res = Database.deactivateComponents(["sysEccRNWF"])
