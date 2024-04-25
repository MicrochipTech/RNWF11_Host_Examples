#  TCP Server Application

This RNWF02 application implements a TCP Server. In this application RNWF02 will host a TCP Server in Wi-Fi STA mode. The user is required to configure the Wi-Fi credentials for Home-AP and specify the port number at which the TCP server would be listening. By default application will connect to Home-AP and starts a TCP Server on the board. After establishing a successful TCP server-client connection, the data exchange will take place.

## Building the Application

To build this application, open the project file<br /> \(`apps/tcp_server/sam_e54_xpro_rnwf_uart.X/`\) in MPLAB X IDE. For<br /> more details on opening the project file in MPLAB X IDE, refer to the [Opening the Project file](../Wi-Fi_easy_config/docs/GUID-671CCA8C-64AE-4EA1-B144-D46A6FEE76FF.md). The following table provides details on the project file.

|Project Name|Description|
|:-----------|:----------|
|`sam_e54_xpro_rnwf_uart.X`|    MPLABX project for SAM E54 Xplained Pro evaluation kit and RNWF02 Add On Board
-   This application demonstrates a TCP server in Host Companion mode. After establishing a successful TCP server-client connection, the data exchange will take place.

<br />

## Running the Application

1.  Mount the RNWF02 Add On Board on SAM E54 Xplained Pro evaluation kit at respective header. For more details about the boards placement in the SAM E54 X-plained host board, see [Figure 6-1](../Wi-Fi_easy_config/docs/GUID-7BA99DE1-89EB-4DD7-973B-974B175D657A.md#FIG_B4M_3WX_PZB)
2.  Connect the debugger USB port on the SAM E54 Xplained Pro evaluation kit to computer using a micro-USB cable.
3.  Open the project and launch MCC Harmony3.
4.  Configure Home-AP credentials for STA mode, using the RNWF02 Wi-Fi configuration options. For more details about the Wi-Fi settings configuration, See [Figure  1](../Wi-Fi_easy_config/docs/GUID-CE9CEDFD-5FD4-4BC4-AB96-17647C430816.md#GUID-98F61951-56D2-4B91-B509-2A796802408B).
5.  Generate the code as illustrated below.

    **Figure 6-21**. Generating the Code

    ![](docs\images\GUID-EDD3733E-E395-4AB6-BD2F-046D2C8D165A-low.png)

6.  Build and program the code to the hardware using MPLABX IDE

    Figure 6-22. Programming the Board

    ![](docs\images\GUID-7B288BCE-2B86-4B4E-A43A-7E862137384C-low.png)

7.  Open the Terminal application \(for example, Tera Term or PuTTY\) on the PC

    Connect to the “EDBG Virtual COM Port" and configure the serial settings as follows:

    1.  Baud: 115200
    2.  Data: 8 Bits
    3.  Parity: None
    4.  Stop: 1 Bit
    5.  Flow Control: None
8.  As the board boots up, it will connect to Home-AP and print the IP address obtained. After establishing a successful TCP server-client connection, the application will continue listening on the socket number configured for incoming messages and then write them back to the client connected.

    <br />

    ![](docs\images\GUID-2FB90060-A0BD-41BC-82BD-F47BA8E1FA6F-low.png "TCP Server - Serial Logs")

    <br />

    Figure 6-24. Mobile AppTCP Client Logs

    ![](docs\images\GUID-BEEACC8D-ED78-473F-87F2-F05D1EC06293-low.png)

