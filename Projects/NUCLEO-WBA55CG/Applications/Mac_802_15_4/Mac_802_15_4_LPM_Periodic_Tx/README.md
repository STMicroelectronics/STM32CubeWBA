## __Mac_802_15_4_LPM_Periodic_Tx Application Description__

How to use MAC 802.15.4 data transmission with STOP1/Standby low power mode enabled.

This application requires one STM32WB55xx-Nucleo board. The device is silently started
as an end device. Once device initialized, the node periodically (every seconds) broadcast 
data over the air. The device goes in low power mode after each transmission and is awakened by
low power timer's interrupt.

Below example implements the RFD Devices.

### __Keywords__

MAC, 802.15.4, low power, Stop mode, Standby mode, STM32WBA , RTC timer, LPTIM1

### __Directory content__

Mac_802_15_4/Mac_802_15_4_RFD/

   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/app_common.h                            Header for all modules with common definition
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/app_conf.h                              Parameters configuration file of the application
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/app_entry.h                             Parameters configuration file of the application
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/hw_conf.h                               Configuration file of the HW
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/main.h                                  Header for main.c module
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/stm_logging.h                           Application header file for logging
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/stm32wbxx_hal_conf.h                    STM32 HAL configuration file.
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/stm32wbxx_it.h                          Interrupt handlers header file
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/utilities_conf.h                        Header for Configuration of utilities
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/STM32_WPAN/App/app_mac_802_15_4_process.h        Header for LPM MAC 802.15.4 processing tasks
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/STM32_WPAN/App/app_mac.h                         Header for LPM MAC 802.15.4 Core Mac Device implementation
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/app_entry.c                             Parameters configuration file of the application
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/stm32wbaxx_hal_msp.c                    Driver configuration
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/hw_uart.c                               UART Driver
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/main.c                                  Main program
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/stm_logging.c                           Logging Implementation
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/stm32wbxx_it.c                          exceptions handler and peripherals interrupt service routine
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/Core/Inc/system_stm32wbaxx.c                     CMSIS Cortex Device Peripheral Access Layer implementation
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/STM32_WPAN/App/app_mac.c    		      			LPM MAC 802.15.4 processing tasks implementation
   - Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx/STM32_WPAN/App/app_mac_802_15_4_process.c        LPM MAC 802.15.4 Core Mac Device implementation
 

### __Hardware and Software environment__

- This example runs on STM32WBA55xx devices.

- This example has been tested with an STMicroelectronics STM32WBA55CG-Nucleo 
  board and can be easily tailored to any other supported device 
  and development board.
  
- On STM32WBA55CG-Nucleo, the jumpers must be configured as described
  in this section. Starting from the top left position up to the bottom 
  right position, the jumpers on the Board must be set as follows:

  - CN11:    GND         [OFF]
  - JP4:     VDDRF       [ON]
  - JP6:     VC0         [ON]
  - JP2:     +3V3        [ON] 
  - JP1:     USB_STL     [ON]   All others [OFF]
  - CN12:    GND         [OFF]
  - CN7:     <All>       [OFF]
  - JP3:     VDD_MCU     [ON]
  - JP5:     GND         [OFF]  All others [ON]
  - CN10:    <All>       [OFF]

### __How to use it ?__

Minimum requirements for the demo:

- 1 STM32WBA55xx-Nucleo board in MAC 802.15.4 LPM mode 
using current generated binary.

In order to make the program work, you must do the following: 

- Connect a STM32WBA55xx-Nucleo board to your PC 
- Open your preferred toolchain 
- Rebuild all files of Mac_802_15_4/Mac_802_15_4_LPM_Periodic_Tx and load your image into 
  the target memory 
- The example run on Reset

Compile flag:

- By default in Standby mode with RTC timer to send data every seconds.
- You can set CFG_LPM_STDBY_SUPPORTED to 0, and the device switch to Stop mode with LPTIM to send data every seconds.

You can use an MAC 802.15.4 over the air sniffer on channel 11 to spy MAC 802.15.4 frames exchanged between the devices.
  
__START DEMO__

This MAC 802.15.4 example is to demonstrate a periodic data transmission from an End device configured in
low power mode.   

By default, on reset, the board starts. Once the device and its MAC layer 
are configured, the 802.15.4 node is silently stated and enter in low power mode.
A timer driven by the LPTIM1/RTC Timer is launch and configured to issue a Timeout interruption.
On Timeout event, the application core exit from the low power state and a data transmit data is requested by 
transmitter task. The application core returns in low power state until it received the MCPS data confirmation
from the RF protocol core. Before returning to the low power state, a new timer is then triggered on MCPS data 
confirmation to schedule a new data transmission. The demo runs in an infinite loops.
If user want to restart the demo, he may simply reset the device.

Power consumption can be measured using the X-Nucleo-LPM01A board solution.

On STM32WBA55xx, the consumption should be:

- Stop mode: 25 uA
- Standby mode: 6 uA
