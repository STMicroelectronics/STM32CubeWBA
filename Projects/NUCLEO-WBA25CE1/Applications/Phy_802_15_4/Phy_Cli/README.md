
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## __Phy_Cli Application Description__
 
A dedicated application allows the control and the test of the 802_15_4 radio via a command line interface or STM32CubeMonitorRF.

### __Keywords__

  Connectivity, 802.15.4, Commands

### __Directory contents__

  - Phy_802_15_4/Phy_Cli/Core/Inc/app_common.h                				Header of App Common application configuration file for STM32WPAN Middleware
  - Phy_802_15_4/Phy_Cli/Core/Inc/app_conf.h                 			 	Header of Application configuration file for STM32WPAN Middleware
  - Phy_802_15_4/Phy_Cli/Core/Inc/app_entry.h                 				Header of Application Entry
  - Phy_802_15_4/Phy_Cli/Core/Inc/main.h                    				Header of Main program
  - Phy_802_15_4/Phy_Cli/Core/Inc/stm32wbaxx_hal_conf.h       				Header of HAL configuration
  - Phy_802_15_4/Phy_Cli/Core/Inc/stm32wbaxx_it.h             				Header of the interrupt handlers
  - Phy_802_15_4/Phy_Cli/Core/Inc/utilities_conf.h            				Header for configuration file to utilities
  - Phy_802_15_4/Phy_Cli/Core/Inc/stm32wbaxx_nucleo_conf.h
  - Phy_802_15_4/Phy_Cli/Core/Src/app_entry.c                 				Application Entry
  - Phy_802_15_4/Phy_Cli/Core/Src/main.c                      				Main program
  - Phy_802_15_4/Phy_Cli/Core/Src/stm32wbaxx_hal_msp.c        				This file provides code for the MSP Initialization and de-Initialization codes
  - Phy_802_15_4/Phy_Cli/Core/Src/stm32wbaxx_it.c             				This file provides all exceptions handler and peripherals interrupt service routine
  - Phy_802_15_4/Phy_Cli/Core/Src/system_stm32wbaxx.c
  - common_connectivity/Connectivity/App/cli_runner/app_cli_runner.c        Entry point for the 802.15.4 CLI
  - common_connectivity/Connectivity/App/cli_runner/app_cli_runner_ex.c     Entry point for the PHY CLI
  - common_connectivity/Connectivity/App/cli_runner/app_phy_cli.h           Header for the 802.15.4 PHY CLI application
  - common_connectivity/Connectivity/App/cli_runner/app_cli_runner.h        Header for the 802.15.4 PHY CLI application
  - Phy_802_15_4/Phy_Cli/STM32_WPAN/Target/linklayer_plat.c
  - Phy_802_15_4/Phy_Cli/STM32_WPAN/Target/ll_sys_if.c
  - Phy_802_15_4/Phy_Cli/STM32_WPAN/Target/ll_sys_if.h
  - Phy_802_15_4/Phy_Cli/STM32_WPAN/Target/power_table.c
  - Phy_802_15_4/Phy_Cli/System/Config/Debug_GPIO/app_debug.c
  - Phy_802_15_4/Phy_Cli/System/Config/Debug_GPIO/app_debug.h
  - Phy_802_15_4/Phy_Cli/System/Config/Debug_GPIO/app_debug_signal_def.h
  - Phy_802_15_4/Phy_Cli/System/Config/Debug_GPIO/debug_config.h
  - Phy_802_15_4/Phy_Cli/System/Config/Flash/simple_nvm_arbiter_conf.h
  - Phy_802_15_4/Phy_Cli/System/Config/LowPower/app_sys.h
  - common_connectivity/Connectivity/Interfaces/hw.h
  - common_connectivity/Connectivity/Interfaces/hw_aes.h
  - common_connectivity/Connectivity/Interfaces/hw_if.h                Header of Hardware Interface
  - common_connectivity/Connectivity/Interfaces/hw_otp.c
  - common_connectivity/Connectivity/Interfaces/hw_pka.c
  - common_connectivity/Connectivity/Interfaces/hw_rng.c
  - common_connectivity/Connectivity/Interfaces/pka_p256.c
  - common_connectivity/Connectivity/Interfaces/stm32_lpm_if.c         Source of Low layer function to enter/exit low power modes (stop, sleep)
  - common_connectivity/Connectivity/Interfaces/stm32_lpm_if.h         Header for stm32_lpm_if.c module (device specific LP management)
  - common_connectivity/Connectivity/Interfaces/timer_if.c             Source file configuration of RTC Alarm, Tick and Calendar manager
  - common_connectivity/Connectivity/Interfaces/timer_if.h             Header for configuration of the timer_if.c instances
  - Phy_802_15_4/Phy_Cli/System/Interfaces/usart_if.c             Source file for interfacing the stm32_adv_trace to hardware
  - Phy_802_15_4/Phy_Cli/System/Interfaces/usart_if.h             Header file for stm32_adv_trace interface file
  - common_connectivity/Connectivity/Modules/Flash/flash_driver.c
  - common_connectivity/Connectivity/Modules/Flash/flash_driver.h
  - common_connectivity/Connectivity/Modules/Flash/flash_manager.c
  - common_connectivity/Connectivity/Modules/Flash/flash_manager.h
  - common_connectivity/Connectivity/Modules/Flash/rf_timing_synchro.c
  - common_connectivity/Connectivity/Modules/Flash/rf_timing_synchro.h
  - common_connectivity/Connectivity/Modules/Flash/simple_nvm_arbiter.c
  - common_connectivity/Connectivity/Modules/Flash/simple_nvm_arbiter.h
  - common_connectivity/Connectivity/Modules/Flash/simple_nvm_arbiter_common.h
  - common_connectivity/Connectivity/Modules/debug_signals.h
  - common_connectivity/Connectivity/Modules/local_debug_tables.h
  - common_connectivity/Connectivity/Modules/RTDebug/RTDebug.c
  - common_connectivity/Connectivity/Modules/RTDebug/RTDebug.h
  - common_connectivity/Connectivity/Modules/RTDebug/RTDebug_dtb.c
  - common_connectivity/Connectivity/Modules/RTDebug/RTDebug_dtb.h
  - common_connectivity/Connectivity/Modules/adc_crtl.c
  - common_connectivity/Connectivity/Modules/adc_crtl.h
  - common_connectivity/Connectivity/Modules/app_sys.c             Source of of system and radio low power mode configuration
  - Phy_802_15_4/Phy_Cli/System/Config/app_sys.h          		   Header of system and radio low power mode configuration
  - common_connectivity/Connectivity/Modules/dbg_trace.h
  - common_connectivity/Connectivity/Modules/otp.c
  - common_connectivity/Connectivity/Modules/otp.h
  - common_connectivity/Connectivity/Modules/scm.c                 Functions for the System Clock Manager.
  - common_connectivity/Connectivity/Modules/scm.h                 Header for scm.c module
  - common_connectivity/Connectivity/Startup/stm32wbaxx_ResetHandler.s
  - Phy_802_15_4/Phy_Cli/System/Modules/utilities_common.h

### __How to use it?__ 

  - This example runs on STM32WBA25xx devices.

  - This example has been tested with an STMicroelectronics STM32WBA25xx
    board and can be easily tailored to any other supported device
    and development board.

  - You can either run it as standalone tool, or with STM32CubeMonitorRF
  
#### __STM32CubeMonitorRF__

  - Please refer to the STM32CubeMonitorRF user guide

### __Standalone binary__

  - The commands are sent via an CLI UART (USART1) from an HyperTerminal to the STM32WB25CE_Nucleo Board.
  - The results are sent via an CLI UART (USART1) from the STM32WB25CE_Nucleo Board (PC) to an HyperTerminal or STM32CubeMonitorRF.
  - At startup. '802_15_4_CLI Appli' should be displayed on the PRINT UART Hyperterminal.

  - The Hyperterminal must be configured as follow :
      - BaudRate = 115200 baud  
      - Word Length = 8 Bits 
      - Stop Bit = 1 bit
      - Parity = none
      - Flow control = none

#### __Full PHY certification and dynamic RX printing__

  - In order to perform a fully exhaustive PHY certification, it may be necessary to enable the "dynamic RX printing" using the *rx_start 1* command.
    This option enables print on the UART the payload of the frames received as they arrive. ST does not guarantee that all frames will be displayed
    using default baudrate of 115200.
  - By setting FULL_CERTIFICATION_CAPABLE to 1 the following features are enabled:
    - the Baud rate increases to 576000, where it has been verified that the application can display 1000 frames with a payload of 12 bytes with no delay in between.
  - Keep in mind this flag disables the STM32CubeMonitorRF compatibility, which explains why it is not enabled by default.

### __Examples of commands__

  - Help
  - Version
  - Set_channel **<channel>**														*Select the 15.4 channel*
  - Set_power **<power>**															*Select TX *power in dbm*
  - Set_smps **<enable/disable>**													*Enable or disable the SMPS*
  - Set_CCA_threshold **<threshold>**												*Set CCA threshold in dbm*
  - TX_start 0x0C,0x01,0x08,0x01,0x22,0x11,0xFF,0xFF,0xB5,0xB6,0xB7				*Send a frame (first byte here 0x0C is the size of the payload + 2 to account for the CRC)*
  - TX_start 0x0C,0x01,0x08,0x01,0x22,0x11,0xFF,0xFF,0xB5,0xB6,0xB7 **<number>** **<delay>**	*Send a frame **number** time with **delay** in ms between each packet*
  - TX_start 0x0C,0x01,0x08,0x01,0x22,0x11,0xFF,0xFF,0xB5,0xB6,0xB7 0			*Send the frame infinitely until TX_stop is called*
  - TX_stop 																	*Stop an ongoing transmission*
  - TX_start 0x0C,0x21,0x08,0x01,0x22,0x11,0xFF,0xFF,0xB5,0xB6,0xB7             *Send a frame with ACK request (requires another CLI with Rx_start)*
  - RX_start 																	*Start a reception*
  - RX_start 1																	*Start a reception, prints paylaod of frames as they arrive*
  - RX_stop																		*Stop a reception*

### __Limitations__
 
  - Updating the SMPS configuration through the *Set_smps* command only works before any radio activity has been started (e.g. any *rx_start* or *tx_start*). A reset of
    the board is required in order to change it.
:::
:::

