## __BLE_SerialPort_Client Application Description__

Demonstrate Point-to-Point communication using Bluetooth Low Energy GATT component. 

The Nucleo board flashed with BLE_SerialPort_Client application acts as GAP central.

BLE_SerialPort_Client application scans and connects to BLE_SerialPort_Server device.

Once connected, BLE_SerialPort_Client can write to the BLE_SerialPort_Server and receive notification from it.

### __Keywords__
  
Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Hardware and Software environment__
  
  - This example runs on STM32WBA55xx devices.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector.
    
### __How to use it?__
  
In order to make the program work:  

 - Open the project with your preferred toolchain.  
 - Rebuild all files and load your image into target memory.  
 - Run the example.  

This is a demo that shows how to implement a simple 2-way Bluetooth LE communication between two STM32WBA5xx nucleo boards emulating serial communication. 

One board must be configured as a Central with BLE_SerialPort_Client application and the other as a Peripheral with BLE_SerialCom_Peripheral.

Open a serial terminal on Central and Peripheral side (ST Link Com Port, @115200 bauds).

At reset, application initialization is done.

  - The peripheral device starts advertising.
  - On the BLE_SerialPort_Server, press the B2 button to clear the security DB.
  - On the BLE_SerialPort_Client, press the B2 button to clear the security DB.
  - Push B1 on central: the central device scans (searching for FW_ID_SERIAL_PORT_SERVER in advertising data) and automatically connects to the peripheral.
  - After Bluetooth connection is established, the client discovers all service and characteristics.
  - Enter data using keyboard (on either Central or Peripheral): when LF or CR is encountered or data buffer is full (20 bytes), data are sent to the peer device.

On server side when connected:

  - B2 sends security request

On server side when idle:

  - B2 clears bonding database

On client side when connected:

  - B2 sends pairing request
  - B3 changes connection interval update

On client side when idle:

  - B2 clears bonding database  

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
