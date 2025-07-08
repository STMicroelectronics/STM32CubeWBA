## __BLE_TransparentMode Application Description__

How to communicate with the STM32CubeMonitor-RF Tool using the transparent mode.  

### __Keywords__

Connectivity, BLE, BLE protocol.  

### __Hardware and Software environment__

  - This application runs on STM32WBA55xx devices.  
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  

### __How to use it?__

In order to make the program work:  

 - Open your preferred toolchain.  
 - Rebuild project and flash the board at the address 0x08000000 with the binary file.  

To test the BLE Transparent Mode application, use the STM32CubeMonitor-RF tool:  

 - Make the connection between STM32CubeMonitor RF tool and BLE_TransparentMode application.  
 - Send some ACI commands from STM32CubeMonitor RF tool as HCI_RESET, HCI_LE_RECEIVER_TEST, HCI_LE_TRANSMITTER_TEST, etc  
 - the application must acknowledge the command with a "HCI_COMMAND_COMPLETE_EVENT" answer.  
 
Button 1 allows you to enter/exit Low Power Stop mode.  

Button 2 allows you to enter in Standby Mode. Exiting standby mode must be done by pressing Reset Button.  

### __Note__

In this project, the BLE host stack is initialized with the Extended Advertising option.  
Refer to the <a href="https://www.st.com/resource/en/programming_manual/pm0271-guidelines-for-bluetooth-low-energy-stack-programming-on-stm32wb-stm32wba-mcus-stmicroelectronics.pdf"> Full mode commands (legacy and extended).</a>  

Supported command in each configuration refer to chapter : 5.9.6 Legacy and extended advertising/scanning commands and event impact.  

Please note, HCI_Reset command must be called between Legacy adv/scan and Extended adv/scan HCI commands.  

It is possible to modify CFG_BLE_OPTIONS in the app_conf.h to disable BLE_OPTIONS_EXTENDED_ADV feature from ACI.  
