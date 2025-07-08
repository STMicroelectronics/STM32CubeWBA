## __BLE_HeartRate_ota Application Description__

How to use the BLE Heart Rate with Over The Air Firmware Update feature.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE Firmware Update Over The Air, BLE profile.  

### __Hardware and Software environment__

  - This application runs on STM32WBA55xx Nucleo board.
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).

### __How to use it?__

In order to make the program work:  

 - Open your preferred toolchain.  
 - Rebuild all files and flash the board:  
   - BLE_ApplicationInstallManager binary is downloaded at the memory address 0x08000000  
   - BLE_HeartRate_ota binary is downloaded at the memory address 0x08006000  
 - Run the example.  

Use one of the following remote applications to interact with your device: <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a>  

- Power on the STM32WBAxx Nucleo board.  
- Open remote interface:  
  - Select the HR__xx in the device list, where xx are the last 2 digits of the BD ADDRESS.  
  - Connect to it.  
  - Select the Heart Rate service.  
  
Heart Rate value and Energy Expanded measurements are displayed in graphs.  
Energy Expanded can be reset.  

Each button (B1, B2, B3) on STM32WBAxx Nucleo board, is associated with the following functions:  

- B1 :  
  - Not connected : Launch fast advertising.  
  - Connected     : Switch between 1Mbit and 2Mbit PHY.  
- B2 :  
  - Not connected : Clear security database.  
  - Connected     : Security request, pairing.  
- B3 :  
  - Not connected : Not used.  
  - Connected     : update connection interval.  

For more information, please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_HeartRate#Heart_Rate_Profile"> Bluetooth LE - Heart Rate Sensor ST wiki.</a>  

The Firmware Update Over The Air (FUOTA) service provides the possibility to receive a new application to be installed while the current application is running.  
For more information please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_FUOTA#Firmware_update_over_the_air_profile"> Firmware Update Over The Air profile.</a>  

To change the application downloading Firmware Update Other The Air:  

- Select the Over The Air Update Server service panel in remote interface.  
- Select the STM32WBA device type.  
- The STM32WBA interface offers the choice to download an Application Binary or a User Conf Binary.  
  - Memory mapping is defined as following:  
    - pages 0 to 2 are reserved for the BLE_ApplicationInstallManager application.  
    - pages 3 to 63 are dedicated to BLE_HeartRate_ota application.  
    - pages 64 to 124 are dedicated for the new application.  
    - page 125 is dedicated to User Configuration Data.  
    - pages 126 to 127 reserved for NVM.  

  - Offset configuration:  
    - If Application Binary is selected:  
      Address offset is set to 0x100000 to download the new application at address 0x08100000.  
    - If User Conf Binary is selected:  
      Address offset is set to 0x0F6000 to download the new application at address 0x080F6000.  
  - Select the binary file to download:  
    - Number of pages to erase is displayed resulting of the size in pages of the file.  
    - The Number of pages may be over written is needed.  
  - Download the file.  
  - When done, a reboot of the application is done.  
