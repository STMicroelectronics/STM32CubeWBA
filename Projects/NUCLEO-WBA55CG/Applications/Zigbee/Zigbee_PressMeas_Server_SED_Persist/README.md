## __Zigbee_PressMeas_Server_SED_Persist Application Description__

This application implements a Zigbee Pressure Measurement server running as a centralized Sleepy End Device (SED). It works with `Zigbee_PressMeas_Client_Coord_Persist`, which creates the network and hosts the corresponding Pressure Measurement client.

The SED joins the coordinator network, exposes the Pressure Measurement cluster, periodically updates its measured-value attribute, and reports that attribute with the configuration received from the coordinator. Zigbee state is saved in non-volatile memory so the device can restore its network context after reset or power loss.


### __Keywords__

Connectivity, Zigbee, Zigbee protocol, IEEE 802.15.4, Sleepy End Device, centralized network, Pressure Measurement cluster, ZCL reporting, persistence.


### __Hardware Requirements__

- One STM32WBA55xx Nucleo board programmed with `Zigbee_PressMeas_Client_Coord_Persist`.
- One STM32WBA55xx Nucleo board programmed with `Zigbee_PressMeas_Server_SED_Persist`.

The application has been tested with an STM32WBA55CGA Nucleo board. Completely erase Flash before programming to avoid obsolete persistence data.


### __Network Roles and Interaction__

`Zigbee_PressMeas_Client_Coord_Persist` is the centralized coordinator: it forms the network, detects devices joining or rejoining, and hosts a Pressure Measurement client. `Zigbee_PressMeas_Server_SED_Persist` is a centralized SED: it does not form a network and joins the coordinator through the configured 2.4 GHz channel mask using the default Home Automation preconfigured link key.

After the SED joins, the coordinator saves its short address. After 20 seconds, it sends a ZCL Configure Reporting request to endpoint 17. This configures `MeasuredValue` (`ZCL_PRESS_MEAS_ATTR_MEAS_VAL`) with minimum and maximum intervals of 30 seconds and a reportable change of 0. The server therefore reports its current value every 30 seconds. The coordinator parses each signed 16-bit report, logs the server address and pressure, and toggles its status LED.


## __Zigbee Endpoint and Cluster Configuration__

The server creates endpoint 17 with the Home Automation profile and Pressure Sensor device type. It allocates and registers a Pressure Measurement server cluster with a valid range from 0 to 32448. It also configures the Basic server cluster with STM32WBA product information and directs Zigbee logs to the application trace service.

The SED capability clears receiver-on-when-idle, router/device-type, and alternate-coordinator capability bits. Its end-device timeout exponent is 1, corresponding to a two-minute timeout; the stack sends keep-alive traffic at one quarter of that timeout.


### __Pressure Measurement Operation__

After joining or restoring the network, the application initializes pressure to 1014 hPa and starts a 30-second periodic timer. Each expiration schedules a pressure-read task:

1. When `BOARD_IS_WITH_PRESS_SENSOR` is defined, this is the intended location for a board-specific sensor read.
2. Otherwise, a random byte simulates a sensor and increases or decreases pressure by 10 hPa.
3. The value is constrained to the range from 0 to 32448.
4. The signed 16-bit value is written to `ZCL_PRESS_MEAS_ATTR_MEAS_VAL` with `ZbZclAttrIntegerWrite`.
5. The server toggles its status LED and logs the updated pressure.

The local update interval and reporting interval are independent. The simulated value changes every 30 seconds, while the coordinator-configured reporting rule transmits the latest value every 30 seconds.


### __Startup, Joining, and Rejoining__

At boot, the STM32 platform, WPAN middleware, Zigbee stack, endpoint, cluster, and scheduler tasks are initialized. The application first attempts to restore a saved Zigbee context. If a valid record is found and persistent startup succeeds, it resumes directly on the network. If no valid record is available, it waits for user action before a normal join attempt.

The join LED blinks during joining or rejoining. On success it remains on, the short address is logged, and the pressure-update timer starts. A failed join is retried after 500 ms; each startup callback can wait for up to 60 seconds.


### __Persistence__

A dedicated 24 KiB Flash area at the end of device Flash holds serialized Zigbee stack data. When the stack signals a state change, the application retrieves it with `ZbPersistGet` and writes it with a sequence count, data CRC, and header CRC. The data is verified after writing.

On a later boot, the module scans Flash, validates the headers and data CRCs, selects the newest valid record, and provides it to `ZbStartupPersist`. The SED can then reconnect without commissioning again while the saved network information remains valid.


### __Push-Button Controls__

- SW1, short press: starts joining or rejoining when the device is not started on a network.
- SW1, long press: sends a Zigbee leave request and disables periodic Zigbee timers after a successful leave.
- SW2: toggles the local 30-second pressure-update timer. It freezes the value update but does not remove an existing reporting rule, so the last value can continue to be reported.
- SW3: erases all persistence data, waits one second, and resets the MCU. A new join operation is then needed.

If persistence is erased on one device, related Zigbee devices should also be erased or recommissioned as needed to preserve a consistent network state.


### __LED and Trace Behavior__

The join LED blinks while the SED attempts to join and remains on after association. The status LED toggles after each local pressure update. UART traces report startup, join/rejoin activity, short address, pressure updates, persistence activity, and Zigbee or ZCL errors. A fatal Zigbee application error makes the join and work LEDs alternate at 500 ms intervals.


### __Demo Procedure__

1. Completely erase and program the coordinator board with `Zigbee_PressMeas_Client_Coord_Persist`.
2. Completely erase and program the SED board with `Zigbee_PressMeas_Server_SED_Persist`.
3. Start the coordinator and wait until its network is ready.
4. Start the SED. If it has no valid persistence record, briefly press SW1 to start joining.
5. Wait until the SED join LED stops blinking and remains on.
6. The coordinator detects the SED, waits 20 seconds, then configures `MeasuredValue` reporting.
7. Observe server pressure updates every 30 seconds and coordinator reports every 30 seconds.
8. Use SW2 to freeze or resume local updates. Use SW3 only when fresh commissioning is required.


### __UART Traces__

Connect the ST-LINK Virtual COM Port with the following settings:

- Baud rate: 115200
- Data bits: 8
- Stop bits: 1
- Parity: none
- Flow control: none
- Line ending: LF

### __Note__

By default, this application runs with Zigbee PRO stack R23.
If you want to run this application using Zigbee PRO stack R22, you should replace ZigBeeProR23_FFD.a by ZigBeeProR22_FFD.a and ZigBeeProR23_RFD.a by ZigBeeProR22_RFD.a and ZigBeeClusters.a by ZigBeeClustersR22.a in the build environment.
Also, set in the project setup compilation flag CONFIG_ZB_REV=22.
