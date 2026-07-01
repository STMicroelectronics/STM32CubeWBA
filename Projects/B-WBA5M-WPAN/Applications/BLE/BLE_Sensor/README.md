## __BLE_Sensor Application Description__

How to use the BLE sensor application.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE profile.

### __Hardware and Software environment__

  - This application runs on STM32WBA5M CEB board.

- Connect the B-WBA5M-WPAN board to your PC with a USB cable type C.
- To flash and debug, connect an STLink v3 to the CN3 connector.<br />
For more details <a href="https://wiki.st.com/stm32mcu/wiki/STM32StepByStep:STM32WBA5M_Discovery_Kit_Out_Of_Box_Demonstration#How_can_I_program_my_WBA-board--"> Program the B-WBA5M-WPAN Board.</a>

### __How to use it?__

In order to make the program work:

 - Open the project with your preferred toolchain.
 - Rebuild all files and load your image into target memory.
 - Run the example.

Use one of the following remote interfaces to interact with your device <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a>
Reset the B-WBA5M-WPAN Nucleo board and launch remote interface on PC/android/ios device.

 - Click on WBA5M_xx to connect the board.
 - Select Environmental Data to display temperature and humidity.
 - Display Plot Data and play the accelerometer data (moving the board will modify the displayed values).

For more informations, please refer to <a href="https://wiki.st.com/stm32mcu/wiki/STM32StepByStep:STM32WBA5M_Discovery_Kit_Out_Of_Box_Demonstration"> B-WBA5M-WPAN ST BLE Sensor application.</a>
