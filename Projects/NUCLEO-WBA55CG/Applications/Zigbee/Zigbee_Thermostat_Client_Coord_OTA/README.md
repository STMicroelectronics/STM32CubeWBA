# __Zigbee_Thermostat_Client_Coord_OTA Application Description__

How to use the Thermostat cluster with Over The Air Firmware Update feature.

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, Zigbee Firmware Update Over The Air.

### __Hardware and Software environment__

- This application runs on STM32WBA55xx Nucleo board.
- Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (`USB_STLINK`).

### __How to use it?__

In order to make the program work:

- Open your preferred toolchain:
  - Rebuild all files (`Zigbee_ApplicationInstallManager`, `Zigbee_Thermostat_Server_Router_OTA` and `Zigbee_Thermostat_Client_Coord_OTA`).
  - On the directory of your preferred toolchain launch `ImageBuilderST.bat` that create the image of file: `ZigbeeThermostatServerRouterOTA_Image.bin`.
- Flash the first board (Device):
  - `Zigbee_ApplicationInstallManager` binary file is downloaded at the memory address **0x08000000**.
  - `Zigbee_Thermostat_Server_Router_OTA` binary file is downloaded at the memory address **0x08006000**.
- Flash the second board (Coordinator):
  - `Zigbee_Thermostat_Client_Coord_OTA` binary file is downloaded at the memory address **0x08000000**.
  - `ZigbeeThermostatServerRouterOTA_Image.bin` binary file is downloaded at the memory address **0x08080000**.

### __Demo use case__

- This demo shows how to use Thermostat cluster:
  - The Thermostat Server update regularly (500 ms) its **`ZCL_THERM_SVR_ATTR_LOCAL_TEMP`** attributes (via the measure of a sensor or by random).
  - The Thermostat Client send a Report Request to obtains regularly the **`ZCL_THERM_SVR_ATTR_LOCAL_TEMP`** attributes.
  - The Thermostat Server send regularly (5 s) a Report with the latest value of **`ZCL_THERM_SVR_ATTR_LOCAL_TEMP`** attributes.
- The Firmware Update Over The Air (FUOTA) cluster provides the possibility to receive a new application to be installed while the current application is running. Download is done by Coordinator by the press of `SW3`.
- 1 read-only attributes is used:
  - Temperature Value (16-bit integer)

```text
               Coord.                                                                    Router
           +------------+                                                                +------------+
           |            |                                                                |            |
           | Thermostat |                                                                | Thermostat |
           |   Client   |                                                                | Server     |  - Temperature Server during Init
           |            |                                                                |            |    launch a 500 ms Periodic Timer (Green Led toggling)
           |            |                                                                |            |
           |            |                                                                |            |  - Every 500 ms
           |            |                                                                |            |    * Read the Temperature Sensor (if exist)
           |            |                                                                |            |      or simulate it with RNG and the Thermostat setting.
           |            |                                                                |            |    * <= ZbZclAttrIntegerWrite(ZCL_THERM_SVR_ATTR_LOCAL_TEMP)
           |            |  After a Router was connected :                                |            |    * Display Local Temperature and Relay State (OPEN or CLOSED
           |            |  ZbZclAttrReportConfigReq(ZCL_THERM_SVR_ATTR_LOCAL_TEMP, 5sec) |            |      depending on thermostat setting).
           |            | -------------------------------------------------------------> |            |
           |            | <------------------------------------------------------------- |            |
           |            |                                                                |            |
           |            | <-------------- Report (every 5 seconds) --------------------- |            |
           |            |                                                                |            |
           |            |                                                                |            | <= PushB SW1/SW2/SW3 : Change a Thermostat setting (see Menu description below)
           |            |                                                                |            |
           |            |                                                                |            |
           |            |                                                                |            |
           +----------+                                                                  +------------+
```

### __Application Setup__

- First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
- To run the application:
  1. Start the first board. It must be the coordinator of the Zigbee network, so in this demo application it is the device running `Zigbee_Thermostat_Client_Coord` application.
  2. Wait for the Blue LED ON.
  3. During this stage, Coordinator verify if a FW Binary is saved on it Flash Memory, since address `0x0808000`. If it's the case, it display information about this FW Binary.
  4. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator. Do the same for the other boards if applicable.
     - At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons.
  5. After this setting, the Coord launc a Report Request to Device. After confirmation, the Device send regularly it Report.
  6. By press the `SW3` button on Coordinator, it send a Request to all Devices to verify if an update of this FW is necessary (based on the version). If it's the case, the download start.

**Note:** When LED Red, Green and Blue are toggling it is indicating an error has occurred on application.

### __Hardware and Software environment__

- This example runs on STM32WBA55xx devices.
- This example has been tested with an STMicroelectronics `STM32WBA55CGA_Nucleo` board and can be easily tailored to any other supported device and development board.
- On `STM32WBA55CGA_Nucleo`, the jumpers must be configured as described in this section. Starting from the top left position up to the bottom right position, the jumpers on the Board must be set as follows:
  - **JP1:**
    - 1-2: [ON]
    - 3-4: [OFF]
    - 5-6: [OFF]
    - 7-8: [OFF]
    - 9-10: [OFF]
  - **JP2:**
    - 1-2: [ON]

### __Traces__

- To get the traces you need to connect your Board to the Hyperterminal (through the STLink Virtual COM Port).
- The UART must be configured as follows:
  - BaudRate = 115200 baud
  - Word Length = 8 Bits
  - Stop Bit = 1 bit
  - Parity = none
  - Flow control = none
  - Terminal “Go to the Line” : `<LF>`

### __Note__

By default, this application runs with Zigbee PRO stack `R23`.
If you want to run this application using Zigbee PRO stack `R22`, you should replace `ZigBeeProR23_FFD.a` by `ZigBeeProR22_FFD.a` and `ZigBeeProR23_RFD.a` by `ZigBeeProR22_RFD.a` and `ZigBeeClusters.a` by `ZigBeeClustersR22.a` in the build environment.
Also, set in the project setup compilation flag `CONFIG_ZB_REV=22`.
