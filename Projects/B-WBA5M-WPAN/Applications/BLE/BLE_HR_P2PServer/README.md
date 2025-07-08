## __BLE_HR_P2PServer Application Description__

How to use the BLE Heart Rate and BLE Peer to Peer Server application.  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE profile.  

### __Hardware and Software environment__

- This example runs on STM32WBA5M device.  
- Connect the B-WBA5M-WPAN board to your PC with a USB cable type C.  
- To flash and debug, connect an STLink v3 to the CN3 connector.  
For more details <a href="https://wiki.st.com/stm32mcu/wiki/STM32StepByStep:STM32WBA5M_Discovery_Kit_Out_Of_Box_Demonstration#How_can_I_program_my_WBA-board--"> Program the B-WBA5M-WPAN Board.</a>  

### __How to use it?__

In order to make the program work:  

 - Open your preferred toolchain.  
 - Rebuild all files and flash the board at the address 0x08000000 with the binary file.  

Use one of the following remote interfaces to interact with your device <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a>  

Reset the B-WBA5M-WPAN Nucleo board and launch the remote interface on PC/android/ios device.  

 - Click on WBA5M_xx to connect the board, where xx are the last 2 digits of the BD ADDRESS.  
 - Heart Rate service:  
   - Select the Heart Rate service.  
   - Sensor position, Heart Rate values and energy measurement are launched and displayed in graphs, the energy measurement can be reset.  

For more informations, please refer to <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_HeartRate#Heart_Rate_Profile"> Bluetooth LE - Heart Rate Sensor ST wiki.</a>  

 - P2P service:  
   - Select the P2P Server service.  
   - Button B2 on B-WBA5M-WPAN board sends a notification message to the connected remote interface.  
   - On the remote interface, click on the LED icon to switch LED1 (blue) of the B-WBA5M-WPAN board On/Off.  
   - After 60s of advertising, the application switch from fast advertising to low power advertising.  

For more information, please refer to <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer#STM32WBA_Peer_to_Peer_Server_application"> STM32WBA Bluetooth LE - Peer 2 Peer Applications ST wiki.</a>  

