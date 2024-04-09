## __BLE_SerialCom_Central Application Description__

How to demonstrate Point-to-Point communication using BLE component. 

The Nucleo board flashed with BLE_SerialCom_Central application acts as GAP central.


### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing


### __Directory contents__

  - BLE_SerialCom_Central/Core/Inc/app_common.h                                     App Common application configuration file for STM32WPAN Middleware. 
  - BLE_SerialCom_Central/Core/Inc/app_conf.h                                       Application configuration file for STM32WPAN Middleware. 
  - BLE_SerialCom_Central/Core/Inc/app_entry.h                                      Interface to the application 
  - BLE_SerialCom_Central/Core/Inc/main.h                                           Header for main.c file. This file contains the common defines of the application.
  - BLE_SerialCom_Central/Core/Inc/stm32wbaxx_hal_conf.h                            HAL configuration file. 
  - BLE_SerialCom_Central/Core/Inc/stm32wbaxx_it.h                                  This file contains the headers of the interrupt handlers. 
  - BLE_SerialCom_Central/Core/Inc/stm32wbaxx_nucleo_conf.h                         STM32WBAXX nucleo board configuration file. This file should be copied to the application folder and renamed
to stm32wbaxx_nucleo_conf.h .
  - BLE_SerialCom_Central/Core/Inc/utilities_conf.h                                 Header for configuration file for STM32 Utilities. 
  - BLE_SerialCom_Central/STM32_WPAN/App/app_ble.h                                  Header for ble application 
  - BLE_SerialCom_Central/STM32_WPAN/App/ble_conf.h                                 Configuration file for BLE Middleware. 
  - BLE_SerialCom_Central/STM32_WPAN/App/ble_dbg_conf.h                             Debug configuration file for BLE Middleware. 
  - BLE_SerialCom_Central/STM32_WPAN/App/gatt_client_app.h                          Header for gatt_client_app.c module 
  - BLE_SerialCom_Central/STM32_WPAN/Target/bpka.h                                  This file contains the interface of the BLE PKA module. 
  - BLE_SerialCom_Central/STM32_WPAN/Target/host_stack_if.h                         This file contains the interface for the stack tasks 
  - BLE_SerialCom_Central/STM32_WPAN/Target/ll_sys_if.h                             Header file for ll_sys_if.c
  - BLE_SerialCom_Central/System/Config/ADC_Ctrl/adc_ctrl_conf.h                               Configuration Header for ADC controller module 
  - BLE_SerialCom_Central/System/Config/CRC_Ctrl/crc_ctrl_conf.h                               Configuration Header for CRC controller module 
  - BLE_SerialCom_Central/System/Config/Debug_GPIO/app_debug.h                      Real Time Debug module application APIs and signal table 
  - BLE_SerialCom_Central/System/Config/Debug_GPIO/app_debug_signal_def.h           Real Time Debug module application signal definition 
  - BLE_SerialCom_Central/System/Config/Debug_GPIO/debug_config.h                   Real Time Debug module general configuration file 
  - BLE_SerialCom_Central/System/Config/Flash/simple_nvm_arbiter_conf.h             Configuration header for simple_nvm_arbiter.c module 
  - BLE_SerialCom_Central/System/Config/Log/log_module.h                                       Configuration Header for log module 
  - BLE_SerialCom_Central/System/Config/LowPower/app_sys.h                          Header for app_sys.c 
  - BLE_SerialCom_Central/System/Config/LowPower/peripheral_init.h                             Header for peripheral init module 
  - BLE_SerialCom_Central/System/Config/LowPower/user_low_power_config.h            Header for user_low_power_config.c
  - BLE_SerialCom_Central/System/Interfaces/hw.h                                    This file contains the interface of STM32 HW drivers. 
  - BLE_SerialCom_Central/System/Interfaces/hw_if.h                                 Hardware Interface 
  - BLE_SerialCom_Central/System/Interfaces/stm32_lpm_if.h                          Header for stm32_lpm_if.c module (device specific LP management) 
  - BLE_SerialCom_Central/System/Interfaces/timer_if.h                              configuration of the timer_if.c instances 
  - BLE_SerialCom_Central/System/Interfaces/usart_if.h                              Header file for stm32_adv_trace interface file 
  - BLE_SerialCom_Central/System/Modules/adc_ctrl.h                                 Header for ADC client manager module 
  - BLE_SerialCom_Central/System/Modules/ble_timer.h                                This header defines the timer functions used by the BLE stack 
  - BLE_SerialCom_Central/System/Modules/crc_ctrl.h                                            Header for CRC client manager module 
  - BLE_SerialCom_Central/System/Modules/dbg_trace.h                                Header for dbg_trace.c 
  - BLE_SerialCom_Central/System/Modules/otp.h                                      Header file for One Time Programmable (OTP) area 
  - BLE_SerialCom_Central/System/Modules/scm.h                                      Header for scm.c module 
  - BLE_SerialCom_Central/System/Modules/stm_list.h                                 Header file for linked list library. 
  - BLE_SerialCom_Central/System/Modules/temp_measurement.h                                    Header file for temperature measurement module
  - BLE_SerialCom_Central/System/Modules/utilities_common.h                         Common file to utilities 
  - BLE_SerialCom_Central/System/Modules/baes/baes.h                                This file contains the interface of the basic AES software module. 
  - BLE_SerialCom_Central/System/Modules/baes/baes_global.h                         This file contains the internal definitions of the AES software module.
  - BLE_SerialCom_Central/System/Modules/Flash/flash_driver.h                       Header for flash_driver.c module 
  - BLE_SerialCom_Central/System/Modules/Flash/flash_manager.h                      Header for flash_manager.c module 
  - BLE_SerialCom_Central/System/Modules/Flash/rf_timing_synchro.h                  Header for rf_timing_synchro.c module 
  - BLE_SerialCom_Central/System/Modules/Flash/simple_nvm_arbiter.h                 Header for simple_nvm_arbiter.c module 
  - BLE_SerialCom_Central/System/Modules/Flash/simple_nvm_arbiter_common.h          Common header of simple_nvm_arbiter.c module 
  - BLE_SerialCom_Central/System/Modules/MemoryManager/advanced_memory_manager.h    Header for advance_memory_manager.c module 
  - BLE_SerialCom_Central/System/Modules/MemoryManager/stm32_mm.h                   Header for stm32_mm.c module 
  - BLE_SerialCom_Central/System/Modules/Nvm/nvm.h                                  This file contains the interface of the NVM manager. 
  - BLE_SerialCom_Central/System/Modules/RFControl/rf_antenna_switch.h              RF related module to handle dedictated GPIOs for antenna switch
  - BLE_SerialCom_Central/System/Modules/RTDebug/debug_signals.h                    Real Time Debug module System and Link Layer signal definition 
  - BLE_SerialCom_Central/System/Modules/RTDebug/local_debug_tables.h               Real Time Debug module System and Link Layer signal 
  - BLE_SerialCom_Central/System/Modules/RTDebug/RTDebug.h                          Real Time Debug module API declaration 
  - BLE_SerialCom_Central/System/Modules/RTDebug/RTDebug_dtb.h                      Real Time Debug module API declaration for DTB usage
  - BLE_SerialCom_Central/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.h         Header file for the serial commands interpreter module
  - BLE_SerialCom_Central/Core/Src/app_entry.c                                      Entry point of the application 
  - BLE_SerialCom_Central/Core/Src/main.c                                           Main program body 
  - BLE_SerialCom_Central/Core/Src/stm32wbaxx_hal_msp.c                             This file provides code for the MSP Initialization and de-Initialization codes.
  - BLE_SerialCom_Central/Core/Src/stm32wbaxx_it.c                                  Interrupt Service Routines. 
  - BLE_SerialCom_Central/Core/Src/system_stm32wbaxx.c                              CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_SerialCom_Central/STM32_WPAN/App/app_ble.c                                  BLE Application 
  - BLE_SerialCom_Central/STM32_WPAN/App/gatt_client_app.c                          GATT Client Application 
  - BLE_SerialCom_Central/STM32_WPAN/Target/bleplat.c                               This file implements the platform functions for BLE stack library. 
  - BLE_SerialCom_Central/STM32_WPAN/Target/bpka.c                                  This file implements the BLE PKA module. 
  - BLE_SerialCom_Central/STM32_WPAN/Target/host_stack_if.c                         Source file for the stack tasks 
  - BLE_SerialCom_Central/STM32_WPAN/Target/linklayer_plat.c                        Source file for the linklayer plateform adaptation layer 
  - BLE_SerialCom_Central/STM32_WPAN/Target/ll_sys_if.c                             Source file for initiating the system sequencer 
  - BLE_SerialCom_Central/STM32_WPAN/Target/power_table.c                           This file contains supported power tables 
  - BLE_SerialCom_Central/System/Config/ADC_Ctrl/adc_ctrl_conf.c                               Source for ADC client controller module configuration file 
  - BLE_SerialCom_Central/System/Config/CRC_Ctrl/crc_ctrl_conf.c                               Source for CRC client controller module configuration file 
  - BLE_SerialCom_Central/System/Config/Debug_GPIO/app_debug.c                      Real Time Debug module application side APIs 
  - BLE_SerialCom_Central/System/Config/Log/log_module.c                                       Source file of the log module 
  - BLE_SerialCom_Central/System/Config/LowPower/user_low_power_config.c            Low power related user configuration
  - BLE_SerialCom_Central/System/Config/LowPower/peripheral_init.c                             Source for peripheral init module 
  - BLE_SerialCom_Central/System/Interfaces/hw_aes.c                                This file contains the AES driver for STM32WBA 
  - BLE_SerialCom_Central/System/Interfaces/hw_otp.c                                This file contains the OTP driver. 
  - BLE_SerialCom_Central/System/Interfaces/hw_pka.c                                This file contains the PKA driver for STM32WBA 
  - BLE_SerialCom_Central/System/Interfaces/hw_rng.c                                This file contains the RNG driver for STM32WBA 
  - BLE_SerialCom_Central/System/Interfaces/pka_p256.c                              This file is an optional part of the PKA driver for STM32WBA. It is dedicated to the P256 elliptic curve.
  - BLE_SerialCom_Central/System/Interfaces/stm32_lpm_if.c                          Low layer function to enter/exit low power modes (stop, sleep) 
  - BLE_SerialCom_Central/System/Interfaces/timer_if.c                              Configure RTC Alarm, Tick and Calendar manager 
  - BLE_SerialCom_Central/System/Interfaces/usart_if.c                              Source file for interfacing the stm32_adv_trace to hardware 
  - BLE_SerialCom_Central/System/Modules/adc_ctrl.c                                 Header for ADC client manager module 
  - BLE_SerialCom_Central/System/Modules/app_sys.c                                  Application system for STM32WPAN Middleware. 
  - BLE_SerialCom_Central/System/Modules/ble_timer.c                                This module implements the timer core functions 
  - BLE_SerialCom_Central/System/Modules/crc_ctrl.c                                            Source for CRC client controller module 
  - BLE_SerialCom_Central/System/Modules/otp.c                                      Source file for One Time Programmable (OTP) area 
  - BLE_SerialCom_Central/System/Modules/scm.c                                      Functions for the System Clock Manager. 
  - BLE_SerialCom_Central/System/Modules/stm_list.c                                 TCircular Linked List Implementation. 
  - BLE_SerialCom_Central/System/Modules/temp_measurement.c                                    Temperature measurement module
  - BLE_SerialCom_Central/System/Modules/baes/baes_cmac.c                           This file contains the AES CMAC implementation. 
  - BLE_SerialCom_Central/System/Modules/baes/baes_ecb.c                            This file contains the AES ECB functions implementation. 
  - BLE_SerialCom_Central/System/Modules/Flash/flash_driver.c                       The Flash Driver module is the interface layer between Flash management modules and HAL Flash drivers
  - BLE_SerialCom_Central/System/Modules/Flash/flash_manager.c                      The Flash Manager module provides an interface to write raw data from SRAM to FLASH
  - BLE_SerialCom_Central/System/Modules/Flash/rf_timing_synchro.c                  The RF Timing Synchronization module provides an interface to synchronize the flash processing versus the RF activity to make
sure the RF timing is not broken
  - BLE_SerialCom_Central/System/Modules/Flash/simple_nvm_arbiter.c                 The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs.
  - BLE_SerialCom_Central/System/Modules/MemoryManager/advanced_memory_manager.c    Memory Manager 
  - BLE_SerialCom_Central/System/Modules/MemoryManager/stm32_mm.c                   Memory Manager
  - BLE_SerialCom_Central/System/Modules/Nvm/nvm_emul.c                             This file implements the RAM version of the NVM manager for STM32WBX. It is made for test purpose.
  - BLE_SerialCom_Central/System/Modules/RFControl/rf_antenna_switch.c              RF related module to handle dedictated GPIOs for antenna switch
  - BLE_SerialCom_Central/System/Modules/RTDebug/RTDebug.c                          Real Time Debug module API definition 
  - BLE_SerialCom_Central/System/Modules/RTDebug/RTDebug_dtb.c                      Real Time Debug module API definition for DTB usage
  - BLE_SerialCom_Central/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.c         Source file for the serial commands interpreter module 

### __Hardware and Software environment__

  - This example runs on STM32WBA55xx devices.
    Connect the Nucleo Board to your PC with a USB cable type A to mini-B to ST-LINK connector (USB_STLINK).


### __How to use it?__

In order to make the program work, you must do the following:

 - Open IAR toolchain 
 - Rebuild all files and flash the board with the executable file.
 
Two STM32WBA nucleo boards are used.
One board is configured as central thanks to BLE_SerialCom_Central and the other as peripheral thanks to BLE_SerialCom_Peripheral.
The central board supports the Data transfer by sending L2CAP data through a COC link.

No service is added.

After BLE link is established, a Connection Oriented Channel (COC) is established (full stack is necessary for COC)
Data are exchanged through the COC link.

Open a VT100 terminal on Central and Peripheral side (ST Link Com Port, @115200 bauds).
This application has to be used in front of BLE_SerialCom_Peripheral.

At reset application initialization is done.

 - The peripheral device starts advertising.
 - Push B1 on central: the central device scans and automatically connects to the peripheral (use of CFG_DEV_ID_BLE_COC). 
 - After BLE connection:
 - the client establishes a COC channel.

- When link is ready: display "CONNECTION READY"
- Configure the terminal with settings local echo.
- When Append LF is configured, enter data using keyboard, check that data are correctly received on peer device.

There are two different modes to send messages from an STM32WBA: "Send button" or "Real time".
After reset, "Real time" mode is selected.
In terminal, when you type the message:
- In "Send button" mode, data are sent when LF is sent.
- In "Real time" mode, each character is immediately sent.

On server side when connected:

- B1 changes "send data mode"
- B2 sends slave security request

On server side when idle:

- B1 changes "send data mode"
- B2 clear database

On client side when connected:

- B1 changes "send data mode"
- B2 sends pairing request
- B3 changes connection interval update

On client side when idle:

- B2 clear database

