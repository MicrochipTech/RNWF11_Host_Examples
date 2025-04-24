# RNWF11
The Microchip's RNWF11 Wi-Fi™ Modules are an attach solution which can enable Wi-Fi and IoT Cloud
connectivity on any existing designs. The RNWF11 modules are designed to reduce the user application
development time by providing ASCII based AT Commands over the UART interface to expose its features and
capabilities. As the RNWF11 handles all the Wi-Fi and IoT cloud related functionality, even a simple 8-bit Host
microcontroller can have the cloud connectivity with minimal hardware requirements. The ASCII AT command
interface provides an easier evaluation of the board using any standard serial port application on a PC.
The RNWF11 Add On Board is an efficient, low-cost development platform to evaluate and demonstrate the
features and functionality of Microchip’s low-power Wi-Fi RNWF11PC Module. It can be used with a Host PC
via USB interface without the need of an additional hardware accessory. This is compliant to the mikroBUS™
Standard. The Add On Board can be plugged easily on the host board and can be controlled by Host
Microcontroller Unit (MCU) with AT commands through UART.
The RNWF11 Add On Board offers:
1. Easy-to-use platform to speed-up design concepts to revenue with the low-power Wi-Fi RNWF11PC Module:
- Host PC via USB Type-C™ interface.
- Host board supporting mikroBUS socket.
2. ATECC608B Crypto Authentication Trust Flex IC device for secure and authenticated cloud connection.
3. Pre-programmed, easy- to-use, low-cost Wi-Fi adapter.

## Features

1. Easy to learn ASCII AT commands for adding Wi-Fi and Cloud connectivity to any processor
2. Mutual authentication supported with AWS and Azure
3. Server authentication supported for test.mosquitto.org
4. Designed for RNWF11 module with integrated Trust&Go (TNG) or Trustflex (Trustflex is mounted on the RNWF11 Add On Board, which makes it very easy for development)
5. Ready to use RNWF11 module to connect to different cloud vendors by just changing the configuration values (single image supports connectivity with AWS / Azure / test.mosquitto.org)
6. Supports MQTT, Socket programming(TCP/ UDP), SNTP, TLS (v1.2), ICMP, IGMP, DNS, DHCP, WPA3, Loading Certificates via UART, WiFi Provisioning.

## Documents

- [RNWF11 Application Developer Guide](https://onlinedocs.microchip.com/oxy/GUID-209426F5-2F78-4B3F-80A0-AD79A119381E-en-US-2/index.html) - RNWF11 Application Developer Guide.

## Hardware Setup
### Hardware Requirements

- RNWF11 Add On board
- Access Point with Internet
- In case the RNWF11 Add On board needs to be evaluated using a Host MCU, refer to the RNWF11 Application Developer Guide to get started

## Solution block diagram
### Solution Overview

The RNWF11 firmware can connect to any of the three Clouds- AWS, Azure or test.mosquitto.org with simple AT configuration commands.

## Evaluating the Solution

Perform the following steps:

1. Connect the RNWF11 Add On board to your PC/ Host MCU.
2. Open the Terminal application (Ex.:Tera term) on the computer.
3. Connect to the "USB to UART" COM port and configure the serial settings as follows:
- Baud : 230400
- Data : 8 Bits
- Parity : None
- Stop : 1 Bit
- Flow Control : None
- Receive and Transmit needs to be set to CR+LF (See below snapshot on how it is done with Teraterm)
- ![image](https://github.com/MicrochipTech/RNWF11_Beta/assets/47098770/3ed41f34-6350-4039-89c0-92dac958ec44)

4. All the configurations on the device will be done using AT Cmds via the UART. The details regarding the supported AT CMDs can be found in the AT command section of the RNWF11 Application Developer Guide.
5. Configure the SNTP details using the AT Cmds
Please refer to the AT command section of the RNWF11 Application Developer Guide for more details on each of the command below.
    Example:
    
        AT+SNTPC=3,"pool.ntp.org"
        AT+SNTPC=2,1
        AT+SNTPC=1,1
6. Configure the home AP credentials using the AT Cmds
Please refer to the AT command section of the RNWF11 Application Developer Guide for more details on each of the command below.
    Example:
    
        AT+WSTAC=1,"DEMO_AP"
        AT+WSTAC=2,3
        AT+WSTAC=3,"password"
        AT+WSTAC=4,0
        AT+WSTA=1
   
8. The device connects to the Wi-Fi and the IP address is assigned, and relevant AT Cmd response is sent to UART.
Please refer to the AT command section of the RNWF11 Application Developer Guide for more details on each of the output printed on UART below.
Example:

        +WSTALU:1,"12:EC:81:FF:DC:77",11
        +WSTAAIP:1,"192.168.227.179"
        +WSTAAIP:1,"FE80::DA47:8FFF:FE68:FAA3"
        +TIME:3909622842
        
9. Configure the TLS configuration to be used to connect to Cloud - in this case either of AWS or Azure or test.mosquitto.org
Example commands to be given while connecting to AWS. Please note that the value to be set in AT+TLSC=1,5 command would depend on the AWS Broker URL with which the device is going to connect. Please refer to the AT command section of the RNWF11 Application Developer Guide for more details on each of the command below.

        AT+TLSC=1,1,"AmazonRootCA1"
        AT+TLSC=1,5,"aznb5n71crum1-ats.iot.us-east-1.amazonaws.com"
        AT+TLSC=1,8,1

10. Configure the device to connect to Cloud - in this case either of AWS or Azure or test.mosquitto.org
Please refer to the AT command section of the RNWF11 Application Developer Guide for more details on each of the command below.
Example:

        AT+MQTTC=1,"aznb5n71crum1-ats.iot.us-east-1.amazonaws.com"
        AT+MQTTC=2,8883
        AT+MQTTC=3,"yourDeviceId"
        AT+MQTTC=7,1
        AT+MQTTC=6,60
        AT+MQTTCONN=1
        
Note: One needs to register the device certificate with AWS and Azure portal in case the user wants to connect to either of these cloud vendors. More details on how to setup the cloud for AWS or Azure are provided in the RNWF11 Application Developer Guide.

The device connects to the Cloud, and relevant AT Cmd response is sent to UART.

Below is a sample output which comes on UART when a device connects to AWS cloud. Please refer to the AT command section of the RNWF11 Application Developer Guide to get more details about each of the line below.

Example:

        +MQTTPROPRX:33
        +MQTTPROPRX:36
        +MQTTPROPRX:37
        +MQTTPROPRX:39
        +MQTTPROPRX:34
        +MQTTPROPRX:40
        +MQTTPROPRX:41
        +MQTTPROPRX:42
        +MQTTPROPRX:19
        +MQTTCONNACK:0,0
        +MQTTCONN:1
        
## Secure Provisioning & Transport Layer Security

The RNWF11 Add On Boards kits are shipped with the RNWF11PC module variant. Since RNWF11PC devices are pre-provisioned, the customer does not need to programe the device certificate for each of his devices and the firmware can utilise the on-chip certificate to securely authenticate with AWS IoT Core/ Azure IoT Hub.
The TrustFLEX hardware secure element on the Add On Board provides users a secure option to connect to cloud servers that require proprietary authentication.
