## __BLE_Sensor Application Description__

How to use the BLE sensor application.  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE profile.  

### __Hardware and Software environment__

- This example runs on STM32WBA6M device.  
- Connect the B-WBA6M-WPAN board to your PC with a USB cable type C.  
- To flash and debug, connect an STLink v3 to the CN3 connector.  
For more details <a href="https://wiki.st.com/stm32mcu/wiki/STM32StepByStep:STM32WBA6M_Discovery_Kit_Out_Of_Box_Demonstration#How_can_I_program_my_WBA-board--"> Program the B-WBA6M-WPAN Board.</a>  

### __How to use it?__

In order to make the program work:  

 - Open your preferred toolchain.  
 - Rebuild all files and flash the board at the address 0x08000000 with the binary file.  

Use one of the following remote interfaces to interact with your device <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a>  

Reset the B-WBA6M-WPAN Nucleo board and launch remote interface on PC/android/ios device.  

 - Click on WBA6M_xx to connect the board.  
 - Select Environmental Data to display temperature and humidity.  
 - Display Plot Data and play the accelerometer data (moving the board will modify the displayed values).  

For more informations, please refer to <a href="https://wiki.st.com/stm32mcu/wiki/STM32StepByStep:STM32WBA6M_Discovery_Kit_Out_Of_Box_Demonstration"> B-WBA6M-WPAN ST BLE Sensor application.</a>  
