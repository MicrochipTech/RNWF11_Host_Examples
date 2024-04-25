######################  RNWF Wireless System Services  ######################
def loadModule():
    print('Load Module: RNWF Wireless System Services')

    sysWifiRNWFComponent = Module.CreateComponent('sysWifiRNWF', 'RNWF WIFI SERVICE','/Wireless/System Services/', 'system/RNWF_wifi/config/sys_wifi.py')
    
    sysWifiProvRNWFComponent = Module.CreateComponent('sysWifiProvRNWF', 'RNWF WIFI PROVISIONING SERVICE', '/Wireless/System Services/', 'system/RNWF_wifiprov/config/sys_wifiprov.py')
    
    sysMQTTRNWFComponent = Module.CreateComponent('sysMqttRNWF', 'RNWF MQTT Service', '/Wireless/System Services/', 'system/RNWF_mqtt/config/sys_mqtt.py')
    
    sysOTARNWFComponent = Module.CreateComponent('sysOtaRNWF', 'RNWF OTA SERVICE', '/Wireless/System Services/', 'system/RNWF_ota/config/sys_ota.py')

    ########################## Harmony Network Net Service #################################    
    sysNetRNWFComponent = Module.CreateComponent("sysNetRNWF", "RNWF Net Service", "/Wireless/System Services/","system/RNWF_net/config/sys_net.py")

    sysEccRNWFComponent = Module.CreateComponent("sysEccRNWF", "RNWF ECC608 Service", "/Wireless/System Services/","system/RNWF_ecc/config/sys_ecc.py")

    sysAppRNWFComponent = Module.CreateComponent("sysAppRNWF", "RNWF Variant", "/Wireless/","system/RNWF_app/config/sys_app.py")


   