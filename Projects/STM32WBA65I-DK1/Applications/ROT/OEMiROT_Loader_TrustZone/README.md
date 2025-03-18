## <b>OEMiROT_Loader_TrustZone Application description</b>

This project is used to generate the OEMiROT loader image file.
The OEMiROT_Loader_TrustZone application consists in two distinct projects: the OEMiROT_Loader_TrustZone
Secure and Non Secure projects.
It has to be compiled after all OEMiROT other ptojects.

In order to ease the development process, prebuild and postbuild commands are
integrated in each toolchain project.
The prebuild command is in charge of preparing the scatter file according to common
flash layout description in linker folder.
The postbuild command is in charge of preparing the loader image.

### <b>Keywords</b>

Security, OEMiROT, FLASH, TrustZone

### <b>Directory contents</b>

  - OEMiROT_Loader_TrustZone/NonSecure/Inc/com.h                         Header file for com.c
  - OEMiROT_Loader_TrustZone/NonSecure/Inc/common.h                      Header file for common.c
  - OEMiROT_Loader_TrustZone/NonSecure/Inc/fw_update_app.h               Header file for fw_update_app.c
  - OEMiROT_Loader_TrustZone/NonSecure/Inc/low_level_flash.h             Header file for low_level_flash.c
  - OEMiROT_Loader_TrustZone/NonSecure/Inc/main.h                        Header file for main.c
  - OEMiROT_Loader_TrustZone/NonSecure/Inc/stm32wbaxx_hal_conf.h         HAL driver configuration file
  - OEMiROT_Loader_TrustZone/NonSecure/Inc/ymodem.h                      Header file for ymodem.c
  - OEMiROT_Loader_TrustZone/NonSecure/Inc/stm32wba55g_discovery_conf.h  STM32WBA55G Discovery board configuration file
  - OEMiROT_Loader_TrustZone/NonSecure/Inc/stm32wba65i_discovery_conf.h  STM32WBA65I Discovery board configuration file

  - OEMiROT_Loader_TrustZone/NonSecure/Src/com.c                         Com functionalities
  - OEMiROT_Loader_TrustZone/NonSecure/Src/common.c                      Common functionalities
  - OEMiROT_Loader_TrustZone/NonSecure/Src/dummy_nsc.c                   Dummy function
  - OEMiROT_Loader_TrustZone/NonSecure/Src/fw_update_app.c               Firmware update functionalities
  - OEMiROT_Loader_TrustZone/NonSecure/Src/low_level_device.c            Flash Low level device configuration
  - OEMiROT_Loader_TrustZone/NonSecure/Src/low_level_flash.c             Flash Low level interface
  - OEMiROT_Loader_TrustZone/NonSecure/Src/main.c                        Main application file
  - OEMiROT_Loader_TrustZone/NonSecure/Src/startup_stm32wbaxx.c          Startup file in c
  - OEMiROT_Loader_TrustZone/NonSecure/Src/system_stm32wbaxx.c           System Init file
  - OEMiROT_Loader_TrustZone/NonSecure/Src/tick.c                        Tick functionalities
  - OEMiROT_Loader_TrustZone/NonSecure/Src/ymodem.c                      Ymodem functionalities

  - OEMiROT_Loader_TrustZone/Secure/Inc/low_level_flash.h                Header file for low_level_flash.c
  - OEMiROT_Loader_TrustZone/Secure/Inc/main.h                           Header file for main.c
  - OEMiROT_Loader_TrustZone/Secure/Inc/partition_stm32wbaxx.h           Partition file
  - OEMiROT_Loader_TrustZone/Secure/Inc/stm32wbaxx_hal_conf.h            HAL driver configuration file
  - OEMiROT_Loader_TrustZone/Secure/Inc/stm32wba55g_discovery_conf.h     STM32WBA55G Discovery board configuration file
  - OEMiROT_Loader_TrustZone/Secure/Inc/stm32wba65i_discovery_conf.h     STM32WBA65I Discovery board configuration file

  - OEMiROT_Loader_TrustZone/Secure/Src/low_level_device.c               Flash Low level device configuration
  - OEMiROT_Loader_TrustZone/Secure/Src/low_level_flash.c                Flash Low level interface
  - OEMiROT_Loader_TrustZone/Secure/Src/main.c                           Main application file
  - OEMiROT_Loader_TrustZone/Secure/Src/secure_nsc.c                     Secure Non-secure callable functions
  - OEMiROT_Loader_TrustZone/Secure/Src/startup_stm32wbaxx.c             Startup file in c
  - OEMiROT_Loader_TrustZone/Secure/Src/system_stm32wbaxx.c              System Init file
  - OEMiROT_Loader_TrustZone/Secure/Src/tick.c                           Tick functionalities

  - OEMiROT_Loader_TrustZone/Secure_nsclib/secure_nsc.h                  Secure Non-Secure Callable (NSC) module header file

### <b>How to use it ?</b>

In order to build the OEMiROT_Loader_TrustZone Secure and Non Secure projects, you must do the following:
 - Open your preferred toolchain
 - Rebuild the two projects

Then refer to OEMiROT readme at upper level directory for OEMiROT usage.
