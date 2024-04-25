# coding: utf-8
"""*****************************************************************************
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
*****************************************************************************"""

################################################################################
#### Global Variables ####
################################################################################
global wifi_prov_helpkeyword

wifi_prov_helpkeyword = "mcc_h3_pic32mzw1_wifi_prov_system_service_configurations"
################################################################################
#### Business Logic ####
################################################################################

################################################################################
#### Component ####
################################################################################
def instantiateComponent(syswifiprovComponent):

    global wifi_prov_helpkeyword

    # Enable dependent Harmony core components

    syswifiprovNvmAdd = syswifiprovComponent.createStringSymbol("SYS_RNWF_WIFIPROV_CALLBACK_HANDLER", None )
    syswifiprovNvmAdd.setLabel("Provision Callback Handler")
    syswifiprovNvmAdd.setHelp(wifi_prov_helpkeyword)
    syswifiprovNvmAdd.setVisible(True)
    syswifiprovNvmAdd.setDescription("Enterthe provision callback function")
    syswifiprovNvmAdd.setDefaultValue("APP_WIFIPROV_Callback")

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    syswifiprovSystemConfFile = syswifiprovComponent.createFileSymbol("SYS_RNWF_WIFIPROV_CONFIGURATION_H", None)
    syswifiprovSystemConfFile.setType("STRING")
    syswifiprovSystemConfFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    syswifiprovSystemConfFile.setSourcePath("system/RNWF_wifiprov/templates/system/system_config.h.ftl")
    syswifiprovSystemConfFile.setMarkup(True)

    syswifiprovHeaderFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_SOURCE", None)
    syswifiprovHeaderFile.setSourcePath("system/RNWF_wifiprov/templates/src/sys_rnwf_provision_service.c.ftl")
    syswifiprovHeaderFile.setOutputName("sys_rnwf_provision_service.c")
    syswifiprovHeaderFile.setDestPath("system/wifiprov/src")
    syswifiprovHeaderFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovHeaderFile.setType("SOURCE")
    syswifiprovHeaderFile.setMarkup(True)
    syswifiprovHeaderFile.setEnabled(True)

    syswifiprovSourceFile = syswifiprovComponent.createFileSymbol("SYS_WIFIPROV_HEADER", None)
    syswifiprovSourceFile.setSourcePath("system/RNWF_wifiprov/templates/sys_rnwf_provision_service.h.ftl")
    syswifiprovSourceFile.setOutputName("sys_rnwf_provision_service.h")
    syswifiprovSourceFile.setDestPath("system/wifiprov/")
    syswifiprovSourceFile.setProjectPath("config/" + configName + "/system/wifiprov/")
    syswifiprovSourceFile.setType("HEADER")
    syswifiprovSourceFile.setMarkup(True)
    syswifiprovSourceFile.setEnabled(True)

############################################################################
#### Dependency ####
############################################################################

def setVal(component, symbol, value):
    print("setVal")

#Handle messages from other components
def handleMessage(messageID, args):
    print("handleMessage")

def onAttachmentConnected(source, target):
    print("onAttachmentConnected")

def syswifiprovNvmCheck(symbol, event):
    print("syswifiprovNvmCheck")

def syswifiprovCustomSet(symbol, event):
    print("syswifiprovCustomSet")
        

def onAttachmentDisconnected(source, target):
    print("onAttachmentDisconnected")

def syswifiprovMenuVisible(symbol, event):
    print("syswifiprovMenuVisible")

def enableTcpipAutoConfigApps(enable):
    print("enableTcpipAutoConfigApps")


def syswifiprovGenSourceFile(sourceFile, event):
    print("WiFi Provisioning Menu Visible.")

def syswifiprovHTTPGenSourceFile(sourceFile, event):
    print("syswifiprovGenSourceFile.")


def syswifiprovHTTPNETGenSourceFile(sourceFile, event):
    print("syswifiprovHTTPNETGenSourceFile")


def syswifiprovHTTPMenuVisible(symbol, event):
    print("syswifiprovHTTPMenuVisible")

def finalizeComponent(component):
    print("finalizeComponent")

def destroyComponent(component):
    print("destroyComponent")


def syswifiprovManageNvmAddr(symbol, event):
    print("syswifiprovManageNvmAddr")
