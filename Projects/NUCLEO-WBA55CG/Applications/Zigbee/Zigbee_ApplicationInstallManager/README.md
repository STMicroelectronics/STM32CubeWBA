## __Zigbee_ApplicationInstallManager Application Description__

The Zigbee_ApplicationInstallManager application, associated to a Zigbee application embedding OTA service, manages the firmware update over the air of a Zigbee application.  

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, Zigbee Firmware Update Over The Air.  

### __Hardware and Software environment__

This application runs on STM32WBA55xx devices.  
    
### __How to use it?__

In order to make the program work, you must do the following:  

 - Open your preferred toolchain.  
 - Rebuild all files and load your image into target memory.  

The Zigbee_ApplicationInstallManager application must run with Zigbee applications embedding OTA service named Zigbee_xxx_OTA.  
Zigbee_Coord_OTA application is used to download new application.

__Zigbee_ApplicationInstallManager__ is loaded at the memory address __0x08000000__ and __Zigbee_xxx_OTA__ application is loaded at the memory address __0x08006000__.  

For more details on OTA process please refers to associated readme.html.  
