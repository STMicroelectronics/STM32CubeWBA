## __BLE_GenericHealth_ECG_ota Application Description__

How to use the Generic Health Sensor profile with Firmware Update Over The Air feature.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE Firmware Update Over The Air, BLE profile.

### __Hardware and Software environment__

  - This application runs on STM32WBA65xx Nucleo board.

  - Connect the Nucleo Board to your PC with a USB cable to USB type C to ST-LINK connector (USB_STLINK).

### __How to use it?__

In order to make the program work:

 - Open the project with your preferred toolchain.
 - Rebuild all files and load your image into target memory.
 - Run the example.

The BLE_ApplicationInstallManager application must run with BLE applications embedding OTA service like: BLE_GenericHealth_ECG_ota application.

__BLE_ApplicationInstallManager__ is loaded at the memory address __0x08000000__ and __BLE_GenericHealth_ECG_ota__ application is loaded at the memory address __0x08006000__.

For more details on OTA process please refers to associated readme.html.

Use one of the following remote applications to interact with your device <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a>

- Power on the STM32WBAxx Nucleo board.
- Open remote interface:
  - Select the ECG__xx in the device list, where xx are the last 2 digits of the BD ADDRESS.
  - Connect to it.
  - Select the Generic Health Sensor service.
  - In the Generic Health Sensor interface, you read and write all the Generic Health Sensor characteristics.

Each button (B1, B2, B3) on STM32WBAxx Nucleo board is associated with the following functions:

- B1:
  - Not connected : Launch advertising.
  - Connected     : Switch between 1Mbit and 2Mbit PHY
- B2:
  - Not connected : Clear security database.
  - Connected     : Security request, pairing.
- B3:
  - Not connected : Not used.
  - Connected     : Update connection interval.

The Firmware Update Over The Air (FUOTA) service provides the possibility to receive a new application to be installed while the current application is running.<br />
For more information please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_FUOTA#Firmware_update_over_the_air_profile"> Firmware Update Over The Air profile.</a>

To change the application using Firmware Update Other The Air:

- Select the Over The Air Update Server service panel in the remote interface.
- Select the STM32WBA device type.
- The STM32WBA interface offers the choice to download an Application Binary or a User Conf Binary.
  - Memory mapping is defined as follows:
    - Pages 0 to 2 are reserved for the BLE_ApplicationInstallManager application.
    - Pages 3 to 127 are dedicated to BLE_GenericHealth_ota application.
    - Pages 128 to 252 are dedicated for the new application.
    - Page 253 is dedicated to User Configuration Data.
    - Pages 254 to 255 reserved for NVM.

  - Offset configuration:
    - If Application Binary is selected:<br />
      Address offset is set to 0x100000 to download the new application at address 0x08100000.
    - If User Conf Binary is selected:<br />
      Address offset is set to 0x0F6000 to download the new application at address 0x080F6000.
  - Select the binary file to download.
    - Number of pages to erase is displayed, resulting from the size in pages of the file.
    - The Number of pages may be overwritten is needed.
  - Download the file.
  - When done, a reboot of the application is performed.

