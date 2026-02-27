## __BLE_SerialCom_Central Application Description__  

How to demonstrate Point-to-Point communication using BLE component.  

The Nucleo board flashed with the BLE_SerialCom_Central application acts as GAP central.  

For more information, please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Serial_Com#Serial_communication_profile"> STM32WBA Bluetooth LE - Serial communication wiki.</a>  

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

Two STM32WBA nucleo boards are used.  
One board is configured as central using BLE_SerialCom_Central and the other as peripheral using BLE_SerialCom_Peripheral.  
The central board supports the Data transfer by sending L2CAP data through a COC link.  

No service is added.  

Once BLE link is established, a Connection Oriented Channel (COC) is established (full stack is necessary for COC).  
Data are exchanged through the COC link.  

Open a VT100 terminal on Central and Peripheral side (ST Link Com Port, @115200 bauds).  
This application has to be used in conjunction with BLE_SerialCom_Peripheral.  

At reset application initialization is done.  

- The peripheral device starts advertising.  
- Push B1 on the central device: the central device scans and automatically connects to the peripheral (use of CFG_DEV_ID_BLE_COC).  
- After the BLE connection:  
  - the client establishes a COC channel.  
  - When link is ready it displays "CONNECTION READY".  
  - Configure the terminal with settings for local echo.  
  - When Append LF is configured, enter data using keyboard and check that the data are correctly received on peer device.  

There are two different modes to send messages from an STM32WBA: "Send button" or "Real time".  
After reset, "Real time" mode is selected.  
In terminal, when you type the message:  

- In "Send button" mode, data are sent when LF is sent.  
- In "Real time" mode, each character is immediately sent.  

On server side when connected:  

- B1 changes the "send data mode".  
- B2 sends a slave security request.  

On server side when idle:  

- B1 changes the "send data mode".  
- B2 clears the database.  

On client side when connected:  

- B1 changes the "send data mode".  
- B2 sends a pairing request.  
- B3 changes the connection interval update.  

On client side when idle:  

- B2 clears database.  

Warning: with CubeIDE Debug variant, some transmitted characters may be missed.