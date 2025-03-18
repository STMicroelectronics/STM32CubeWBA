## __BLE_p2pServer_TZ Application Description__

This example is similar to BLE_p2pServer with the TrustZone being activated and the Blue LED being connected to the Secure side of the MCU.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, TrustZone

### __Directory contents__

 - BLE_p2pServer_TZ/NonSecure/Core/Inc/app_common.h                                   App Common application configuration file for STM32WPAN Middleware.
 - BLE_p2pServer_TZ/NonSecure/Core/Inc/app_conf.h                                     Application configuration file for STM32WPAN Middleware.
 - BLE_p2pServer_TZ/NonSecure/Core/Inc/app_entry.h                                    Interface to the application
 - BLE_p2pServer_TZ/NonSecure/Core/Inc/main.h                                         Header for main.c file.
 - BLE_p2pServer_TZ/NonSecure/Core/Inc/stm32wbaxx_hal_conf.h                          HAL configuration file.
 - BLE_p2pServer_TZ/NonSecure/Core/Inc/stm32wbaxx_it.h                                This file contains the headers of the interrupt handlers.
 - BLE_p2pServer_TZ/NonSecure/Core/Inc/stm32wbaxx_nucleo_conf.h                       STM32WBAXX nucleo board configuration file.
 - BLE_p2pServer_TZ/NonSecure/Core/Inc/utilities_conf.h                               Header for configuration file for STM32 Utilities.
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/App/app_ble.h                                Header for ble application
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/App/ble_conf.h                               Configuration file for BLE Middleware.
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/App/ble_dbg_conf.h                           Debug configuration file for BLE Middleware.
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/App/p2p_server.h                             Header for p2p_server.c
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/App/p2p_server_app.h                         Header for p2p_server_app.c
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/Target/bpka.h                                This file contains the interface of the BLE PKA module.
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/Target/host_stack_if.h                       This file contains the interface for the stack tasks
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/Target/ll_sys_if.h                           Header file for initiating system
 - BLE_p2pServer_TZ/NonSecure/System/Config/ADC_Ctrl/adc_ctrl_conf.h                               Configuration Header for ADC controller module 
 - BLE_p2pServer_TZ/NonSecure/System/Config/CRC_Ctrl/crc_ctrl_conf.h                               Configuration Header for CRC controller module 
 - BLE_p2pServer_TZ/NonSecure/System/Config/Debug_GPIO/app_debug.h                    Real Time Debug module application APIs and signal table
 - BLE_p2pServer_TZ/NonSecure/System/Config/Debug_GPIO/app_debug_signal_def.h         Real Time Debug module application signal definition
 - BLE_p2pServer_TZ/NonSecure/System/Config/Debug_GPIO/debug_config.h                 Real Time Debug module general configuration file
 - BLE_p2pServer_TZ/NonSecure/System/Config/Flash/simple_nvm_arbiter_conf.h           Configuration header for simple_nvm_arbiter.c module
 - BLE_p2pServer_TZ/NonSecure/System/Config/Log/log_module.h                          Header file of the log module.
 - BLE_p2pServer_TZ/NonSecure/System/Config/LowPower/app_sys.h                        Header for app_sys.c
 - BLE_p2pServer_TZ/NonSecure/System/Config/LowPower/peripheral_init.h                Header for peripheral init module
 - BLE_p2pServer_TZ/NonSecure/System/Config/LowPower/user_low_power_config.h          Header for user_low_power_config.c
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/hw.h                                  This file contains the interface of STM32 HW drivers.
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/hw_if.h                               Hardware Interface
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/stm32_lpm_if.h                        Header for stm32_lpm_if.c module (device specific LP management)
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/timer_if.h                            Configuration of the timer_if.c instances
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/usart_if.h                            Header file for stm32_adv_trace interface file
 - BLE_p2pServer_TZ/NonSecure/System/Modules/adc_ctrl.h                               Header for ADC client manager module
 - BLE_p2pServer_TZ/NonSecure/System/Modules/ble_timer.h                              This header defines the timer functions used by the BLE stack
 - BLE_p2pServer_TZ/NonSecure/System/Modules/crc_ctrl.h                                            Header for CRC client manager module 
 - BLE_p2pServer_TZ/NonSecure/System/Modules/dbg_trace.h                              Header for dbg_trace.c
 - BLE_p2pServer_TZ/NonSecure/System/Modules/otp.h                                    Header file for One Time Programmable (OTP) area
 - BLE_p2pServer_TZ/NonSecure/System/Modules/scm.h                                    Header for scm.c module
 - BLE_p2pServer_TZ/NonSecure/System/Modules/stm_list.h                               Header file for linked list library.
 - BLE_p2pServer_TZ/NonSecure/System/Modules/temp_measurement.h                                    Header file for temperature measurement module
 - BLE_p2pServer_TZ/NonSecure/System/Modules/utilities_common.h                       Common file to utilities
 - BLE_p2pServer_TZ/NonSecure/System/Modules/baes/baes.h                              This file contains the interface of the basic AES software module.
 - BLE_p2pServer_TZ/NonSecure/System/Modules/baes/baes_global.h                       This file contains the internal definitions of the AES software
module.
 - BLE_p2pServer_TZ/NonSecure/System/Modules/Flash/flash_driver.h                     Header for flash_driver.c module
 - BLE_p2pServer_TZ/NonSecure/System/Modules/Flash/flash_manager.h                    Header for flash_manager.c module
 - BLE_p2pServer_TZ/NonSecure/System/Modules/Flash/rf_timing_synchro.h                Header for rf_timing_synchro.c module
 - BLE_p2pServer_TZ/NonSecure/System/Modules/Flash/simple_nvm_arbiter.h               Header for simple_nvm_arbiter.c module
 - BLE_p2pServer_TZ/NonSecure/System/Modules/Flash/simple_nvm_arbiter_common.h        Common header of simple_nvm_arbiter.c module
 - BLE_p2pServer_TZ/NonSecure/System/Modules/MemoryManager/advanced_memory_manager.h  Header for advance_memory_manager.c module
 - BLE_p2pServer_TZ/NonSecure/System/Modules/MemoryManager/stm32_mm.h                 Header for stm32_mm.c module
 - BLE_p2pServer_TZ/NonSecure/System/Modules/Nvm/nvm.h                                This file contains the interface of the NVM manager.
 - BLE_p2pServer_TZ/NonSecure/System/Modules/RFControl/rf_antenna_switch.h            RF related module to handle dedictated GPIOs for antenna switch
 - BLE_p2pServer_TZ/NonSecure/System/Modules/RTDebug/debug_signals.h                  Real Time Debug module System and Link Layer signal definition
 - BLE_p2pServer_TZ/NonSecure/System/Modules/RTDebug/local_debug_tables.h             Real Time Debug module System and Link Layer signal
 - BLE_p2pServer_TZ/NonSecure/System/Modules/RTDebug/RTDebug.h                        Real Time Debug module API declaration
 - BLE_p2pServer_TZ/NonSecure/System/Modules/RTDebug/RTDebug_dtb.h                    Real Time Debug module API declaration for DTB usage
 - BLE_p2pServer_TZ/NonSecure/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.h       Header file for the serial commands interpreter module.
 - BLE_p2pServer_TZ/Secure/Core/Inc/main.h                                            Header for main.c file.
 - BLE_p2pServer_TZ/Secure/Core/Inc/partition_stm32wba55xx.h                          CMSIS STM32WBA55xx Device Initial Setup for Secure / Non-Secure Zones
for ARMCM33 based on CMSIS CORE V5.4.0 partition_ARMCM33.h Template.
 - BLE_p2pServer_TZ/Secure/Core/Inc/stm32wbaxx_hal_conf.h                             HAL configuration file.
 - BLE_p2pServer_TZ/Secure/Core/Inc/stm32wbaxx_it.h                                   This file contains the headers of the interrupt handlers.
 - BLE_p2pServer_TZ/Secure/Core/Inc/stm32wbaxx_nucleo_conf.h                          STM32WBAXX nucleo board configuration file.
 - BLE_p2pServer_TZ/Secure_nsclib/secure_nsc.h                                        Header for secure non-secure callable APIs list
 - BLE_p2pServer_TZ/NonSecure/Core/Src/app_entry.c                                    Entry point of the application
 - BLE_p2pServer_TZ/NonSecure/Core/Src/main.c                                         Main program body
 - BLE_p2pServer_TZ/NonSecure/Core/Src/stm32wbaxx_hal_msp.c                           This file provides code for the MSP Initialization
and de-Initialization codes.
 - BLE_p2pServer_TZ/NonSecure/Core/Src/stm32wbaxx_it.c                                Interrupt Service Routines.
 - BLE_p2pServer_TZ/NonSecure/Core/Src/system_stm32wbaxx_ns.c                         CMSIS Cortex-M33 Device Peripheral Access Layer System Source File
to be used in non-secure application when the system implements the TrustZone-M security.
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/App/app_ble.c                                BLE Application
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/App/p2p_server.c                             p2p_server definition.
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/App/p2p_server_app.c                         p2p_server_app application definition.
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/Target/bleplat.c                             This file implements the platform functions for BLE stack library.
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/Target/bpka.c                                This file implements the BLE PKA module.
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/Target/host_stack_if.c                       Source file for the stack tasks
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/Target/linklayer_plat.c                      Source file for the linklayer plateform adaptation layer
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/Target/ll_sys_if.c                           Source file for initiating system
 - BLE_p2pServer_TZ/NonSecure/STM32_WPAN/Target/power_table.c                         This file contains supported power tables
 - BLE_p2pServer_TZ/NonSecure/System/Config/ADC_Ctrl/adc_ctrl_conf.c                               Source for ADC client controller module configuration file 
 - BLE_p2pServer_TZ/NonSecure/System/Config/CRC_Ctrl/crc_ctrl_conf.c                               Source for CRC client controller module configuration file 
 - BLE_p2pServer_TZ/NonSecure/System/Config/Debug_GPIO/app_debug.c                    Real Time Debug module application side APIs
 - BLE_p2pServer_TZ/NonSecure/System/Config/Flash/simple_nvm_arbiter_conf.c           The Simple NVM arbiter module provides an interface to write and/or
restore data from SRAM to FLASH with use of NVMs.
 - BLE_p2pServer_TZ/NonSecure/System/Config/Log/log_module.c                          Source file of the log module.
 - BLE_p2pServer_TZ/NonSecure/System/Config/LowPower/peripheral_init.c                peripheral reinitialization
 - BLE_p2pServer_TZ/NonSecure/System/Config/LowPower/user_low_power_config.c          Low power related user configuration.
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/hw_aes.c                              This file contains the AES driver for STM32WBA
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/hw_otp.c                              This file contains the OTP driver.
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/hw_pka.c                              This file contains the PKA driver for STM32WBA
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/hw_rng.c                              This file contains the RNG driver for STM32WBA
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/pka_p256.c                            This file is an optional part of the PKA driver for STM32WBA.
It is dedicated to the P256 elliptic curve.
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/stm32_lpm_if.c                        Low layer function to enter/exit low power modes (stop, sleep)
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/timer_if.c                            Configure RTC Alarm, Tick and Calendar manager
 - BLE_p2pServer_TZ/NonSecure/System/Interfaces/usart_if.c                            Source file for interfacing the stm32_adv_trace to hardware
 - BLE_p2pServer_TZ/NonSecure/System/Modules/adc_ctrl.c                               Header for ADC client manager module
 - BLE_p2pServer_TZ/NonSecure/System/Modules/app_sys.c                                Application system for STM32WPAN Middleware.
 - BLE_p2pServer_TZ/NonSecure/System/Modules/ble_timer.c                              This module implements the timer core functions
 - BLE_p2pServer_TZ/NonSecure/System/Modules/crc_ctrl.c                                            Source for CRC client controller module 
 - BLE_p2pServer_TZ/NonSecure/System/Modules/otp.c                                    Source file for One Time Programmable (OTP) area
 - BLE_p2pServer_TZ/NonSecure/System/Modules/scm.c                                    Functions for the System Clock Manager.
 - BLE_p2pServer_TZ/NonSecure/System/Modules/stm_list.c                               TCircular Linked List Implementation.
 - BLE_p2pServer_TZ/NonSecure/System/Modules/temp_measurement.c                                    Temperature measurement module
 - BLE_p2pServer_TZ/NonSecure/System/Modules/baes/baes_cmac.c                         This file contains the AES CMAC implementation.
 - BLE_p2pServer_TZ/NonSecure/System/Modules/baes/baes_ecb.c                          This file contains the AES ECB functions implementation.
 - BLE_p2pServer_TZ/NonSecure/System/Modules/Flash/flash_driver.c                     The Flash Driver module is the interface layer between Flash
management modules and HAL Flash drivers
 - BLE_p2pServer_TZ/NonSecure/System/Modules/Flash/flash_manager.c                    The Flash Manager module provides an interface to write raw data
from SRAM to FLASH
 - BLE_p2pServer_TZ/NonSecure/System/Modules/Flash/rf_timing_synchro.c                The RF Timing Synchronization module provides an interface to
synchronize the flash processing versus the RF activity to make sure the RF timing is not broken
 - BLE_p2pServer_TZ/NonSecure/System/Modules/Flash/simple_nvm_arbiter.c                                   The Simple NVM arbiter module provides an interface to write and/or
restore data from SRAM to FLASH with use of NVMs.
 - BLE_p2pServer_TZ/NonSecure/System/Modules/MemoryManager/advanced_memory_manager.c                      Memory Manager
 - BLE_p2pServer_TZ/NonSecure/System/Modules/MemoryManager/stm32_mm.c                                   
 - BLE_p2pServer_TZ/NonSecure/System/Modules/Nvm/nvm_emul.c                           This file implements the RAM version of the NVM manager for
STM32WBX. It is made for test purpose.
 - BLE_p2pServer_TZ/NonSecure/System/Modules/RFControl/rf_antenna_switch.c            RF related module to handle dedictated GPIOs for antenna switch
 - BLE_p2pServer_TZ/NonSecure/System/Modules/RTDebug/RTDebug.c                        Real Time Debug module API definition
 - BLE_p2pServer_TZ/NonSecure/System/Modules/RTDebug/RTDebug_dtb.c                    Real Time Debug module API definition for DTB usage
 - BLE_p2pServer_TZ/NonSecure/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.c               Source file for the serial commands interpreter module.
 - BLE_p2pServer_TZ/Secure/Core/Src/main.c                                            Main program body
 - BLE_p2pServer_TZ/Secure/Core/Src/secure_nsc.c                                      This file contains the non-secure callable APIs (secure world)
 - BLE_p2pServer_TZ/Secure/Core/Src/stm32wbaxx_hal_msp.c                              This file provides code for the MSP Initialization
and de-Initialization codes.
 - BLE_p2pServer_TZ/Secure/Core/Src/stm32wbaxx_it.c                                   Interrupt Service Routines.
 - BLE_p2pServer_TZ/Secure/Core/Src/system_stm32wbaxx_s.c                             CMSIS Cortex-M33 Device Peripheral Access Layer System Source File
to be used in secure application when the system implements
the TrustZone-M security.

### __Hardware and Software environment__

  - This application runs on STM32WBA55 Nucleo board.
  - Another Nucleo board may be necessary to run p2pClient or p2pRouter application.
    
### __How to use it?__

Please refer to dedicated wiki page describing how to setup and use this application:
 <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_BLE_%26_TrustZone"> Wiki: STM32WBA Bluetooth® LE & TrustZone</a>
 
With the following Option bytes configuration : 
TZEN = 1
SECBOOTADD0 = 0x180000
SECWM1_PSTRT = 0x0
SECWM1_PEND = 0x7F
SECWM2_PSTRT = 0x7F
SECWM2_PEND = 0x0

