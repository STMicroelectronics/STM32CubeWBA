## __BLE_TransparentMode_SPI Application Description__

How to communicate with the STM32CubeWise - Bluetooth LE Explorer tool using the transparent mode (SPI configuration).

### __Keywords__

Connectivity, BLE, BLE protocol, SPI.

### __Hardware and Software environment__

  - This application runs on STM32WBA55xx Nucleo board.

  - Connect the Nucleo Board to your PC with a USB cable to USB type C to ST-LINK connector (USB_STLINK).
  - This example runs on STM32WBAxx devices connected to an external microcontroller (Bluetooth LE network coprocessor framework, SPI mode).

### __How to use it?__

In order to make the program work:

 - Open the project with your preferred toolchain.
 - Rebuild all files and load your image into target memory.
 - Run the example.

To test the BLE Transparent Mode SPI application, use the STM32CubeWise - Bluetooth LE Explorer tool:

 - Make the connection between STM32CubeWise - Bluetooth LE Explorer and the external microcontroller connected to a STM32WBAxx configured with BLE_TransparentMode_SPI application
 - Send some ACI commands from STM32CubeWise - Bluetooth LE Explorer as HCI_RESET, HCI_LE_RECEIVER_TEST, HCI_LE_TRANSMITTER_TEST, ...
 - the application must acknowledge the command with a "HCI_COMMAND_COMPLETE_EVENT" answer.

### __Notes__

Refer to STM32CubeWise - Bluetooth LE Explorer tool documentation for more information about how to interact with the tool.<br />
The following setting must be applied on BLE_TransparenMode_SPI app_conf.h:<br />

#define CFG_BLE_OPTIONS             (0 | \<br />
                                     0 | \<br />
                                     0 | \<br />
                                     BLE_OPTIONS_EXTENDED_ADV | \<br />
                                     0 | \<br />
                                     BLE_OPTIONS_GATT_CACHING | \<br />
                                     0 | \<br />
                                     0 | \<br />
                                     BLE_OPTIONS_ENHANCED_ATT)

#define CFG_BLE_NUM_LINK                         (8)

#define CFG_BLE_AOA_AOD_ENABLE                   (0)

