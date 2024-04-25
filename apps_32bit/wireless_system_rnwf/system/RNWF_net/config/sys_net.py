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
global net_helpkeyword
sysrnwfnetMaxSockets = 2
sysrnwfnetSocketInstance = []
sysrnwfnetSocketPrev = 1
sysrnwfnetBind = [] 
sysrnwfnetSockPort = []
sysrnwfnetSockType = []
sysrnwfnetSockAddr = []
sysrnwfnetMode = []
sysrnwfnetEnableTls = []
sysrnwfnetTlsRootCert = []
sysrnwfnetTlsDevCertificate = []
sysrnwfnetTlsDevKey = []
sysrnwfnetTlsDevKeyPwd = []
sysrnwfnetTlsServerName = []
sysrnwfnetTlsDomainName = []


net_helpkeyword = "mcc_h3_RNWF_net_system_service_configurations"
################################################################################
#### Business Logic ####
################################################################################

################################################################################
#### Component ####
################################################################################
def instantiateComponent(sysrnwfNetComponent):
    global net_helpkeyword

    sysrnwfnetEnableErrMsg = sysrnwfNetComponent.createCommentSymbol("SYS_RNWF_NET_ERR", None)
    sysrnwfnetEnableErrMsg.setLabel("**Placeholder for error display")
    sysrnwfnetEnableErrMsg.setHelp(net_helpkeyword)
    sysrnwfnetEnableErrMsg.setVisible(False)

    sysrnwfnetSocketEnable = sysrnwfNetComponent.createBooleanSymbol("SYS_RNWF_NET_SOCK_CONF", None)
    sysrnwfnetSocketEnable.setLabel("Socket Configurations")
    sysrnwfnetSocketEnable.setHelp(net_helpkeyword)
    sysrnwfnetSocketEnable.setDescription("Debug - Logs and commands")
    sysrnwfnetSocketEnable.setDefaultValue(True)

    sysrnwfnetNoOfSocks = sysrnwfNetComponent.createIntegerSymbol("SYS_RNWF_NET_NO_OF_SOCKS", sysrnwfnetSocketEnable)
    sysrnwfnetNoOfSocks.setLabel("Number of Sockets ")
    sysrnwfnetNoOfSocks.setHelp(net_helpkeyword)
    sysrnwfnetNoOfSocks.setVisible(True)
    sysrnwfnetNoOfSocks.setDefaultValue(1)
    sysrnwfnetNoOfSocks.setMin(1)
    sysrnwfnetNoOfSocks.setMax(sysrnwfnetMaxSockets)
    sysrnwfnetNoOfSocks.setDependencies(sysnetSubMenuVisible, ["SYS_RNWF_NET_SOCK_CONF"])

    # Get Size of Each Slot
    for slot in range(0,sysrnwfnetMaxSockets):
        sysrnwfnetSocketInstance.append(sysrnwfNetComponent.createMenuSymbol("SYS_RNWF_SOCKET"+str(slot), sysrnwfnetNoOfSocks))
        sysrnwfnetSocketInstance[slot].setLabel("Socket "+ str(slot))
        sysrnwfnetNoOfSocks.setHelp(net_helpkeyword)

#        sysrnwfnetBind.append(sysrnwfNetComponent.createComboSymbol("SYS_RNWF_NET_BIND_TYPE"+str(slot), sysrnwfnetSocketInstance[slot], ["LOCAL", "REMOTE", "MCAST", "NONE"]))
#        sysrnwfnetBind[slot].setLabel("Bind type")
#        sysrnwfnetBind[slot].setHelp(net_helpkeyword)
#        sysrnwfnetBind[slot].setDefaultValue("LOCAL")

        sysrnwfnetSockPort.append(sysrnwfNetComponent.createIntegerSymbol("SYS_RNWF_NET_SOCK_PORT"+str(slot), sysrnwfnetSocketInstance[slot]))    
        sysrnwfnetSockPort[slot].setLabel("Socket Port")
        sysrnwfnetSockPort[slot].setHelp(net_helpkeyword)
        sysrnwfnetSockPort[slot].setDefaultValue(6666)
        sysrnwfnetSockPort[slot].setMin(1)
        sysrnwfnetSockPort[slot].setMax(65535)

        sysrnwfnetSockType.append(sysrnwfNetComponent.createComboSymbol("SYS_RNWF_NET_SOCK_TYPE"+str(slot), sysrnwfnetSocketInstance[slot], ["UDP", "TCP"]))
        sysrnwfnetSockType[slot].setLabel("Ip Protocol")
        sysrnwfnetSockType[slot].setHelp(net_helpkeyword)
        sysrnwfnetSockType[slot].setDefaultValue("TCP")

        sysrnwfnetMode.append(sysrnwfNetComponent.createComboSymbol("SYS_RNWF_NET_MODE"+str(slot), sysrnwfnetSocketInstance[slot], ["CLIENT", "SERVER"]))
        sysrnwfnetMode[slot].setLabel("Mode")
        sysrnwfnetMode[slot].setHelp(net_helpkeyword)
        sysrnwfnetMode[slot].setDefaultValue("CLIENT")

        sysrnwfnetSockAddr.append(sysrnwfNetComponent.createStringSymbol("SYS_RNWF_NET_SOCK_ADDR"+str(slot),  sysrnwfnetSocketInstance[slot]))
        sysrnwfnetSockAddr[slot].setLabel("Server IP")
        sysrnwfnetSockAddr[slot].setHelp(net_helpkeyword)
        sysrnwfnetSockAddr[slot].setDescription("Socket IP Address")
        sysrnwfnetSockAddr[slot].setDefaultValue("")

        sysrnwfnetEnableTls.append(sysrnwfNetComponent.createBooleanSymbol("SYS_RNWF_NET_ENABLE_TLS"+str(slot), sysrnwfnetSocketInstance[slot]))
        sysrnwfnetEnableTls[slot].setLabel("Enable TLS")
        sysrnwfnetEnableTls[slot].setHelp(net_helpkeyword)
        sysrnwfnetEnableTls[slot].setDefaultValue(False)

        sysrnwfnetTlsRootCert.append(sysrnwfNetComponent.createStringSymbol("SYS_RNWF_NET_ROOT_CERT"+str(slot), sysrnwfnetEnableTls[slot]))
        sysrnwfnetTlsRootCert[slot].setLabel("Root CA")
        sysrnwfnetTlsRootCert[slot].setHelp(net_helpkeyword)
        sysrnwfnetTlsRootCert[slot].setDescription("TLS Root Certificate Name")
        sysrnwfnetTlsRootCert[slot].setDefaultValue("AmazonRootCA1")
        sysrnwfnetTlsRootCert[slot].setDependencies(sysnetSubMenuVisible, ["SYS_RNWF_NET_ENABLE_TLS"+str(slot)])

        sysrnwfnetTlsDevCertificate.append(sysrnwfNetComponent.createStringSymbol("SYS_RNWF_NET_DEVICE_CERTIFICATE"+str(slot), sysrnwfnetEnableTls[slot]))
        sysrnwfnetTlsDevCertificate[slot].setLabel("Device Certificate")
        sysrnwfnetTlsDevCertificate[slot].setHelp(net_helpkeyword)
        sysrnwfnetTlsDevCertificate[slot].setDescription("TLS Key Name")
        sysrnwfnetTlsDevCertificate[slot].setDefaultValue("")
        sysrnwfnetTlsDevCertificate[slot].setDependencies(sysnetSubMenuVisible, ["SYS_RNWF_NET_ENABLE_TLS"+str(slot)])

        sysrnwfnetTlsDevKey.append(sysrnwfNetComponent.createStringSymbol("SYS_RNWF_NET_DEVICE_KEY"+str(slot), sysrnwfnetEnableTls[slot]))
        sysrnwfnetTlsDevKey[slot].setLabel("Device Key")
        sysrnwfnetTlsDevKey[slot].setHelp(net_helpkeyword)
        sysrnwfnetTlsDevKey[slot].setDescription("Device key ")
        sysrnwfnetTlsDevKey[slot].setDefaultValue("")
        sysrnwfnetTlsDevKey[slot].setDependencies(sysnetSubMenuVisible, ["SYS_RNWF_NET_ENABLE_TLS"+str(slot)])
        
        sysrnwfnetTlsDevKeyPwd.append(sysrnwfNetComponent.createStringSymbol("SYS_RNWD_NET_DEVICE_KEY_PWD"+str(slot), sysrnwfnetEnableTls[slot]))
        sysrnwfnetTlsDevKeyPwd[slot].setLabel("Device Key Password")
        sysrnwfnetTlsDevKeyPwd[slot].setHelp(net_helpkeyword)
        sysrnwfnetTlsDevKeyPwd[slot].setDescription("Device key ")
        sysrnwfnetTlsDevKeyPwd[slot].setDefaultValue("")
        sysrnwfnetTlsDevKeyPwd[slot].setDependencies(sysnetSubMenuVisible, ["SYS_RNWF_NET_ENABLE_TLS"+str(slot)])
        
        sysrnwfnetTlsServerName.append(sysrnwfNetComponent.createStringSymbol("SYS_RNWF_NET_SERVER_NAME"+str(slot), sysrnwfnetEnableTls[slot]))
        sysrnwfnetTlsServerName[slot].setLabel("Server Name")
        sysrnwfnetTlsServerName[slot].setHelp(net_helpkeyword)
        sysrnwfnetTlsServerName[slot].setDescription("Enter the Server NAme")
        sysrnwfnetTlsServerName[slot].setDefaultValue("s3-us-west-2.amazonaws.com")
        sysrnwfnetTlsServerName[slot].setDependencies(sysnetSubMenuVisible, ["SYS_RNWF_NET_ENABLE_TLS"+str(slot)])
        

        sysrnwfnetTlsDomainName.append(sysrnwfNetComponent.createStringSymbol("SYS_RNWF_NET_DOMAIN_NAME"+str(slot), sysrnwfnetEnableTls[slot]))
        sysrnwfnetTlsDomainName[slot].setLabel("Domain Name")
        sysrnwfnetTlsDomainName[slot].setHelp(net_helpkeyword)
        sysrnwfnetTlsDomainName[slot].setDescription("Domain Name")
        sysrnwfnetTlsDomainName[slot].setDefaultValue("")
        sysrnwfnetTlsDomainName[slot].setDependencies(sysnetSubMenuVisible, ["SYS_RNWF_NET_ENABLE_TLS"+str(slot)])
        
        if (slot < sysrnwfnetNoOfSocks.getValue()):
            sysrnwfnetSocketInstance[slot].setVisible(True)
        else:
            sysrnwfnetSocketInstance[slot].setVisible(False)
        sysrnwfnetSocketInstance[slot].setDependencies(sysrnwfnetSlotInstanceEnable,["SYS_RNWF_NET_SOCK_CONF","SYS_RNWF_NET_NO_OF_SOCKS"])


    sysrnwfnetSocketCallbackHandler = sysrnwfNetComponent.createStringSymbol("SYS_RNWF_NET_SOCKET_CALLBACK_HANDLER", None)
    sysrnwfnetSocketCallbackHandler.setLabel("Net Socket Callback handler")
    sysrnwfnetSocketCallbackHandler.setHelp(net_helpkeyword)
    sysrnwfnetSocketCallbackHandler.setVisible(True)
    sysrnwfnetSocketCallbackHandler.setDescription("Enter the name of net socket callback handler")
    sysrnwfnetSocketCallbackHandler.setDefaultValue("APP_SOCKET_Callback")

    ############################################################################
    #### Code Generation ####
    ############################################################################
    configName = Variables.get("__CONFIGURATION_NAME")

    sysrnwfnetHeaderFile = sysrnwfNetComponent.createFileSymbol("SYS_RNWF_NET_HEADER", None)
    sysrnwfnetHeaderFile.setSourcePath("system/RNWF_net/templates/sys_rnwf_net_service.h.ftl")
    sysrnwfnetHeaderFile.setOutputName("sys_rnwf_net_service.h")
    sysrnwfnetHeaderFile.setDestPath("system/net/")
    sysrnwfnetHeaderFile.setProjectPath("config/" + configName + "/system/net/")
    sysrnwfnetHeaderFile.setType("HEADER")
    sysrnwfnetHeaderFile.setOverwrite(True)

    print("Network Service Component Header Path %s", sysrnwfnetHeaderFile.getProjectPath())
	
    sysrnwfnetSourceFile = sysrnwfNetComponent.createFileSymbol("SYS_RNWF_NET_SOURCE", None)
    sysrnwfnetSourceFile.setSourcePath("system/RNWF_net/templates/src/sys_rnwf_net_service.c.ftl")
    sysrnwfnetSourceFile.setOutputName("sys_rnwf_net_service.c")
    sysrnwfnetSourceFile.setDestPath("system/net/src")
    sysrnwfnetSourceFile.setProjectPath("config/" + configName + "/system/net/")
    sysrnwfnetSourceFile.setType("SOURCE")
    sysrnwfnetSourceFile.setOverwrite(True)

    sysrnwfnetSystemConfigFile = sysrnwfNetComponent.createFileSymbol("SYS_RNWF_CONSOLE_SYS_CONFIG", None)
    sysrnwfnetSystemConfigFile.setType("STRING")
    sysrnwfnetSystemConfigFile.setOutputName("core.LIST_SYSTEM_CONFIG_H_SYSTEM_SERVICE_CONFIGURATION")
    sysrnwfnetSystemConfigFile.setSourcePath("system/RNWF_net/templates/system/system_config.h.ftl")
    sysrnwfnetSystemConfigFile.setMarkup(True)

############################################################################
#### Dependency ####
############################################################################
#Set symbols of other components
def sysrnwfnetSlotInstanceEnable(symbol, event):
    global sysrnwfnetSocketPrev
    print("Start sysotaFileInstanceEnable")
    if(event["id"] == "SYS_RNWF_NET_SOCK_CONF"):
        otafileEnable = Database.getSymbolValue("sysrnwfnetSocketEnable","SYS_RNWF_NET_SOCK_CONF")
        fileIndex = int(filesymbol.getID().strip("SYS_RNWF_SOCKET"))
        print("File Slot " + str(fileIndex))
        print(sysrnwfnetSocketPrev)
        # print(tcpipDhcpsInstancesNumPrev)
        if(otafileEnable == True):
            if(fileIndex < sysrnwfnetSocketPrev ):
                symbol.setVisible(True)
        else:
            symbol.setVisible(False)

    else:
        print(symbol.getID())
        print(event["id"])
        sysotaFileNumValue = event["value"]
        print(sysotaFileNumValue)
        print(sysrnwfnetSocketPrev)
        if (sysotaFileNumValue > sysrnwfnetSocketPrev):
            sysrnwfnetSocketInstance[sysrnwfnetSocketPrev].setVisible(True)
            sysrnwfnetSocketPrev = sysrnwfnetSocketPrev + 1
        else:
            if(sysotaFileNumValue < sysrnwfnetSocketPrev):
                sysrnwfnetSocketPrev = sysrnwfnetSocketPrev - 1
                sysrnwfnetSocketInstance[sysrnwfnetSocketPrev].setVisible(False)
                print("Set False " + str(sysrnwfnetSocketPrev))

            else:
                print("Do Nothing "+ str(sysrnwfnetSocketPrev))
    
    print("END sysotaFileInstanceEnable")

def setVisible_OnValueChanged(symbol, event):
    symbol.setVisible(event['value'])
def sysnetSubMenuVisible(symbol, event):
    print("sysnetSubMenuVisible.")
    if (event["value"] == True):
        symbol.setVisible(True)
    else:
        symbol.setVisible(False)

        

    
