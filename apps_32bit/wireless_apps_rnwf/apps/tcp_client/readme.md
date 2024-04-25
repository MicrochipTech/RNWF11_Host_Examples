#  TCP Client Application
This application demonstrates a traditional open TCP client in the Host Companion mode. In this application the RNWF02 will host a TCP Client in Wi-Fi STA mode. The user is required to configure the Wi-Fi credentials for Home-AP and provide the necessary details for the remote TCP server configuration. By default the application will connect to Home-AP and begin a TCP client socket connection with a TCP server. After establishing a successful TCP server-client connection, the data exchange will take place.

## Building the Application

To build this application, open the project file \(`apps/tcp_client/sam_e54_xpro_rnwf_uart.X/`\) in MPLAB X IDE. For more details on opening the project file in MPLAB X IDE, refer to the [Opening the Project file](../Wi-Fi_easy_config/docs/GUID-671CCA8C-64AE-4EA1-B144-D46A6FEE76FF.md). The following table provides details on the project file.

|Project Name|Description|
|:-----------|:----------|
|`sam_e54_xpro_rnwf_uart.X`| -   MPLAB X project for SAM E54 Xplained Pro evaluation kit and RNWF02 Add On Board
|
-   This application demonstrates a traditional open TCP client in Host Companion mode. After establishing a successful TCP server-client connection, the data exchange will take place


## Running the Application

1.  Mount the RNWF02 Add On Board on SAM E54 Xplained Pro evaluation kit at respective header. For more details about the boards placement in the SAM E54 X-plained host board, see [Figure 6-1](../Wi-Fi_easy_config/docs/GUID-7BA99DE1-89EB-4DD7-973B-974B175D657A.md#FIG_B4M_3WX_PZB)
2.  Connect the debugger USB port on the SAM E54 Xplained Pro evaluation kit to computer using a micro-USB cable.
3.  Open the project and launch MCC Harmony3.
4.  Configure Home-AP credentials for STA mode, using the RNWF02 Wi-Fi configuration options. For more details about the Wi-Fi settings configuration, See [Figure 3-52](../Wi-Fi_easy_config/docs/GUID-CE9CEDFD-5FD4-4BC4-AB96-17647C430816.md#GUID-98F61951-56D2-4B91-B509-2A796802408B).
5.  Configure TCP Server details such as TCP server IP address and port. For more details about the Net Sock settings configuration, See [Figure 1](docs\GUID-834E84DC-609A-4A37-853F-3552166E1009.md#FIG_M55_PXS_SZB).
6.  Generate the code as illustrated below.

    Figure 6-11. Generating the Code

    ![](docs\images\GUID-EDD3733E-E395-4AB6-BD2F-046D2C8D165A-low.png)

7.  Build and program the code to the hardware using MPLABX IDE

    Figure 6-12. Programming the Board

    ![](docs\images\GUID-7B288BCE-2B86-4B4E-A43A-7E862137384C-low.png)

8.  Open the Terminal application \(for example, Tera Term or PuTTY\) on the PC

9.  Connect to the “EDBG Virtual COM Port" and configure the serial settings as follows:

    1.  Baud: 115200

    2.  Data: 8 Bits

    3.  Parity: None

    4.  Stop: 1 Bit

    5.  Flow Control: None

10. As the board boots up, it will connect to Home-AP and print the IP address obtained. The board will establish a connection with configured TCP Server and print a success message. Once the TCP client-server connection is successful, the application will continue listening on the socket for incoming messages and then write them back to the server.

    <br />

    ![](docs\images\GUID-6DE0F6F6-65B7-4205-912C-E1E3DFEEB337-low.png "TCP Client - Serial Logs")

    <br />

    Figure 6-14. Mobile App TCP Server Logs

    ![](docs\images\GUID-946340A6-82AB-41FD-B5EA-5E0FB6DA0DA8-low.png)


