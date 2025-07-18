## __BLE_ApplicationInstallManager Application Description__

The BLE_ApplicationInstallManager application, associated to a BLE application embedding OTA service, manages the firmware update over the air of a BLE application.  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE Firmware Update Over The Air, BLE profile.  

### __Hardware and Software environment__

This application runs on STM32WBA55xx devices.  
    
### __How to use it?__

In order to make the program work, you must do the following:  

 - Open your preferred toolchain.  
 - Rebuild all files and load your image into target memory.  

The BLE_ApplicationInstallManager application must run with BLE applications embedding OTA service like: BLE_HeartRate_ota or BLE_p2pServer_ota applications.  

__BLE_ApplicationInstallManager__ is loaded at the memory address __0x08000000__ and __BLE_HeartRate_ota__ or __BLE_p2pServer_ota__ application is loaded at the memory address __0x08006000__.  

For more details on OTA process please refers to associated readme.html.  
