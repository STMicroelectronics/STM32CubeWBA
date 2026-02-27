## __BLE_SerialCom_Peripheral Application Description__  

How to demonstrate Point-to-Point communication using BLE component.  

The Nucleo board flashed with the BLE_SerialCom_Peripheral application acts as a GAP peripheral.  

For more information, please refer to <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Serial_Com#Serial_communication_profile"> STM32WBA Bluetooth LE - Serial communication wiki.</a>  

### __Keywords__
  
Connectivity, BLE, BLE protocol, BLE pairing.  

### __Hardware and Software environment__
  
  - This application runs on STM32WBA25xx devices.  
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  

### __How to use it?__
  
In order to make the program work:  

 - Open the project with your preferred toolchain.  
 - Rebuild all files and load your image into target memory.  
 - Run the example.  

For this application, two STM32WBA nucleo boards are used, or one STM32WBA and a smartphone.  
One board is configured as central using BLE_SerialCom_Central and the other as peripheral using BLE_SerialCom_Peripheral.  
The central board supports data transfer by sending L2CAP data through a COC link.  

No service is added.  

After BLE link is established, a Connection Oriented Channel (COC) is established (full stack is necessary for COC).  
Data are exchanged through the COC link.  

Open a VT100 terminal on both the Central and Peripheral sides (ST Link Com Port, @115200 bauds).  
This application has to be used in conjunction with BLE_SerialCom_Central.  

At reset application initialization is done.  

- The peripheral device starts advertising.  
- Push B1 on central device: the central device scans and automatically connects to the peripheral (use of CFG_DEV_ID_BLE_COC).  
- After BLE connection:  
  - The client establishes a COC channel.  
  - When the link is ready it displays "CONNECTION READY"  
  - Configure the terminal with settings for local echo.  
  - When Append LF is configured, enter data using keyboard and check that the data are correctly received on the peer device.  

There are two different modes to send messages from an STM32WBA: "Send button" or "Real time".  
After reset, "Real time" mode is selected.  
In the terminal, when you type the message:  

- In "Send button" mode, data are sent when LF is sent.  
- In "Real time" mode, each character is immediately sent.  

On the server side when connected:  

- B1 changes "send data mode".  
- B2 sends slave security request.  

On the server side when idle:  

- B1 changes "send data mode".  
- B2 clears database.  

On the client side when connected:  

- B1 changes "send data mode".  
- B2 sends pairing request.  
- B3 changes connection interval update.  

On the client side when idle:  

- B2 clears database.  

In case the central is a smartphone, open ST BLE Toolbox and click on COC.  
Pairing is requested; click on "pair".  

Warning: with CubeIDE Debug variant, some transmitted characters may be missed.

Please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Serial_Com#How_to_use_the_Bluetooth.C2.AE_Low_Energy_SerialCom_central_application"> STM32WBA Bluetooth LE Serial Communication.</a>  

