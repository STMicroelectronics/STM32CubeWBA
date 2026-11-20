## __Zigbee_OccupSensing_Server_SED_WithTempCalib Application Description__

This application implements a Zigbee Occupancy Sensing __Server__ that operates as a __Sleepy End Device (SED)__ in a centralized Home Automation network. It joins the network formed by `Zigbee_OccupSensing_Client_Coord`, exposes the Occupancy Sensing server cluster, and publishes room-occupancy changes through standard Zigbee Cluster Library (ZCL) attribute reporting.


### __Keywords__

Connectivity, Zigbee, IEEE 802.15.4, Home Automation profile, Occupancy Sensing cluster, Sleepy End Device, ZCL reporting, STM32WBA55xx, internal temperature sensor.


### __Hardware Requirements__

The demonstration requires:

- One STM32WBA55xx Nucleo board programmed with `Zigbee_OccupSensing_Client_Coord`. It is the Zigbee coordinator and Occupancy Sensing client.
- One STM32WBA55xx Nucleo board programmed with `Zigbee_OccupSensing_Server_SED_WithTempCalib`. It is the Occupancy Sensing server and Sleepy End Device.
- A USB connection to each board for programming and, optionally, viewing the UART trace through the ST-LINK Virtual COM Port.


### __Application Overview__

At reset, `Core/Src/main.c` initializes the HAL, STM32_WPAN support package, GPIO, DMA, instruction cache, RAM configuration, RTC, and ADC4. It then starts the Zigbee application scheduler with `MX_APPE_Init()`. The main loop continuously calls `MX_APPE_Process()`, which executes Zigbee-stack, timer, and sequencer processing.

The server creates a Home Automation endpoint with the following configuration:

| Item | Configuration |
| --- | --- |
| Zigbee role | Sleepy End Device (SED) |
| Endpoint | 17 |
| Profile | Home Automation |
| Device type | On/Off Occupancy Sensor |
| Cluster | Occupancy Sensing server (`ZCL_CLUSTER_OCCUPANCY`) |
| Attribute updated by the application | `ZCL_OCC_ATTR_OCCUPANCY` |
| Default RF transmit power | 0 dBm |
| Channel selection | 2.4 GHz Zigbee channel mask |

The application starts with the occupancy value cleared (`0`, room empty), then writes it to the server cluster as soon as the device joins the network.


### __Network Joining and Low-Power Operation__

`APP_ZIGBEE_ApplicationInit()` configures the device to join an existing Zigbee network (`ZbStartTypeJoin`). The server uses the default Home Automation preconfigured link key and retries after a failed attempt. While joining, the join-status LED blinks every second; it remains on after association succeeds.

The startup configuration clears the receiver-on-when-idle, router, and alternate-coordinator capabilities, making the node a Sleepy End Device. The SED timeout setting is `1`, corresponding to a two-minute end-device timeout; the Zigbee stack sends its keep-alive automatically every quarter of that interval. Low-power operation is authorized after the device has joined and the application has started.

A short press of SW1 restarts joining when the device is not connected. A long press of SW1 sends a Zigbee leave request. After a successful leave, the application disables Zigbee periodic timers and waits for a subsequent short SW1 press to join again.


### __Occupancy State Handling__

SW2 controls the demonstrator occupancy value. Each press toggles the one-bit `cOccupancyState` variable between:

- `0`: room empty;
- `1`: room occupied.

The button handler schedules `APP_ZIGBEE_AttributeOccupancyUpdate()` through the STM32 sequencer. That task writes `cOccupancyState` to `ZCL_OCC_ATTR_OCCUPANCY` by calling `ZbZclAttrIntegerWrite()` on the Occupancy Sensing server cluster. A button press updates the local ZCL attribute; the Zigbee stack sends a report when the coordinator's reporting configuration permits it.


### __Temperature Sensor and Calibration Scope__

The project initializes ADC4 as a single-conversion, 12-bit ADC and selects the STM32 internal temperature-sensor channel (`ADC_CHANNEL_TEMPSENSOR`) with a long sampling time (814.5 ADC cycles). This setup is present in `Core/Src/main.c` and is consistent with the `WithTempCalib` project name.

However, the current application sources do __not__ start an ADC conversion, convert raw ADC data to degrees Celsius, apply factory calibration constants, or expose/report a temperature ZCL attribute. The active Zigbee behavior is occupancy sensing only. Temperature measurement or calibration must be implemented separately before it can affect occupancy logic or be transmitted to the coordinator.


### __Interaction with Zigbee_OccupSensing_Client_Coord__

`Zigbee_OccupSensing_Client_Coord` forms the centralized Zigbee network as coordinator on channel 14. It creates an Occupancy Sensing __client__ on endpoint 17 and registers handlers for ZCL commands and attribute reports.

When the coordinator receives a device-join indication, it saves the server short address and starts a one-shot 20-second timer. At expiry, it sends a Configure Reporting request to the server for `ZCL_OCC_ATTR_OCCUPANCY`. The request uses normal reporting direction, an 8-bit bitmap attribute type, a minimum interval of 0 seconds, a maximum interval of 10 seconds, and a reportable-change value of 1.

After reporting is configured, a SW2 press on the server changes the occupancy attribute. The Zigbee stack sends an attribute report to the coordinator according to the configured interval and change threshold. The coordinator validates the report length and endpoint, logs either “Room is occupied” or “Room is empty,” and toggles its work LED.

```text
+------------------------------------------------+        +------------------------------------------------------+
| Zigbee_OccupSensing_Client_Coord               |        | Zigbee_OccupSensing_Server_SED_WithTempCalib        |
| Coordinator / Occupancy Client / Endpoint 17    |        | Sleepy End Device / Occupancy Server / Endpoint 17  |
+------------------------------------------------+        +------------------------------------------------------+
             |                                                           |
             | Forms the Zigbee network on channel 14                    |
             |<-------------------------- joins -------------------------|
             |                                                           |
             | Configure Reporting for ZCL_OCC_ATTR_OCCUPANCY            |
             |-------------------------> (20 s after join) -------------|
             |                                                           |
             |               SW2 toggles empty/occupied                  |
             |                                                           |
             |<-------------------- occupancy attribute report ----------|
             | logs state and toggles the work LED                        |
```


### __Application Setup__

1. Import, build, and program `Zigbee_OccupSensing_Client_Coord` on the first STM32WBA55xx Nucleo board.
2. Import, build, and program `Zigbee_OccupSensing_Server_SED_WithTempCalib` on the second board.
3. Start the coordinator first and wait until its join-status LED indicates that the Zigbee network has been formed.
4. Power or reset the server. Its join-status LED blinks while it searches for and joins the coordinator network, then remains on after association succeeds.
5. Allow at least 20 seconds after the server joins. This gives the coordinator time to send its Configure Reporting request.
6. Press SW2 on the server to alternate between empty and occupied. Observe the corresponding report and work-LED toggle on the coordinator, or view messages through the UART trace.
7. Use a short press of SW1 on the server to retry joining after a leave; use a long press of SW1 to leave the network.


### __Diagnostics and Error Indication__

Application and Zigbee-stack messages are available on USART1 through the ST-LINK Virtual COM Port. The UART is configured for 115200 baud, 8 data bits, 1 stop bit, no parity, and no hardware flow control.

If the Zigbee application enters its fatal-error handler, it continuously alternates the join and work LEDs every 500 ms. The generic MCU `Error_Handler()` disables interrupts and remains in an infinite loop.


### __Hardware and Software environment__

- The application targets STM32WBA55xx devices and is configured for an STM32WBA55CGA Nucleo board.
- It is an STM32CubeIDE project using the STM32 HAL, STM32_WPAN middleware, Zigbee stack, and Zigbee Cluster Library.
- The radio uses the 2.4 GHz IEEE 802.15.4 Zigbee band. The coordinating companion application is configured for channel 14.
- The server UART trace is available through USART1 and the ST-LINK Virtual COM Port.
- On the STM32WBA55CGA Nucleo board, configure the jumpers as follows: JP1 1-2 ON and JP1 3-4 through 9-10 OFF; JP2 1-2 ON.


### __Note__

The two applications must use compatible Zigbee security and channel settings to join the same network. This project uses the default Home Automation preconfigured link key. The coordinator configures reporting only after it receives the server join indication, so reports are not expected until the server has joined and the 20-second reporting-configuration delay has elapsed.

Despite the project name and ADC4 temperature-sensor initialization, this version does not contain runtime temperature acquisition or calibration code. Do not interpret the occupancy reports as temperature-calibrated data.

By default, this application runs with Zigbee PRO stack R23.
If you want to run this application using Zigbee PRO stack R22, you should replace ZigBeeProR23_FFD.a by ZigBeeProR22_FFD.a and ZigBeeProR23_RFD.a by ZigBeeProR22_RFD.a and ZigBeeClusters.a by ZigBeeClustersR22.a in the build environment.
Also, set in the project setup compilation flag CONFIG_ZB_REV=22.
