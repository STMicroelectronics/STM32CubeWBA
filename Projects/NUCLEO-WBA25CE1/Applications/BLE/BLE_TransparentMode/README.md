## __BLE_TransparentMode Application Description__

How to communicate with the STM32CubeWise - Bluetooth LE Explorer tool using the transparent mode (UART configuration).

### __Keywords__

Connectivity, BLE, BLE protocol.

### __Hardware and Software environment__

  - This application runs on STM32WBA25xx Nucleo board.

  - Connect the Nucleo Board to your PC with a USB cable to USB type C to ST-LINK connector (USB_STLINK).

### __How to use it?__

In order to make the program work:

 - Open the project with your preferred toolchain.
 - Rebuild all files and load your image into target memory.
 - Run the example.

To test the BLE Transparent Mode application, use the STM32CubeWise - Bluetooth LE Explorer tool:

 - Make the connection between STM32CubeWise - Bluetooth LE Explorer tool and BLE_TransparentMode application.
 - Send some ACI commands from STM32CubeWise - Bluetooth LE Explorer tool as HCI_RESET, HCI_LE_RECEIVER_TEST, HCI_LE_TRANSMITTER_TEST, ...
 - the application must acknowledge the command with a "HCI_COMMAND_COMPLETE_EVENT" answer.

Button 1 allows you to enter/exit Low Power Stop mode.

Button 2 allows you to enter in Standby Mode. Exiting standby mode must be done by pressing Reset Button.

### __Notes__

In this project, the BLE host stack is initialized with the Extended Advertising option.<br />
Refer to the <a href="https://www.st.com/resource/en/programming_manual/pm0271-guidelines-for-bluetooth-low-energy-stack-programming-on-stm32wb-stm32wba-mcus-stmicroelectronics.pdf"> Full mode commands (legacy and extended).</a>

Supported command in each configuration refer to chapter : 5.9.6 Legacy and extended advertising/scanning commands and event impact.<br />
Please note, HCI_Reset command must be called between Legacy adv/scan and Extended adv/scan HCI commands.<br />
It is possible to modify CFG_BLE_OPTIONS in the app_conf.h to disable BLE_OPTIONS_EXTENDED_ADV feature from ACI.
