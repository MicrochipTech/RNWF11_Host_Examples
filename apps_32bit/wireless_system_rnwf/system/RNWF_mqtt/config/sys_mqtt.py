# coding: utf-8
"""*****************************************************************************
Copyright (C) 2020-2021 released Microchip Technology Inc.  All rights reserved.

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
global mqtt_helpkeyword
sysrnwfMqttSubTopicInstance = []
sysrnwfMqttMaxTopic = 5
sysmqttTopicNumPrev = 1

mqtt_helpkeyword = "mcc_h3_RNWF_mqtt_system_service_configurations"
################################################################################
#### Business Logic ####
################################################################################

################################################################################
#### Component ####
################################################################################
def instantiateComponent(sysrnwfMQTTRNWFComponent):
    global mqtt_helpkeyword

    # Basic Configuration
    sysrnwfMqttEnableCloudCons = sysrnwfMQTTRNWFComponent.createMenuSymbol("SYS_RNWF_MQTT_CLOUD_CONF", None)
    sysrnwfMqttEnableCloudCons.setLabel("Cloud Configuration")
    sysrnwfMqttEnableCloudCons.setHelp(mqtt_helpkeyword)
    sysrnwfMqttEnableCloudCons.setVisible(True)
		
    sysrnwfmqttBrokerName = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_CLOUD_URL", sysrnwfMqttEnableCloudCons)
    sysrnwfmqttBrokerName.setLabel("Cloud URL")
    sysrnwfmqttBrokerName.setHelp(mqtt_helpkeyword)
    sysrnwfmqttBrokerName.setVisible(True)
    sysrnwfmqttBrokerName.setDescription(" Configure Cloud provider endpoint / MQTT Broker URL")
    sysrnwfmqttBrokerName.setDefaultValue("g2-cert-dps.azure-devices-provisioning.net")
    sysrnwfmqttBrokerName.setDependencies(mqttSNIAutoMenu, ["SYS_RNWF_MQTT_CLOUD_URL"])

    sysrnwfmqttPort = sysrnwfMQTTRNWFComponent.createIntegerSymbol("SYS_RNWF_MQTT_CLOUD_PORT", sysrnwfMqttEnableCloudCons)
    sysrnwfmqttPort.setLabel("Cloud Port")
    sysrnwfmqttPort.setHelp(mqtt_helpkeyword)
    sysrnwfmqttPort.setMin(1)
    sysrnwfmqttPort.setMax(65535)
    sysrnwfmqttPort.setDescription("Configure Cloud/MQTT port")
    sysrnwfmqttPort.setDefaultValue(8883)

    sysrnwfMqttAzureDpsEnable = sysrnwfMQTTRNWFComponent.createBooleanSymbol("SYS_RNWF_MQTT_AZURE_DPS_ENABLE", sysrnwfMqttEnableCloudCons)
    sysrnwfMqttAzureDpsEnable.setLabel("Azure DPS Enable")
    sysrnwfMqttAzureDpsEnable.setHelp(mqtt_helpkeyword)
    sysrnwfMqttAzureDpsEnable.setVisible(True)
    sysrnwfMqttAzureDpsEnable.setDefaultValue(False)  

    sysrnwfmqttScopeId = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_SCOPE_ID", sysrnwfMqttAzureDpsEnable)
    sysrnwfmqttScopeId.setLabel("Scope Id")
    sysrnwfmqttScopeId.setHelp(mqtt_helpkeyword)
    sysrnwfmqttScopeId.setVisible(False)
    sysrnwfmqttScopeId.setDescription(" Provide Scope ID from Azure IoT Central portal for registered device")
    sysrnwfmqttScopeId.setDefaultValue("0ne00B5478B")
    sysrnwfmqttScopeId.setDependencies(sysMqttSubMenuVisible, ["SYS_RNWF_MQTT_AZURE_DPS_ENABLE"])

    sysrnwfmqttDeviceTemplate = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_DEVICE_TEMPLATE", sysrnwfMqttAzureDpsEnable)
    sysrnwfmqttDeviceTemplate.setLabel("Device Template")
    sysrnwfmqttDeviceTemplate.setHelp(mqtt_helpkeyword)
    sysrnwfmqttDeviceTemplate.setVisible(False)
    sysrnwfmqttDeviceTemplate.setDescription("Configure Cloud DPS specific device template")
    sysrnwfmqttDeviceTemplate.setDefaultValue("dtmi:com:Microchip:SAME54_XPRO;1")
    sysrnwfmqttDeviceTemplate.setDependencies(sysMqttSubMenuVisible, ["SYS_RNWF_MQTT_AZURE_DPS_ENABLE"])

    sysrnwfmqttUserName = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_CLOUD_USER_NAME", sysrnwfMqttEnableCloudCons)
    sysrnwfmqttUserName.setLabel("User Name")
    sysrnwfmqttUserName.setHelp(mqtt_helpkeyword)
    sysrnwfmqttUserName.setVisible(True)
    sysrnwfmqttUserName.setDescription("Configure Cloud Client user name")
    sysrnwfmqttUserName.setDefaultValue("0ne00B5478B/registrations/rnwf02_device_01/api-version=2019-03-31")

    sysrnwfmqttPassword = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_PASSWORD", sysrnwfMqttEnableCloudCons)
    sysrnwfmqttPassword.setLabel("Password")
    sysrnwfmqttPassword.setHelp(mqtt_helpkeyword)
    sysrnwfmqttPassword.setVisible(True)
    sysrnwfmqttPassword.setDescription("Enter the cloud client password")
    sysrnwfmqttPassword.setDefaultValue("")

    sysrnwfmqttClientId = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_CLIENT_ID", sysrnwfMqttEnableCloudCons)
    sysrnwfmqttClientId.setLabel("Client Id")
    sysrnwfmqttClientId.setHelp(mqtt_helpkeyword)
    sysrnwfmqttClientId.setVisible(True)
    sysrnwfmqttClientId.setDescription("MQTT Client Id which should be unique for the MQTT Broker. If empty, the id will be randomly generated")
    sysrnwfmqttClientId.setDefaultValue("rnwf02_device_01")

    sysrnwfMqttAzurePublish = sysrnwfMQTTRNWFComponent.createBooleanSymbol("SYS_RNWF_MQTT_AZURE_PUBLISH", sysrnwfMqttEnableCloudCons)
    sysrnwfMqttAzurePublish.setLabel("Publish")
    sysrnwfMqttAzurePublish.setHelp(mqtt_helpkeyword)
    sysrnwfMqttAzurePublish.setVisible(True)
    sysrnwfMqttAzurePublish.setDefaultValue(False) 

    sysrnwfmqttPubTopicName = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_PUB_TOPIC_NAME", sysrnwfMqttAzurePublish)
    sysrnwfmqttPubTopicName.setLabel("Publish Topic Name")
    sysrnwfmqttPubTopicName.setHelp(mqtt_helpkeyword)
    sysrnwfmqttPubTopicName.setVisible(False)
    sysrnwfmqttPubTopicName.setDescription("Enter the cloud client password")
    sysrnwfmqttPubTopicName.setDefaultValue("devices/rnwf02_device_01/messages/events/")
    sysrnwfmqttPubTopicName.setDependencies(sysMqttSubMenuVisible, ["SYS_RNWF_MQTT_AZURE_PUBLISH"])

    sysrnwfMqttQosType = sysrnwfMQTTRNWFComponent.createComboSymbol("SYS_RNWF_MQTT_QOS_TYPE", sysrnwfMqttAzurePublish , ["QOS0", "QOS1" , "QOS2"])
    sysrnwfMqttQosType.setLabel("Pub. QoS")
    sysrnwfMqttQosType.setHelp(mqtt_helpkeyword)
    sysrnwfMqttQosType.setVisible(False)
    sysrnwfMqttQosType.setDescription("QoS type for the message ::RNWF_MQTT_QOS_t")
    sysrnwfMqttQosType.setDefaultValue("QOS0")
    sysrnwfMqttQosType.setDependencies(sysMqttSubMenuVisible, ["SYS_RNWF_MQTT_AZURE_PUBLISH"])

    sysrnwfMqttAzureRetainFlag = sysrnwfMQTTRNWFComponent.createBooleanSymbol("SYS_RNWF_MQTT_AZURE_RETAIN_FLAG", sysrnwfMqttAzurePublish)
    sysrnwfMqttAzureRetainFlag.setLabel("Retain Flag")
    sysrnwfMqttAzureRetainFlag.setHelp(mqtt_helpkeyword)
    sysrnwfMqttAzureRetainFlag.setVisible(False)
    sysrnwfMqttAzureRetainFlag.setDefaultValue(False) 
    sysrnwfMqttAzureRetainFlag.setDependencies(sysMqttSubMenuVisible, ["SYS_RNWF_MQTT_AZURE_PUBLISH"])
    
    sysrnwfMqttAzureKeepAlive = sysrnwfMQTTRNWFComponent.createBooleanSymbol("SYS_RNWF_MQTT_AZURE_KEEP_ALIVE", sysrnwfMqttEnableCloudCons)
    sysrnwfMqttAzureKeepAlive.setLabel("Keep Alive")
    sysrnwfMqttAzureKeepAlive.setHelp(mqtt_helpkeyword)
    sysrnwfMqttAzureKeepAlive.setVisible(True)
    sysrnwfMqttAzureKeepAlive.setDefaultValue(False)

    sysrnwfMqttAzureKeepAliveInt = sysrnwfMQTTRNWFComponent.createIntegerSymbol("SYS_RNWF_MQTT_KEEP_ALIVE_INT", sysrnwfMqttAzureKeepAlive)
    sysrnwfMqttAzureKeepAliveInt.setLabel("Keep Alive Interval")
    sysrnwfMqttAzureKeepAliveInt.setHelp(mqtt_helpkeyword)
    sysrnwfMqttAzureKeepAliveInt.setVisible(False)
    sysrnwfMqttAzureKeepAliveInt.setDescription("Configure the field in the range of 1-1000 (in seconds)")
    sysrnwfMqttAzureKeepAliveInt.setMin(0)
    sysrnwfMqttAzureKeepAliveInt.setMax(10000)
    sysrnwfMqttAzureKeepAliveInt.setDefaultValue(60)
    sysrnwfMqttAzureKeepAliveInt.setDependencies(sysMqttSubMenuVisible, ["SYS_RNWF_MQTT_AZURE_KEEP_ALIVE"])

    sysrnwfMqttAzureSubscribe = sysrnwfMQTTRNWFComponent.createBooleanSymbol("SYS_RNWF_MQTT_AZURE_SUBSCRIBE", sysrnwfMqttEnableCloudCons)
    sysrnwfMqttAzureSubscribe.setLabel("Subscribe")
    sysrnwfMqttAzureSubscribe.setHelp(mqtt_helpkeyword)
    sysrnwfMqttAzureSubscribe.setVisible(True)
    sysrnwfMqttAzureSubscribe.setDefaultValue(False) 

    sysrnwfMqttAzureTotalSubTopics = sysrnwfMQTTRNWFComponent.createIntegerSymbol("SYS_RNWF_MQTT_TOTAL_SUB_TOPICS", sysrnwfMqttAzureSubscribe)
    sysrnwfMqttAzureTotalSubTopics.setLabel("Total Subscribe Topics")
    sysrnwfMqttAzureTotalSubTopics.setHelp(mqtt_helpkeyword)
    sysrnwfMqttAzureTotalSubTopics.setVisible(False)
    sysrnwfMqttAzureTotalSubTopics.setDescription("Number of subscribe tipics (1 to 5)")
    sysrnwfMqttAzureTotalSubTopics.setMin(1)
    sysrnwfMqttAzureTotalSubTopics.setMax(sysrnwfMqttMaxTopic)
    sysrnwfMqttAzureTotalSubTopics.setDefaultValue(3)
    sysrnwfMqttAzureTotalSubTopics.setDependencies(sysMqttSubMenuVisible, ["SYS_RNWF_MQTT_AZURE_SUBSCRIBE"])


    # Get Size of Each Slot
    for slot in range(0,sysrnwfMqttMaxTopic):
       sysrnwfMqttSubTopicInstance.append(sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_SUB_TOPIC"+str(slot),sysrnwfMqttAzureTotalSubTopics))
       sysrnwfMqttSubTopicInstance[slot].setLabel("Sub Topic "+ str(slot))
       sysrnwfMqttAzureTotalSubTopics.setHelp(mqtt_helpkeyword)
       print(sysrnwfMqttAzureTotalSubTopics.getValue())

       if (slot < sysrnwfMqttAzureTotalSubTopics.getValue()):
            sysrnwfMqttSubTopicInstance[slot].setVisible(True)
       else:
            sysrnwfMqttSubTopicInstance[slot].setVisible(False)
       sysrnwfMqttSubTopicInstance[slot].setDependencies(sysmqttTopicInstanceEnable,["SYS_RNWF_MQTT_AZURE_SUBSCRIBE","SYS_RNWF_MQTT_TOTAL_SUB_TOPICS"])


    sysrnwfMqttAzureSubQos = sysrnwfMQTTRNWFComponent.createComboSymbol("SYS_RNWF_MQTT_SUB_QOS_TYPE", sysrnwfMqttAzureSubscribe , ["QOS0", "QOS1" , "QOS2"])
    sysrnwfMqttAzureSubQos.setLabel("Sub. QoS")
    sysrnwfMqttAzureSubQos.setHelp(mqtt_helpkeyword)
    sysrnwfMqttAzureSubQos.setVisible(False)
    sysrnwfMqttAzureSubQos.setDescription("QoS type for the message ::RNWF_MQTT_QOS_t")
    sysrnwfMqttAzureSubQos.setDefaultValue("QOS0")
    sysrnwfMqttAzureSubQos.setDependencies(sysMqttSubMenuVisible, ["SYS_RNWF_MQTT_AZURE_SUBSCRIBE"])

    sysrnwfMqttAzureEnableTls = sysrnwfMQTTRNWFComponent.createBooleanSymbol("SYS_MQTT_AZURE_ENABLE_TLS", sysrnwfMqttEnableCloudCons)
    sysrnwfMqttAzureEnableTls.setLabel("Enable TLS")
    sysrnwfMqttAzureEnableTls.setHelp(mqtt_helpkeyword)
    sysrnwfMqttAzureEnableTls.setVisible(True)
    sysrnwfMqttAzureEnableTls.setDefaultValue(False)

    sysrnwfMqttTlsServerCert = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_SERVER_CERT", sysrnwfMqttAzureEnableTls)
    sysrnwfMqttTlsServerCert.setLabel("Server Certificate")
    sysrnwfMqttTlsServerCert.setHelp(mqtt_helpkeyword)
    sysrnwfMqttTlsServerCert.setVisible(False)
    sysrnwfMqttTlsServerCert.setDescription("TLS Server Certificate Name")
    sysrnwfMqttTlsServerCert.setDefaultValue("AmazonRootCA1")
    sysrnwfMqttTlsServerCert.setDependencies(sysMqttSubMenuVisible, ["SYS_MQTT_AZURE_ENABLE_TLS"])
    
    sysrnwfMqttTlsDeviceCert = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_DEVICE_CERT", sysrnwfMqttAzureEnableTls)
    sysrnwfMqttTlsDeviceCert.setLabel("Device Certificate")
    sysrnwfMqttTlsDeviceCert.setHelp(mqtt_helpkeyword)
    sysrnwfMqttTlsDeviceCert.setVisible(False)
    sysrnwfMqttTlsDeviceCert.setDescription("TLS Device Certificate Name")
    sysrnwfMqttTlsDeviceCert.setDefaultValue("rnwf02_device_01")
    sysrnwfMqttTlsDeviceCert.setDependencies(sysMqttSubMenuVisible, ["SYS_MQTT_AZURE_ENABLE_TLS"])

    sysrnwfMqttTlsDeviceKey = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_DEVICE_KEY", sysrnwfMqttAzureEnableTls)
    sysrnwfMqttTlsDeviceKey.setLabel("Device Key")
    sysrnwfMqttTlsDeviceKey.setHelp(mqtt_helpkeyword)
    sysrnwfMqttTlsDeviceKey.setVisible(False)
    sysrnwfMqttTlsDeviceKey.setDescription("TLS Device Key")
    sysrnwfMqttTlsDeviceKey.setDefaultValue("rnwf02_device_01")
    sysrnwfMqttTlsDeviceKey.setDependencies(sysMqttSubMenuVisible, ["SYS_MQTT_AZURE_ENABLE_TLS"])  

    sysrnwfMqttTlsDeviceKeyPassword = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_DEVICE_KEY_PSK", sysrnwfMqttAzureEnableTls)
    sysrnwfMqttTlsDeviceKeyPassword.setLabel("Device Key Password")
    sysrnwfMqttTlsDeviceKeyPassword.setHelp(mqtt_helpkeyword)
    sysrnwfMqttTlsDeviceKeyPassword.setVisible(False)
    sysrnwfMqttTlsDeviceKeyPassword.setDescription("TLS Device Key Password")
    sysrnwfMqttTlsDeviceKeyPassword.setDefaultValue("")
    sysrnwfMqttTlsDeviceKeyPassword.setDependencies(sysMqttSubMenuVisible, ["SYS_MQTT_AZURE_ENABLE_TLS"]) 
    
    sysrnwfMqttTlsServerName = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_TLS_SERVER_NAME", sysrnwfMqttAzureEnableTls)
    sysrnwfMqttTlsServerName.setLabel("Server Name")
    sysrnwfMqttTlsServerName.setHelp(mqtt_helpkeyword)
    sysrnwfMqttTlsServerName.setVisible(False)
    sysrnwfMqttTlsServerName.setDescription("TLS Server name")
    sysrnwfMqttTlsServerName.setDefaultValue("")
    sysrnwfMqttTlsServerName.setDependencies(sysMqttSubMenuVisible, ["SYS_MQTT_AZURE_ENABLE_TLS"])

    sysrnwfMqttTlsDomainName = sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_TLS_DOMAIN_NAME", sysrnwfMqttAzureEnableTls)
    sysrnwfMqttTlsDomainName.setLabel("Domain Name")
    sysrnwfMqttTlsDomainName.setHelp(mqtt_helpkeyword)
    sysrnwfMqttTlsDomainName.setVisible(False)
    sysrnwfMqttTlsDomainName.setDescription("TLS Domain name")
    sysrnwfMqttTlsDomainName.setDefaultValue("")
    sysrnwfMqttTlsDomainName.setDependencies(sysMqttSubMenuVisible, ["SYS_MQTT_AZURE_ENABLE_TLS"])

    sysrnwfmqttCallbackHandler= sysrnwfMQTTRNWFComponent.createStringSymbol("SYS_RNWF_MQTT_CALLBACK_HANDLER", None)
    sysrnwfmqttCallbackHandler.setLabel("MQTT Callback Handler ")
    sysrnwfmqttCallbackHandler.setHelp(mqtt_helpkeyword)
    sysrnwfmqttCallbackHandler.setVisible(True)
    sysrnwfmqttCallbackHandler.setDescription("MQTT Callback Handler ")
    sysrnwfmqttCallbackHandler.setDefaultValue("APP_MQTT_Callback")

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    sysrnwfmqttHeaderFile = sysrnwfMQTTRNWFComponent.createFileSymbol("SYS_RNWF_MQTT_HEADER", None)
    sysrnwfmqttHeaderFile.setSourcePath("system/RNWF_mqtt/templates/sys_rnwf_mqtt_service.h.ftl")
    sysrnwfmqttHeaderFile.setOutputName("sys_rnwf_mqtt_service.h")
    sysrnwfmqttHeaderFile.setDestPath("system/mqtt/")
    sysrnwfmqttHeaderFile.setProjectPath("config/" + configName + "/system/mqtt/")
    sysrnwfmqttHeaderFile.setType("HEADER")
    sysrnwfmqttHeaderFile.setOverwrite(True)


    
    print("Mqtt Service Component Header Path %s", sysrnwfmqttHeaderFile.getProjectPath())
	
    sysrnwfmqttSourceFile = sysrnwfMQTTRNWFComponent.createFileSymbol("SYS_RNWF_MQTT_SOURCE", None)
    sysrnwfmqttSourceFile.setSourcePath("system/RNWF_mqtt/templates/src/sys_rnwf_mqtt_service.c.ftl")
    sysrnwfmqttSourceFile.setOutputName("sys_rnwf_mqtt_service.c")
    sysrnwfmqttSourceFile.setDestPath("system/mqtt/src")
    sysrnwfmqttSourceFile.setProjectPath("config/" + configName + "/system/mqtt/")
    sysrnwfmqttSourceFile.setType("SOURCE")
    sysrnwfmqttSourceFile.setOverwrite(True)

    print("Mqtt Service Component Header Path %s", sysrnwfmqttHeaderFile.getProjectPath())
		
    sysrnwfmqttSystemConfigFile = sysrnwfMQTTRNWFComponent.createFileSymbol("SYS_RNWF_CONSOLE_SYS_CONFIG", None)
    sysrnwfmqttSystemConfigFile.setType("STRING")
    sysrnwfmqttSystemConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_SYSTEM_SERVICE_CONFIGURATION")
    sysrnwfmqttSystemConfigFile.setSourcePath("system/RNWF_mqtt/templates/system/system_config.h.ftl")
    sysrnwfmqttSystemConfigFile.setMarkup(True)

############################################################################
#### Dependency ####
############################################################################


def sysmqttTopicInstanceEnable(symbol, event):
    global sysmqttTopicNumPrev
    print("Start sysmqttTopicInstanceEnable")
    if(event["id"] == "SYS_RNWF_MQTT_AZURE_SUBSCRIBE"):
        mqttTopicEnable = Database.getSymbolValue("sysrnwfMqttAzureSubscribe","SYS_RNWF_MQTT_AZURE_SUBSCRIBE")
        fileIndex = int(filesymbol.getID().strip("SYS_RNWF_MQTT_SUB_TOPIC"))
        print("File Slot " + str(fileIndex))
        print(sysmqttTopicNumPrev)
        if(mqttTopicEnable == True):
            if(fileIndex < sysmqttTopicNumPrev ):
                symbol.setVisible(True)
        else:
            symbol.setVisible(False)
 
    else:
        print(symbol.getID())
        print(event["id"])
        sysmqttTopicNumValue = event["value"]
        print(sysmqttTopicNumValue)
        print(sysmqttTopicNumPrev)
        if (sysmqttTopicNumValue > sysmqttTopicNumPrev):
            sysrnwfMqttSubTopicInstance[sysmqttTopicNumPrev].setVisible(True)
            sysrnwfMqttSubTopicInstance[sysmqttTopicNumPrev].setValue("")
            sysmqttTopicNumPrev = sysmqttTopicNumPrev + 1
        else:
            if(sysmqttTopicNumValue < sysmqttTopicNumPrev):
                sysmqttTopicNumPrev = sysmqttTopicNumPrev - 1
                sysrnwfMqttSubTopicInstance[sysmqttTopicNumPrev].setVisible(False)
                sysrnwfMqttSubTopicInstance[sysmqttTopicNumPrev].setValue("")
                print("Set False " + str(sysmqttTopicNumPrev))
 
            else:
                print("Do Nothing "+ str(sysmqttTopicNumPrev))
    print("END sysmqttTopicInstanceEnable")




def genAppCode(symbol, event):
    print("genAppCode.")
        
def setVal(component, symbol, value):
    print("setVal.")
        

def sysMqttSubMenuVisible(symbol, event):
    print("sysMqttSubMenuVisible.")
    if (event["value"] == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)


def onAttachmentConnected(source, target):
    print("onAttachmentConnected.")


def onAttachmentDisconnected(source, target):
    print(onAttachmentDisconnected)

def mqttTlsAutoMenu(symbol, event):
    print("mqttTlsAutoMenu.")

def mqttSNIAutoMenu(symbol, event):
    print("mqttSNIAutoMenu.")

def mqttALPNAutoMenu(symbol, event):
    print("mqttALPNAutoMenu.")

def mqttIntfAutoMenu(symbol, event):
    print("mqttIntfAutoMenu.")









def finalizeComponent(sysrnwfMQTTRNWFComponent):
    print("finalizeComponent.")
        
