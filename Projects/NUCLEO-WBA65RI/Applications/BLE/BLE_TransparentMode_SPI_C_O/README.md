## __BLE_TransparentMode_SPI_C_O Application Description__

How to communicate with the STM32CubeWise - Bluetooth LE Explorer tool using the transparent mode.

### __Keywords__

Connectivity, BLE, BLE protocol.

### __Hardware and Software environment__

  - This application runs on STM32WBA65xx Nucleo board.

  - Connect the Nucleo Board to your PC with a USB cable to USB type C to ST-LINK connector (USB_STLINK).

### __How to use it?__

In order to make the program work:

 - Open the project with your preferred toolchain.
 - Rebuild all files and load your image into target memory.
 - Run the example.

To test the BLE Transparent Mode SPI Controller Only application, use the STM32CubeWise - Bluetooth LE Explorer tool:

 - Make the connection between STM32CubeWise - Bluetooth LE Explorer and the external microcontroller connected to a STM32WBAxx configured with BLE_TransparentMode_SPI_C_O application
 - Send some ACI commands from STM32CubeWise - Bluetooth LE Explorer as HCI_RESET, HCI_LE_RECEIVER_TEST, HCI_LE_TRANSMITTER_TEST, ...
 - the application must acknowledge the command with a "Command Complete" answer

### __Notes__

Refer to STM32CubeWise - Bluetooth LE Explorer tool documentation for more information about how to interact with the tool.

