## __BLE_ApplicationInstallManager Application Description__

The BLE_ApplicationInstallManager application, associated to a BLE application embedding OTA service, manages the firmware update over the air of a BLE application. 

### __Keywords__

Connectivity, BLE, BLE protocol, BLE firmware update over the air, BLE profile

### __Directory contents__

  - BLE_ApplicationInstallManager/Core/Inc/main.h                                             Header for main.c file. This file contains the common defines of the application
  - BLE_ApplicationInstallManager/Core/Inc/stm32_assert.h                                     Assert_param macro definition
  - BLE_ApplicationInstallManager/Core/Inc/stm32wbaxx_hal_conf.h                              HAL configuration file
  - BLE_ApplicationInstallManager/Core/Inc/stm32wbaxx_it.h                                    This file contains the headers of the interrupt handlers
  - BLE_ApplicationInstallManager/Core/Src/main.c                                             Main program body 
  - BLE_ApplicationInstallManager/Core/Src/stm32wbaxx_hal_msp.c                               This file provides code for the MSP Initialization and de-Initialization codes
  - BLE_ApplicationInstallManager/Core/Src/stm32wbaxx_it.c                                    Interrupt Service Routines. 
  - BLE_ApplicationInstallManager/Core/Src/system_stm32wbaxx.c                                CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 


### __Hardware and Software environment__

  - This application runs on STM32WBA65 Nucleo board.
    
### __How to use it?__

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory

The BLE_ApplicationInstallManager application must run with BLE applications embedding OTA service like: BLE_HeartRate_ota or BLE_p2pServer_ota applications.

__BLE_ApplicationInstallManager__ is loaded at the memory address __0x08000000__ and __BLE_HeartRate_ota__ or __BLE_p2pServer_ota__ application is loaded at the memory address __0x08006000__.

For more details on OTA process please refers to associated readme.html.
