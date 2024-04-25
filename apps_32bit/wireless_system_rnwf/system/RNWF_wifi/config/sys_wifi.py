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
global wifi_helpkeyword

wifi_helpkeyword = "mcc_h3_RNWF_wifi_system_service_configurations"
################################################################################
#### Business Logic ####
################################################################################

################################################################################
#### Component ####
################################################################################

from datetime import datetime


def instantiateComponent(sysWifiRNWFComponent):
    global wifi_helpkeyword
    syswifiEnable = sysWifiRNWFComponent.createBooleanSymbol("SYS_RNWF_WIFI_ENABLE", None)
    syswifiEnable.setHelp(wifi_helpkeyword)
    syswifiEnable.setVisible(False)
    syswifiEnable.setDefaultValue(True)

    sysrnwfwifiEnableErrMsg = sysWifiRNWFComponent.createCommentSymbol("SYS_RNWF_WIFI_ERR", None)
    sysrnwfwifiEnableErrMsg.setLabel("**Placeholder for error display")
    sysrnwfwifiEnableErrMsg.setHelp(wifi_helpkeyword)
    sysrnwfwifiEnableErrMsg.setVisible(False)

    sysrnwfwifiMode = sysWifiRNWFComponent.createComboSymbol("SYS_RNWF_WIFI_MODE", None, ["STA", "AP", "PROV" ])
    sysrnwfwifiMode.setLabel("Device Mode")
    sysrnwfwifiMode.setHelp(wifi_helpkeyword)
    sysrnwfwifiMode.setDescription("Select the Device Boot Mode ")
    sysrnwfwifiMode.setDefaultValue("STA")

    sysrnwfwifistaEnable = sysWifiRNWFComponent.createBooleanSymbol("SYS_RNWF_WIFI_MODE_STA", sysrnwfwifiMode)
    sysrnwfwifistaEnable.setLabel("STA Mode")
    sysrnwfwifistaEnable.setHelp(wifi_helpkeyword)
    sysrnwfwifistaEnable.setDefaultValue(True)
    sysrnwfwifistaEnable.setDescription("Enable STA mode Configuration ")
    sysrnwfwifistaEnable.setDependencies(syswifiSTAautoMenu, ["SYS_RNWF_WIFI_MODE_STA"])
    
    sysrnwfServices = sysWifiRNWFComponent.createCommentSymbol("SYS_SYSTEM_SERVICE", None)
    sysrnwfServices.setLabel("Advanced Configurations ")
    sysrnwfServices.setHelp(wifi_helpkeyword)
        
    sysrnwfcompautoactivate = sysWifiRNWFComponent.createComboSymbol("SYS_RNWF_COMPONENT_ACTIVATE", sysrnwfServices , ["None" , "SAME54X-pro"])
    sysrnwfcompautoactivate.setLabel("Auto Component Activation of device ")
    sysrnwfcompautoactivate.setHelp(wifi_helpkeyword)
    sysrnwfcompautoactivate.setDependencies(syscomponentautoactivate, ["SYS_RNWF_COMPONENT_ACTIVATE"])
    sysrnwfcompautoactivate.setDescription("Select the Device")
    sysrnwfcompautoactivate.setDefaultValue("None")
    
    sysrnwfinterfaceMode = sysWifiRNWFComponent.createComboSymbol("SYS_RNWF_INTERFACE_MODE", sysrnwfServices , ["UART"])
    sysrnwfinterfaceMode.setLabel("Interface Mode")
    sysrnwfinterfaceMode.setHelp(wifi_helpkeyword)
    sysrnwfinterfaceMode.setDescription("Select the Device Interface Mode ")
    sysrnwfinterfaceMode.setDefaultValue("UART")

    sysrnwfwifiSerprovEnable = sysWifiRNWFComponent.createBooleanSymbol("SYS_RNWF_WIFI_SER_PROV_ENABLE", sysrnwfServices)
    sysrnwfwifiSerprovEnable.setLabel("RNWF WiFi Provisioning Service")
    sysrnwfwifiSerprovEnable.setHelp(wifi_helpkeyword)
    sysrnwfwifiSerprovEnable.setDefaultValue(False)
    sysrnwfwifiSerprovEnable.setDescription("Enable WiFi Provisioning System Service ")
    sysrnwfwifiSerprovEnable.setDependencies(syswifiprovautoInclude, ["SYS_RNWF_WIFI_SER_PROV_ENABLE"])

    sysrnwfmqttEnable = sysWifiRNWFComponent.createBooleanSymbol("SYS_RNWF_MQTT_ENABLE", sysrnwfServices)
    sysrnwfmqttEnable.setLabel("RNWF MQTT Service")
    sysrnwfmqttEnable.setHelp(wifi_helpkeyword)
    sysrnwfmqttEnable.setDefaultValue(False)
    sysrnwfmqttEnable.setDescription("Enable mqtt System Service ")
    sysrnwfmqttEnable.setDependencies(sysmqttautoInclude, ["SYS_RNWF_MQTT_ENABLE"])

    sysrnwfotaSerEnable = sysWifiRNWFComponent.createBooleanSymbol("SYS_RNWF_OTA_SER_ENABLE", sysrnwfServices)
    sysrnwfotaSerEnable.setLabel("RNWF OTA Service")
    sysrnwfotaSerEnable.setHelp(wifi_helpkeyword)
    sysrnwfotaSerEnable.setDefaultValue(False)
    sysrnwfotaSerEnable.setDescription("Enable ota System Service ")
    sysrnwfotaSerEnable.setDependencies(sysotaautoInclude, ["SYS_RNWF_OTA_SER_ENABLE"])

    sysrnwfnetSerEnable = sysWifiRNWFComponent.createBooleanSymbol("SYS_RNWF_NET_SER_ENABLE", sysrnwfServices)
    sysrnwfnetSerEnable.setLabel("RNWF NET Service")
    sysrnwfnetSerEnable.setHelp(wifi_helpkeyword)
    sysrnwfnetSerEnable.setDefaultValue(False)
    sysrnwfnetSerEnable.setDescription("Enable net System Service ")
    sysrnwfnetSerEnable.setDependencies(sysnetautoInclude, ["SYS_RNWF_NET_SER_ENABLE"])

    sysrnwfwifistaSsid = sysWifiRNWFComponent.createStringSymbol("SYS_RNWF_WIFI_STA_SSID_NAME", sysrnwfwifistaEnable)
    sysrnwfwifistaSsid.setLabel("SSID")
    sysrnwfwifistaSsid.setHelp(wifi_helpkeyword)
    sysrnwfwifistaSsid.setVisible(True)
    sysrnwfwifistaSsid.setDescription("Enter STA Mode SSID.The maximum length is 32 characters.")
    sysrnwfwifistaSsid.setDefaultValue("DEMO_AP")
    sysrnwfwifistaSsid.setDependencies(syswifiMenuVisible, ["SYS_RNWF_WIFI_MODE_STA"])

    sysrnwfwifistaAuth = sysWifiRNWFComponent.createComboSymbol("SYS_RNWF_WIFI_STA_SECURITY", sysrnwfwifistaEnable, ["OPEN", "WPA2-MIXED" , "WPA2" , "WPA3-TRANS" , "WPA3"])
    sysrnwfwifistaAuth.setLabel("Security type")
    sysrnwfwifistaAuth.setHelp(wifi_helpkeyword)
    sysrnwfwifistaAuth.setDescription("Enter STA Mode Security type")
    sysrnwfwifistaAuth.setDefaultValue("WPA2")
    sysrnwfwifistaAuth.setDependencies(syswifiMenuVisible, ["SYS_RNWF_WIFI_MODE_STA"])

    sysrnwfwifistaPwd = sysWifiRNWFComponent.createStringSymbol("SYS_RNWF_WIFI_STA_PWD_NAME", sysrnwfwifistaEnable)
    sysrnwfwifistaPwd.setLabel("Password")
    sysrnwfwifistaPwd.setHelp(wifi_helpkeyword)
    sysrnwfwifistaPwd.setVisible(True)
    sysrnwfwifistaPwd.setDescription("Enter STA Mode Password.WPA2/WPA3 - Maximum key length is 63 characters.Minimum key length is 8 characters.")
    sysrnwfwifistaPwd.setDefaultValue("password")
    sysrnwfwifistaPwd.setDependencies(syswifiMenuVisible, ["SYS_RNWF_WIFI_MODE_STA"])

    sysrnwfwifistaAuto = sysWifiRNWFComponent.createBooleanSymbol("SYS_RNWF_WIFI_STA_AUTOCONNECT", sysrnwfwifistaEnable)
    sysrnwfwifistaAuto.setLabel("Auto Connect")
    sysrnwfwifistaAuto.setHelp(wifi_helpkeyword)
    sysrnwfwifistaAuto.setDescription("Enable Auto Connect Feature")
    sysrnwfwifistaAuto.setDefaultValue(True)

    sysrnwfwifiapEnable = sysWifiRNWFComponent.createBooleanSymbol("SYS_RNWF_WIFI_MODE_AP", sysrnwfwifiMode)
    sysrnwfwifiapEnable.setLabel("AP Mode")
    sysrnwfwifiapEnable.setHelp(wifi_helpkeyword)
    sysrnwfwifiapEnable.setDefaultValue(False)
    sysrnwfwifiapEnable.setDescription("Enable AP Mode Configuration")
    sysrnwfwifiapEnable.setDependencies(syswifiAPautoMenu, ["SYS_RNWF_WIFI_MODE_AP"])

    sysrnwfwifiapSsid = sysWifiRNWFComponent.createStringSymbol("SYS_RNWF_WIFI_AP_SSID_NAME", sysrnwfwifiapEnable)
    sysrnwfwifiapSsid.setLabel("SSID")
    sysrnwfwifiapSsid.setHelp(wifi_helpkeyword)
    sysrnwfwifiapSsid.setVisible(False)
    sysrnwfwifiapSsid.setDescription("Enter AP Mode SSID.The maximum length is 32 characters")
    sysrnwfwifiapSsid.setDefaultValue("DEMO_AP_SOFTAP")
    sysrnwfwifiapSsid.setDependencies(syswifiMenuVisible, ["SYS_RNWF_WIFI_MODE_AP"])

    sysrnwfwifiapAuth = sysWifiRNWFComponent.createComboSymbol("SYS_RNWF_WIFI_AP_SECURITY", sysrnwfwifiapEnable, ["OPEN", "WPA2-MIXED" , "WPA2" , "WPA3-TRANS" , "WPA3"])
    sysrnwfwifiapAuth.setLabel("Security")
    sysrnwfwifiapAuth.setHelp(wifi_helpkeyword)
    sysrnwfwifiapAuth.setVisible(False)
    sysrnwfwifiapAuth.setDescription("Enter AP Mode Security")
    sysrnwfwifiapAuth.setDefaultValue("WPA2")
    sysrnwfwifiapAuth.setDependencies(syswifiMenuVisible, ["SYS_RNWF_WIFI_MODE_AP"])

    sysrnwfwifiapPwd = sysWifiRNWFComponent.createStringSymbol("SYS_WIFI_AP_PWD_NAME", sysrnwfwifiapEnable)
    sysrnwfwifiapPwd.setLabel("Password")
    sysrnwfwifiapPwd.setHelp(wifi_helpkeyword)
    sysrnwfwifiapPwd.setVisible(False)
    sysrnwfwifiapPwd.setDescription("Enter AP Mode Password.WPA2/WPA3 - Maximum key length is 63 characters.Minimum key length is 8 characters.")
    sysrnwfwifiapPwd.setDefaultValue("password")
    sysrnwfwifiapPwd.setDependencies(syswifiMenuVisible, ["SYS_RNWF_WIFI_MODE_AP"])    

    sysrnwfwifiapautoconnect = sysWifiRNWFComponent.createBooleanSymbol("SYS_RNWF_WIFI_AP_AUTOCONNECT", sysrnwfwifiapEnable)
    sysrnwfwifiapautoconnect.setLabel("Auto connect")
    sysrnwfwifiapautoconnect.setHelp(wifi_helpkeyword)
    sysrnwfwifiapautoconnect.setDefaultValue(False)
    sysrnwfwifiapautoconnect.setDescription("Auto Connect")   
    sysrnwfwifiapautoconnect.setDependencies(syswifiMenuVisible, ["SYS_RNWF_WIFI_MODE_AP"])

    sysrnwfwifiProvEnable = sysWifiRNWFComponent.createBooleanSymbol("SYS_RNWF_WIFI_MODE_PROV", sysrnwfwifiMode)
    sysrnwfwifiProvEnable.setLabel("Provision Mode")
    sysrnwfwifiProvEnable.setHelp(wifi_helpkeyword)
    sysrnwfwifiProvEnable.setDefaultValue(False)
    sysrnwfwifiProvEnable.setDescription("Enable Provision Mode Configuration")
    sysrnwfwifiProvEnable.setDependencies(syswifiAPautoMenu, ["SYS_RNWF_WIFI_MODE_PROV"])

    sysrnwfwifiprovSsid = sysWifiRNWFComponent.createStringSymbol("SYS_RNWF_WIFI_PROV_SSID_NAME", sysrnwfwifiProvEnable)
    sysrnwfwifiprovSsid.setLabel("SSID")
    sysrnwfwifiprovSsid.setHelp(wifi_helpkeyword)
    sysrnwfwifiprovSsid.setVisible(False)
    sysrnwfwifiprovSsid.setDescription("Enter AP Mode SSID.The maximum length is 32 characters")
    sysrnwfwifiprovSsid.setDefaultValue("DEMO_AP_SOFTAP")
    sysrnwfwifiprovSsid.setDependencies(syswifiMenuVisible, ["SYS_RNWF_WIFI_MODE_PROV"])

    sysrnwfwifiProvAuth = sysWifiRNWFComponent.createComboSymbol("SYS_RNWF_WIFI_PROV_SECURITY", sysrnwfwifiProvEnable, ["OPEN", "WPA2-MIXED" , "WPA2" , "WPA3-TRANS" , "WPA3"])
    sysrnwfwifiProvAuth.setLabel("Security")
    sysrnwfwifiProvAuth.setHelp(wifi_helpkeyword)
    sysrnwfwifiProvAuth.setVisible(False)
    sysrnwfwifiProvAuth.setDescription("Enter Provision Mode Security")
    sysrnwfwifiProvAuth.setDefaultValue("WPA2")
    sysrnwfwifiProvAuth.setDependencies(syswifiMenuVisible, ["SYS_RNWF_WIFI_MODE_PROV"])

    sysrnwfwifiProvPwd = sysWifiRNWFComponent.createStringSymbol("SYS_WIFI_PROV_PWD_NAME", sysrnwfwifiProvEnable)
    sysrnwfwifiProvPwd.setLabel("Password")
    sysrnwfwifiProvPwd.setHelp(wifi_helpkeyword)
    sysrnwfwifiProvPwd.setVisible(False)
    sysrnwfwifiProvPwd.setDescription("Enter AP Mode Password.WPA2/WPA3 - Maximum key length is 63 characters.Minimum key length is 8 characters.")
    sysrnwfwifiProvPwd.setDefaultValue("password")
    sysrnwfwifiProvPwd.setDependencies(syswifiMenuVisible, ["SYS_RNWF_WIFI_MODE_PROV"])
    
    sysrnwfwifiprovautoconnect = sysWifiRNWFComponent.createBooleanSymbol("SYS_RNWF_WIFI_PROV_AUTOCONNECT", sysrnwfwifiProvEnable)
    sysrnwfwifiprovautoconnect.setLabel("Auto connect")
    sysrnwfwifiprovautoconnect.setHelp(wifi_helpkeyword)
    sysrnwfwifiprovautoconnect.setDefaultValue(False)
    sysrnwfwifiprovautoconnect.setDescription("Auto Connect")
    sysrnwfwifiprovautoconnect.setDependencies(syswifiMenuVisible, ["SYS_RNWF_WIFI_MODE_PROV"])
    
    sysrnwfwifiProvMethod = sysWifiRNWFComponent.createComboSymbol("SYS_RNWF_WIFI_PROV_METHOD", sysrnwfwifiProvEnable, ["PROV_WEB_SERVER", "PROV_MOBILE_APP" ])
    sysrnwfwifiProvMethod.setLabel("Provision Method")
    sysrnwfwifiProvMethod.setHelp(wifi_helpkeyword)
    sysrnwfwifiProvMethod.setVisible(False)
    sysrnwfwifiProvMethod.setDescription("Enter Provision method")
    sysrnwfwifiProvMethod.setDefaultValue("PROV_MOBILE_APP")
    sysrnwfwifiProvMethod.setDependencies(syswifiMenuVisible, ["SYS_RNWF_WIFI_MODE_PROV"])

    sysrnwfwifiCallbackHandler = sysWifiRNWFComponent.createStringSymbol("SYS_RNWF_WIFI_CALLBACK_HANDLER", None)
    sysrnwfwifiCallbackHandler.setLabel("WiFi callback handler")
    sysrnwfwifiCallbackHandler.setHelp(wifi_helpkeyword)
    sysrnwfwifiCallbackHandler.setVisible(True)
    sysrnwfwifiCallbackHandler.setDescription("WiFi callback handler")
    sysrnwfwifiCallbackHandler.setDefaultValue("APP_WIFI_Callback")

    
    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    sysrnwfwifiSourceFile = sysWifiRNWFComponent.createFileSymbol("SYS_RNWF_WIFI_SOURCE", None)
    sysrnwfwifiSourceFile.setSourcePath("system/RNWF_wifi/templates/src/sys_rnwf_wifi_service.c.ftl")
    sysrnwfwifiSourceFile.setOutputName("sys_rnwf_wifi_service.c")
    sysrnwfwifiSourceFile.setDestPath("system/wifi/src")
    sysrnwfwifiSourceFile.setProjectPath("config/" + configName + "/system/wifi/")
    sysrnwfwifiSourceFile.setType("SOURCE")
    sysrnwfwifiSourceFile.setMarkup(True)
    sysrnwfwifiSourceFile.setEnabled(True)

    sysrnwfwifiHeaderFile = sysWifiRNWFComponent.createFileSymbol("SYS_RNWF_WIFI_HEADER", None)
    sysrnwfwifiHeaderFile.setSourcePath("system/RNWF_wifi/templates/sys_rnwf_wifi_service.h.ftl")
    sysrnwfwifiHeaderFile.setOutputName("sys_rnwf_wifi_service.h")
    sysrnwfwifiHeaderFile.setDestPath("system/wifi/")
    sysrnwfwifiHeaderFile.setProjectPath("config/" + configName + "/system/wifi/")
    sysrnwfwifiHeaderFile.setType("HEADER")
    sysrnwfwifiHeaderFile.setMarkup(True)
    sysrnwfwifiHeaderFile.setEnabled(True)
    
    sysrnwfInterfaceSourceFile = sysWifiRNWFComponent.createFileSymbol("SYS_RNWF_INTERFACE_SOURCE", None)
    sysrnwfInterfaceSourceFile.setSourcePath("system/RNWF_wifi/templates/src/sys_rnwf_interface.c.ftl")
    sysrnwfInterfaceSourceFile.setOutputName("sys_rnwf_interface.c")
    sysrnwfInterfaceSourceFile.setDestPath("system/inf/src")
    sysrnwfInterfaceSourceFile.setProjectPath("config/" + configName + "/system/inf/")
    sysrnwfInterfaceSourceFile.setType("SOURCE")
    sysrnwfInterfaceSourceFile.setMarkup(True)
    sysrnwfInterfaceSourceFile.setEnabled(True)

    sysrnwfInterfaceHeaderFile = sysWifiRNWFComponent.createFileSymbol("SYS_RNWF_INTERFACE_HEADER", None)
    sysrnwfInterfaceHeaderFile.setSourcePath("system/RNWF_wifi/templates/sys_rnwf_interface.h.ftl")
    sysrnwfInterfaceHeaderFile.setOutputName("sys_rnwf_interface.h")
    sysrnwfInterfaceHeaderFile.setDestPath("system/inf/")
    sysrnwfInterfaceHeaderFile.setProjectPath("config/" + configName + "/system/inf/")
    sysrnwfInterfaceHeaderFile.setType("HEADER")
    sysrnwfInterfaceHeaderFile.setMarkup(True)
    sysrnwfInterfaceHeaderFile.setEnabled(True)

    sysrnwfwifiSystemConfFile = sysWifiRNWFComponent.createFileSymbol("SYS_RNWF_WIFI_CONFIGURATION_H", None)
    sysrnwfwifiSystemConfFile.setType("STRING")
    sysrnwfwifiSystemConfFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
    sysrnwfwifiSystemConfFile.setSourcePath("system/RNWF_wifi/templates/system/system_config.h.ftl")
    sysrnwfwifiSystemConfFile.setMarkup(True)


    sysrnwfInterfaceSysSourceFile = sysWifiRNWFComponent.createFileSymbol("SYS_RNWF_INF_SYSTEM_SOURCE", None)
    sysrnwfInterfaceSysSourceFile.setSourcePath("system/RNWF_system/templates/src/sys_rnwf_system_service.c.ftl")
    sysrnwfInterfaceSysSourceFile.setOutputName("sys_rnwf_system_service.c")
    sysrnwfInterfaceSysSourceFile.setDestPath("system/")
    sysrnwfInterfaceSysSourceFile.setProjectPath("config/" + configName + "/system/")
    sysrnwfInterfaceSysSourceFile.setType("SOURCE")
    sysrnwfInterfaceSysSourceFile.setOverwrite(True)


    sysrnwfInterfaceSysHeaderFile = sysWifiRNWFComponent.createFileSymbol("SYS_RNWF_INF_SYSTEM_HEADER", None)
    sysrnwfInterfaceSysHeaderFile.setSourcePath("system/RNWF_system/templates/sys_rnwf_system_service.h.ftl")
    sysrnwfInterfaceSysHeaderFile.setOutputName("sys_rnwf_system_service.h")
    sysrnwfInterfaceSysHeaderFile.setDestPath("system/")
    sysrnwfInterfaceSysHeaderFile.setProjectPath("config/" + configName + "/system/")
    sysrnwfInterfaceSysHeaderFile.setType("HEADER")
    sysrnwfInterfaceSysHeaderFile.setOverwrite(True)

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

def syswifiMenuVisible(symbol, event):
    if (event["value"] == True):
        print("WIFI Menu Visible.")
        symbol.setVisible(True)
    else:
        print("WIFI Menu Invisible.")
        symbol.setVisible(False)

def syswifiSTASecurityMenu(symbol, event):
    print("syswifiSTASecurityMenu")

def syswifiAPSecurityMenu(symbol, event):
    print("syswifiAPSecurityMenu")

def syswifiChannelErr(symbol, event):
    print("syswifiChannelErr")

def syswifiSTAMenu(symbol, event):
    print("syswifiSTAMenu")

def syswifiAPMenu(symbol, event):
    print("syswifiAPMenu")

def syscomponentautoactivate(symbol, event):
    print("syscomponentautoactivate")
    if(Database.getComponentByID("BSP_SAM_E54_Xplained_Pro") == None):    
        res = Database.activateComponents(["BSP_SAM_E54_Xplained_Pro"])

    if(Database.getComponentByID("sercom2") == None):    
        res = Database.activateComponents(["sercom2"])
        Database.setSymbolValue("sercom2", "USART_RXPO", 1)
        setVal("sercom2", "USART_OPERATING_MODE", 2)
        Database.setSymbolValue("sercom2", "USART_TX_RING_BUFFER_SIZE", 4096)
        Database.setSymbolValue("sercom2", "USART_RX_RING_BUFFER_SIZE", 4096)
    if(Database.getComponentByID("sercom0") == None):    
        res = Database.activateComponents(["sercom0"])
        Database.setSymbolValue("sercom0", "USART_RXPO", 1)
        Database.setSymbolValue("sercom0", "USART_SFDE", True)
        Database.setSymbolValue("sercom0", "USART_BAUD_RATE", 230400)
        Database.setSymbolValue("sercom0", "USART_OPERATING_MODE", 2)

    if(Database.getComponentByID("HarmonyCore") == None):    
        res = Database.activateComponents(["HarmonyCore"])
        Database.setSymbolValue("HarmonyCore", "ENABLE_SYS_PORTS", True)
        Database.setSymbolValue("HarmonyCore", "ENABLE_SYS_RESET", True)

    Database.setSymbolValue("core","PIN_21_FUNCTION_TYPE","SERCOM0_PAD0")
    Database.setSymbolValue("core","PIN_22_FUNCTION_TYPE","SERCOM0_PAD1")
    Database.setSymbolValue("core","PIN_100_FUNCTION_TYPE","SERCOM2_PAD1")
    Database.setSymbolValue("core","PIN_101_FUNCTION_TYPE","SERCOM2_PAD0")       

    Database.setSymbolValue("core", "DMAC_ENABLE_CH_0", True)
    Database.setSymbolValue("core", "DMAC_CHCTRLA_TRIGSRC_CH_0", "SERCOM0_Transmit")
    Database.setSymbolValue("core", "DMAC_ENABLE_CH_1", True)
    Database.setSymbolValue("core", "DMAC_CHCTRLA_TRIGSRC_CH_1", "SERCOM0_Receive")
    Database.setSymbolValue("core", "DMAC_ENABLE_CH_2", True)
    Database.setSymbolValue("core", "DMAC_CHCTRLA_TRIGSRC_CH_2", "SERCOM2_Transmit")


    if(Database.getComponentByID("sys_console") == None):    
        res = Database.activateComponents(["sys_console"])
    if(Database.getComponentByID("sys_console_0") == None):    
        res = Database.activateComponents(["sys_console_0"])
    if(Database.getComponentByID("sys_debug") == None):    
        res = Database.activateComponents(["sys_debug"])
        print("sys_debug_SYS_CONSOLE_dependency....")
        autoConnectTableCon = [[ "sys_console_0","sys_console" , "sys_debug", "sys_debug_SYS_CONSOLE_dependency"]]
        res = Database.connectDependencies(autoConnectTableCon)
        autoConnectTableuart = [[ "sys_console_0","sys_console_UART_dependency" , "sercom2", "SERCOM2_UART"]]
        res = Database.connectDependencies(autoConnectTableuart)

def syswifiautoInclude(symbol, event):
    if(Database.getComponentByID("sysWifiRNWF") == None):    
        res = Database.activateComponents(["sysWifiRNWF"])
def syswifiprovautoInclude(symbol, event):
    if(Database.getComponentByID("sysWifiProvRNWF") == None):    
        res = Database.activateComponents(["sysWifiProvRNWF"])
    if(Database.getComponentByID("sysNetRNWF") == None):    
        res = Database.activateComponents(["sysNetRNWF"]) 
        Database.setSymbolValue("sysWifiRNWF", "SYS_RNWF_NET_SER_ENABLE", True)
def sysmqttautoInclude(symbol, event):
    if(Database.getComponentByID("sysMqttRNWF") == None):    
        res = Database.activateComponents(["sysMqttRNWF"])
def sysnetautoInclude(symbol, event):
    if(Database.getComponentByID("sysNetRNWF") == None):    
        res = Database.activateComponents(["sysNetRNWF"])
def sysotaautoInclude(symbol, event):
    if(Database.getComponentByID("sysOtaRNWF") == None):    
        res = Database.activateComponents(["sysOtaRNWF"])
    if(Database.getComponentByID("sysNetRNWF") == None):    
        res = Database.activateComponents(["sysNetRNWF"]) 
        Database.setSymbolValue("sysWifiRNWF", "SYS_RNWF_NET_SER_ENABLE", True)

def syswifiSTAautoMenu(symbol, event):
    print("syswifiSTAautoMenu")
    
def syswifiAPautoMenu(symbol, event):
    print("syswifiAPautoMenu")
 
def destroyComponent(component):
    print("destroyComponent")
