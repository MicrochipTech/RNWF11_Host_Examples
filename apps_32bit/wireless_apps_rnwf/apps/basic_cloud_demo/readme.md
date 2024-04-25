# Azure Cloud Demo

The Azure Cloud application demonstrates the Azure Plug and Play connection with the Azure IoT Central. The RNWF02 board acts as a MQTT client using the inbuilt MQTT AT Command sets. The demo uses the x.509 certificate based group enrollment to ease the connection of multiple devices to the Azure cloud. The RNWF02 module ships with a built-in X.509 root certificates to authenticate Azure MQTT server which simplifies the TLS handshake protocol with Microsoft Azure. The user can upload the device certificates into the RNWF02 for the TLS client authentication. Using this application, most users can connect their Microchip RNWF02 to Azure Cloud/Azure IoT Hub in a few minutes.

## Building the Application

To build this application, open the project file (`apps\basic_cloud_demo\firmware\sam_e54_xpro_rnwf_uart.X`\) in MPLAB X<br /> IDE. For more details on opening the project file in MPLAB X IDE, refer to the [Opening the Project file](../Wi-Fi_easy_config/docs/GUID-671CCA8C-64AE-4EA1-B144-D46A6FEE76FF.md). The following table provides details on the project file.

|Project Name|Description|
|:-----------|:----------|
|`sam_e54_xpro_rnwf_uart.X`|  MPLAB X project for SAM E54 Xplained Pro evaluation kit and RNWF02 Add On Board
|
-   This application demonstrates the Azure Plug and Play connection with the Azure IoT Central. Using this application, most users can connect their Microchip RNWF02 to Azure Cloud/Azure IoT Hub in a few minutes.

## Running the Application

1.  **Create a Root of Trust Chain for RNWF Devices:**

    The demo application includes a set of tools in `../apps/azure_cloud/tools` folder. It enables easy creation of certificate infrastructure and individual device certificates with very minimal user inputs.

    **Note:**

    -   On Windows Operating Systems, make sure the [GitBash](https://git-scm.com/download/win) is installed to run these tools.
    -   These scripts are based on the Azure's [Create and Upload Certificates for Testing](https://learn.microsoft.com/en-us/azure/iot-hub/tutorial-x509-test-certs?tabs=windows) tutorial.
    <br />

    1.  Right click and open the Git Bash in the tools folder

        ![](docs\images\GUID-3C12EE95-B5C5-456A-929F-FBA3B9DA72D6-low.png "Git Bash")

    2.  Run the *create\_initial\_setup.sh* as illustrated in the following figure. It will request the user to provide the domain suffix and common name for Root CA.

        ![](docs\images\GUID-9B8CA362-7D12-4940-84D0-FCA5EA237E3D-low.png "Running create_initial_setup.sh")

    3.  Up on successful execution of above command, following three new folders will be created

        ![](docs\images\GUID-8F1085A8-21D6-4D94-B1CA-B209D970317C-low.png "Setup Folders")

2.  **Device Certificate**

    1.  Run the `create_device_certificate.sh` to create individual device certificate. It requests the user to provide Subordinate/Intermediate CA folder name and a unique device id \(Common Name\).

        ![](docs\images\GUID-8578BBD5-36C9-45F3-8715-F4FC7B4561C5-low.png "Unique Device ID")

        **Note:**

        -   Note down the information of device ID or common name for later use in Azure configuration.
        -   In order to modify the default subject identifier information, open the `create_device_certificate.sh` and update the following macros.
        ![](docs\images\GUID-6E3F015B-3B03-4BF3-AD38-29C40A1D0A06-low.png "create_device_certificate.sh")

    2.  On successful execution of device certificate, a new folder is created inside the `../tools/devcerts` folder as illustrated in following figure

        **Note:** Note down the information of certificate and key file names \(by default device ID or common name\) for use later in the Azure TLS configuration

    3.  Use the PC companion mode of the setup and program the .pem and \* .key\* files \(highlighted/marked below\) using file\_upload tools.

        ![](docs\images\GUID-EFF2B98F-C6AB-497C-B1A3-FC61C07CB354-low.png "Certificate and Key File")

    <br />

3.  **Creating a Microsoft Azure Account and Azure IoT Central Application**

    <br />

    1.  For new users, create a new [Azure Account](https://azure.microsoft.com/en-us/free)
    2.  Log into the [Azure Portal](https://azure.microsoft.com/en-us/free) using the new/existing account
    3.  Create an IoT Central application by clicking on *Create a resource\>Internet of Things\>IoT Central application*

        ![](docs\images\GUID-29D78F64-0E9F-49D7-82BB-6ED81B978E3C-low.png "Create A Resource")

        ![](docs\images\GUID-E8CC79B2-5C5F-435E-B636-E34A94AC6DC5-low.png "IOT Central Application")

    4.  Under Subscription create a new group \(say RNWF\) to manage it's resources and cost
    5.  Provide the resource name and Application URL to view/manage the IoT Central devices

        ![](docs\images\GUID-1D53EDE2-ED33-4F81-B685-DF4F804856AC-low.png "IOT Central Application")

    6.  Open the [Azure Central Applicaiton](https://apps.azureiotcentral.com/build) and build a new App

        ![](docs\images\GUID-381BA63B-C02C-4766-B0AF-57B4FF65C4F3-low.png "Building IOT Application")

        ![](images\GUID-8945FE16-4389-47DF-B37C-7F083FC0C3EE-low.png "New Application Details")

    7.  Select the application and navigate to *Permissions\>Device connection groups*

        ![](docs\images\GUID-E9DD03FC-9876-46BB-9C8C-56ED525B83E5-low.png "Application Portal Enrollment")

    8.  Note down the information of ID scope and click **+ New** as illustrated above to create an enrollment group
    9.  Provide a name for the enrollment group and select Attestation Type as **Certificates \(X.509\)**

        ![](docs\images\GUID-5F9A0392-C7ED-4FC6-9F75-4B6768B396EF-low.png "Creating New Enrollment Group")

    10. Save the enrollment group and click on *Primary\>Manage primary* to manage the Primary certificate and select the **subordinate CA certificate** generated in the step 1 \(Create a Root of Trust Chain for RNWF Devices\) `subca.crt` certificate inside the `subca` folder.

        ![](docs\images\GUID-DD6DF2C5-93FB-468B-82CE-588E7A2E6916-low.png)

    11. Mount the RNWF Add On Board on SAM E54 Xplained Pro evaluation kit at respective headers. For more details about the boards placement in the Curiosity Nano base, see [Figure 6-1](../Wi-Fi_easy_config/docs/GUID-7BA99DE1-89EB-4DD7-973B-974B175D657A.md#FIG_B4M_3WX_PZB).
    12. Connect the debugger USB port on the SAM E54 Xplained Pro evaluation kit to computer using a micro USB cable
    13. Open the project and launch MCC with Harmony3.
    14. Configure Home-AP credentials for STA mode, using the Wi-Fi settings configuration. For more details about the Wi-Fi settings configuration, See [Figure 3-52](../Wi-Fi_easy_config/docs/GUID-CE9CEDFD-5FD4-4BC4-AB96-17647C430816.md#GUID-98F61951-56D2-4B91-B509-2A796802408B).
    15. Update the Azure Cloud configuration with following details. For more details about the Azure Cloud configuration, See [Figure 3-58](docs/GUID-DD648E0B-2B4D-45AA-9A19-A8A1849D5FC9.md#FIG_OH5_Z1T_SZB)
        1. ID Scope noted in step 8 of Creating a Microsoft Azure Account and Azure IoT Central Application, update this scope ID in Azure Cloud configuration as illustrated below

            ![](docs\images\GUID-3EEC13CA-6FA8-4525-8ECE-660DF11ECEFB-low.png "Scope ID")

        2.  Unique Device ID \(Common Name\) noted in step 1 of Device Certificate, update this Unique Device ID in Azure Cloud configuration as illustrated below

            ![](docs\images\GUID-4032DD95-719A-4AC4-B821-C6658A929C27-low.png "User Name")

            ![](docs\images\GUID-080AA52F-A140-465B-A1FB-0BE3E91DB464-low.png "Publish Topic Name")

        3.  Device Certificate and Key file names noted in step 2 of Device Certificate, update this in Azure Cloud configuration as illustrated below

            ![](docs\images\GUID-1BDD7F4C-9B19-4443-9766-0992218D0CE7-low.png "Device Certificate and Key")

    16. Generate the code as illustrated below

        ![](docs\images\GUID-EDD3733E-E395-4AB6-BD2F-046D2C8D165A-low.png)

    17. Build and program the code to the hardware using MPLABX IDE

        ![](docs\images\GUID-7B288BCE-2B86-4B4E-A43A-7E862137384C-low.png)

    18. Open the Terminal application \(for example, Tera Term or PuTTY\) on the PC

        Connect<br /> to the "EDBG Virtual COM Port" and configure the serial settings as follows:

        1.  **Baud:** 115200
        2.  **Data:** 8 Bits
        3.  **Parity:** None
        4.  **Stop:** 1 Bit
        5.  **Flow Control:** None
    19. Press the Reset button on the host board, see [Figure 6-1](../Wi-Fi_easy_config/docs/GUID-7BA99DE1-89EB-4DD7-973B-974B175D657A.md#FIG_B4M_3WX_PZB).
    20. As the board boots up, application will list available certificates and keys on RNWF02 board. The board will connect to Home-AP configured. After this, the demo will attempt to connect to Azure IoT Hub \(using link configured\) and starts sending the telemetry

        ![](docs\images\GUID-A0208156-947E-42D6-AE1A-466E5011F8B2-low.png)

        ![](docs\images\GUID-59E6EFC9-8348-4F90-8081-ED3F29C06C5E-low.png)

    21. After the successful connection to Azure IoT Central, the device gets listed in the *[Azure Central Applicaiton](https://apps.azureiotcentral.com/)\> Devices view*.

        ![](docs\images\GUID-DE6123C1-87F0-4C8A-8993-18F047094C18-low.png "Device Connection to Azure IoT Central")

    22. Click on the device name to view the device About, Overview, Commands, Raw data tabs as follows

        ![](docs\images\GUID-BBAD695B-E47D-4ECA-A961-7386D6519D2E-low.png "Device Details")

    23. The SW0 press from the device will be notified to the Azure Cloud in the Raw data view

        ![](docs\images\GUID-97470190-C26D-4B1B-AF1A-75DEBDF3114F-low.png "Raw Data View")

    <br />
