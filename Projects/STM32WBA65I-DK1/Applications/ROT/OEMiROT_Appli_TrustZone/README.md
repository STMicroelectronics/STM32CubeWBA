## <b>OEMiROT_Appli_TrustZone Application Description</b>

This project provides a OEMiROT boot path application example. Boot is performed through OEMiROT boot path after authenticity and the integrity checks of the project firmware and project data
images.

This project is composed of two sub-projects:

- One for the secure application part (Project_s)

- One for the non-secure application part (Project_ns).


Please remember that on system with security enabled, the system always boots in secure and
the secure application is responsible for launching the non-secure application. When the secure application is started the MPU
is already configured (by OEMiROT) to limit the execution area to the project firmware execution slot. This is done in order to avoid
any malicious code execution from an unauthorised area (RAM, out of execution slot in user flash ...). Once started, it is up to the secure
application to adapt the security configuration to its needs. In this example, the MPU is simply disabled.

This project mainly shows how to switch from secure application to non-secure application
thanks to the system isolation performed to split the internal Flash and internal SRAM memories.
The split of the internal Flash depends on OEMiROT configuration done through the provisioning steps.
The split of the internal SRAM memories is independent from provisioning process and can be updated
at any time before compilation steps.

The non-secure application will display a menu on the console allowing to download through standard bootloader a new version
of the application firmware and the application data images.
At next reset, the OEMiROT will decrypt and install these new versions after successful check of the authenticity and the integrity of each image.

### <b>Keywords</b>

TrustZone, OEMiROT, Boot path, Root Of Trust, Security, MPU

### <b>Directory contents</b>

  - OEMiROT_Appli_TrustZone/Secure_nsclib/appli_flash_layout.h          Flash mapping for appli NS & S
  - OEMiROT_Appli_TrustZone/Secure_nsclib/appli_region_defs.h           RAM and FLASH regions definitions for appli NS & S

  - OEMiROT_Appli_TrustZone/Secure/Inc/low_level_flash.h                Header file for low_level_flash.c
  - OEMiROT_Appli_TrustZone/Secure/Inc/low_level_spi_flash.h            Header file for low_level_spi_flash.c
  - OEMiROT_Appli_TrustZone/Secure/Inc/m95p32_conf.h                    Header file for BSP spi flash configuration
  - OEMiROT_Appli_TrustZone/Secure/Inc/main.h                           Header file for main.c
  - OEMiROT_Appli_TrustZone/Secure/Inc/partition_stm32wbaxx.h           STM32WBAxx Device System Configuration file
  - OEMiROT_Appli_TrustZone/Secure/Inc/stm32_board.h                    Definitions for stm32wbaxx board
  - OEMiROT_Appli_TrustZone/Secure/Inc/stm32_hal.h                      Header file for wba specifics include
  - OEMiROT_Appli_TrustZone/Secure/Inc/stm32wbaxx_hal_conf.h            HAL configuration file
  - OEMiROT_Appli_TrustZone/Secure/Inc/stm32wba55g_discovery_conf.h     STM32WBA55G Discovery board configuration file
  - OEMiROT_Appli_TrustZone/Secure/Inc/stm32wba55g_discovery_errno.h    STM32WBA55G Discovery board errors file
  - OEMiROT_Appli_TrustZone/Secure/Inc/stm32wba65i_discovery_conf.h     STM32WBA65I Discovery board configuration file
  - OEMiROT_Appli_TrustZone/Secure/Inc/stm32wba65i_discovery_errno.h    STM32WBA65I Discovery board errors file
  - OEMiROT_Appli_TrustZone/Secure/Inc/stm32wbaxx_it.h                  Header file for stm32wbaxx_it.c

  - OEMiROT_Appli_TrustZone/Secure/Src/low_level_device.c               Flash Low level device setting
  - OEMiROT_Appli_TrustZone/Secure/Src/low_level_flash.c                Flash Low level interface
  - OEMiROT_Appli_TrustZone/Secure/Src/low_level_spi_device.c           External Flash Low level device configuration
  - OEMiROT_Appli_TrustZone/Secure/Src/low_level_spi_flash.c            External Flash Low level interface
  - OEMiROT_Appli_TrustZone/Secure/Src/main.c                           Secure Main program
  - OEMiROT_Appli_TrustZone/Secure/Src/secure_nsc.c                     Secure Non-secure callable functions
  - OEMiROT_Appli_TrustZone/Secure/Src/startup_stm32wbaxx.c             Startup file in c
  - OEMiROT_Appli_TrustZone/Secure/Src/stm32wbaxx_hal_msp.c             Secure HAL MSP module
  - OEMiROT_Appli_TrustZone/Secure/Src/stm32wbaxx_it.c                  Secure Interrupt handlers
  - OEMiROT_Appli_TrustZone/Secure/Src/system_stm32wbaxx.c              Secure STM32WBAxx system clock configuration file

  - OEMiROT_Appli_TrustZone/NonSecure/Inc/cmsis.h                       Header file for CMSIS
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/com.h                         Header file for com.c
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/common.h                      Header file for common.c
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/fw_update_app.h               Header file for fw_update.c
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/low_level_flash.h             Header file for low_level_flash.c
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/low_level_spi_flash.h         Header file for low_level_spi_flash.c
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/m95p32_conf.h                 Header file for BSP spi flash configuration
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/main.h                        Header file for main.c
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/Inc/mpu_armv8m_drv.h          Header file for mpu_armv8m_drv.c
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/ns_data.h                     Header file for ns_data.c
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/stm32_hal.h                   Header file for wba specifics include
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/stm32wbaxx_hal_conf.h         HAL configuration file
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/stm32wba55g_discovery_conf.h  STM32WBA55G Discovery board configuration file
  - OEMiROT_Appli_TrustZone/NonSecure/stm32wba55g_discovery_errno.h     STM32WBA55G Discovery board errors file
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/stm32wba65i_discovery_conf.h  STM32WBA65I Discovery board configuration file
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/stm32wba65i_discovery_errno.h STM32WBA65I Discovery board errors file
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/stm32wbaxx_it.h               Header file for stm32wbaxx_it.c
  - OEMiROT_Appli_TrustZone/NonSecure/Inc/ymodem.h                      Header file for ymodem.c

  - OEMiROT_Appli_TrustZone/NonSecure/Src/com.c                         Uart low level interface
  - OEMiROT_Appli_TrustZone/NonSecure/Src/common.c                      Uart common functionalities
  - OEMiROT_Appli_TrustZone/NonSecure/Src/fw_update_app.c               Firmware update example
  - OEMiROT_Appli_TrustZone/NonSecure/Src/low_level_device.c            Flash Low level device setting
  - OEMiROT_Appli_TrustZone/NonSecure/Src/low_level_flash.c             Flash Low level interface
  - OEMiROT_Appli_TrustZone/NonSecure/Src/low_level_spi_device.c        External Flash Low level device configuration
  - OEMiROT_Appli_TrustZone/NonSecure/Src/low_level_spi_flash.c         External Flash Low level interface
  - OEMiROT_Appli_TrustZone/NonSecure/Src/main.c                        Main application file
  - OEMiROT_Appli_TrustZone/NonSecure/Src/mpu_armv8m_drv.c              MPU low level interface
  - OEMiROT_Appli_TrustZone/NonSecure/Src/ns_data.c                     NS Data image example
  - OEMiROT_Appli_TrustZone/NonSecure/Src/startup_stm32wbaxx.c          Startup file in c
  - OEMiROT_Appli_TrustZone/NonSecure/Src/stm32wbaxx_it.c               Interrupt handlers
  - OEMiROT_Appli_TrustZone/NonSecure/Src/system_stm32wbaxx.c           System init file
  - OEMiROT_Appli_TrustZone/NonSecure/Src/ymodem.c                      Ymodem support

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA65I-DK1 devices with security enabled (TZEN=B4).
  - This example has been tested with STMicroelectronics STM32WBA65I-DK1 (MB2130)
  - To print the application menu in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.

### <b>How to use it ?</b>

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker files
will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through oemirot_appli_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running provisioning.bat (.sh) from ROT_Provisioning/OEMiROT or ROT_Provisioning/OEMiROT_OEMuROT,
and follow its instructions. Refer to ROT_Provisioning/OEMiROT or ROT_Provisioning/OEMiROT_OEMuROT readme for more information on
the provisioning process for OEMiROT boot path or OEMiROT_OEMuROT boot path.

After application startup, check in your "UART console" the menu is well displayed:
```
=================== Main Menu =============================
    New Firmware Image ------------------------------------ 1
    Selection :
```

To update the application firmware and/or the application data image version, you must:

  - Connect Tera-Term application (or equivalent) through UART console
  - Select the function "New Fw Image"
  - Download the new image(s) through YModem protocole
  - Reset the board
  - After authenticity and intergrity checks the new images are decrypted and installed.

For more details, refer to STM32WBA Wiki articles:

  - [STM32WBA security](https://wiki.st.com/stm32mcu/wiki/Category:STM32WBA).
  - [OEMiRoT OEMuRoT for STM32WBA](https://wiki.st.com/stm32mcu/wiki/Security:OEMiRoT_OEMuRoT_for_STM32WBA).
  - [How to start with ROT on STM32WBA](https://wiki.st.com/stm32mcu/wiki/Category:How_to_start_with_RoT_on_WBA).

#### <b>Note:</b>

  1. The most efficient way to develop and debug an application is to boot directly on user flash in RDL level 0 by setting with
     STM32CubeProgrammer the RDP to 0xAA and the SECBOOTADD to (0x0C006000 + offset of the firmware execution slot).

  2. Two versions of ROT_AppliConfig are available: windows executable and python version. By default, the windows executable is selected. It
     is possible to switch to python version by:
        - installing python (Python 3.10 or newer) with the required modules listed in requirements.txt.
        ```
        pip install -r requirements.txt
        ```
        - having python in execution path variable
        - deleting main.exe in Utilities\PC_Software\ROT_AppliConfig\dist
