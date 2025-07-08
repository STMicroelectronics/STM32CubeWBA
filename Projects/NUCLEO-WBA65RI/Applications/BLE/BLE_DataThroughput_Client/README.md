## __BLE_DataThroughput_Client Application Description__

How to demonstrate Point-to-Point communication using BLE component (as GATT server or GATT client).  

The Nucleo board flashed with BLE_DataThroughput_Client application acts as GATT Client and can connect and exchange data with a nucleo board flashed with BLE_DataThroughput_Server application.  

Data transfer via notification from server to client or/and write from client to server. Use of a timer that enables to calculate the throughput at application level.  

For more information, please refer to <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Data_Throughput#Data_throughput_profile"> STM32WBA Bluetooth LE - Data throughput profile.</a>  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile.  

### __Hardware and Software environment__

  - This application runs on STM32WBA65xx devices.  
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  

### __How to use it?__

In order to make the program work, you must do the following:  

 - Open your preferred toolchain.  
 - Rebuild all files and flash the board with the executable file.  

__Use of two nucleo boards__ 

Two STM32WBA nucleo boards are used: one central/client (flashed with BLE_DataThroughput_Client) and one peripheral/server (flashed with BLE_DataThroughput_Server).  

The GATT server board supports a Data transfer service with a transmission characteristic that supports notifications.  

The GATT client board supports a Data transfer by writing without response to the GATT server board's reception characteristic.  

The board flashed with BLE_DataThroughput_Client is defined as GAP central.  

Open a VT100 terminal on both the Central and Peripheral sides (ST Link Com Port, @115200 bauds).  

Upon reset application initialization is done.  

 - The peripheral device starts advertising.  
 - Push B1 on central: the central device scans and automatically connects to the peripheral (use of CFG_DEV_ID_DT_SERVER).  
 - After connection:  
   - the client starts to search the data transfer service and characteristic.  
   - the client enables the notification of the transmission characteristic.  

On server side when connected:  

 - B1 starts (blue LED is ON)/stops (blue LED is OFF) notification.  
 - B2 sends an L2CAP connection update - toggles 26.25 ms/11.25 ms.  
 - B3 toggles PHY.  

On the server side when idle:  

 - B2 clears database.  

On the client side when connected:  

 - B1 starts (blue LED is ON)/stops (blue LED is OFF) write without response.  
 - B2 sends a pairing request.  

On the client side when idle:  

 - B2 clears database.  

On the client terminal, the current notification displays the number of bytes per second.  

On the server terminal, the current write without response displays the number of bytes per second.     

In app_conf.h :  

 - if #define CFG_TX_PHY    2 and #define CFG_RX_PHY    2, link is set to 2M at connection.  
 - if #define CFG_TX_PHY    1 and #define CFG_RX_PHY    1, link stays at 1M.  

