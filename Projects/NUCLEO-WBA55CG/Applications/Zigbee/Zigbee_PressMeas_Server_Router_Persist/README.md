## __Zigbee_PressMeas_Server_Router_Persist Application Description__


This application makes an STM32WBA55xx board a Zigbee __router__ implementing the ZCL __Pressure Measurement server__ cluster. It is designed to operate with `Zigbee_PressMeas_Client_Coord_Persist`, which acts as the centralized-network coordinator and Pressure Measurement client.

Once joined, the router periodically obtains or simulates a pressure value, writes it to `ZCL_PRESS_MEAS_ATTR_MEAS_VAL`, and supplies that value through standard ZCL attribute reporting. Zigbee network data is persisted in non-volatile memory so the device can recover its network membership after a reset or power cycle.


### __Keywords__

Zigbee, Zigbee PRO, centralized network, router, coordinator, ZCL, Pressure Measurement cluster, attribute reporting, persistence, STM32WBA55xx, NUCLEO-WBA55xx.


### __Hardware Requirements__

- One STM32WBA55xx Nucleo board programmed with `Zigbee_PressMeas_Client_Coord_Persist` (coordinator and Pressure Measurement client).
- One or more STM32WBA55xx boards programmed with `Zigbee_PressMeas_Server_Router_Persist` (routers and Pressure Measurement servers).
- A USB connection for programming and, optionally, an ST-LINK Virtual COM Port connection for traces.


### __Application Roles and Interaction__

| Application | Zigbee role | ZCL role | Responsibility |
|---|---|---|---|
| `Zigbee_PressMeas_Client_Coord_Persist` | Coordinator | Pressure Measurement client | Forms the network, opens a join window when requested, configures reporting, and receives reports. |
| `Zigbee_PressMeas_Server_Router_Persist` | Router | Pressure Measurement server | Joins and extends the mesh, maintains the local pressure attribute, and reports it using the client configuration. |

The two applications use compatible Zigbee PRO settings: the Home Automation profile, endpoint `17`, the HA preconfigured link key, channel `14`, and a default transmit power of `0 dBm`.


### __Server Code Architecture__

The server behavior is implemented mainly in these source files:

- `STM32_WPAN/App/app_zigbee.c`: Zigbee-stack initialization, startup/rejoin handling, retry timers, and persistence startup.
- `STM32_WPAN/App/app_zigbee_endpoint.c`: endpoint and cluster setup, pressure acquisition/simulation, sequencer tasks, LEDs, and push-button actions.
- `STM32_WPAN/App/app_zigbee_persistence_nvm.c`: non-volatile Zigbee persistence support.
- `STM32_WPAN/App/app_zigbee_utility.c`: common application utilities.

The code creates a Home Automation pressure-sensor endpoint (`17`) and allocates a `ZbZclPressMeasServer` cluster. The cluster supports values from `0` to `32448`; the current signed 16-bit measurement is written locally with `ZbZclAttrIntegerWrite(..., ZCL_PRESS_MEAS_ATTR_MEAS_VAL, ...)`.


### __Network Startup and Join__

At initialization, the router sets `ZbStartTypeJoin`, registers application tasks, configures the endpoint, and starts the Zigbee layers.

1. The application first schedules its persistence-rejoin task.
2. When saved Zigbee state is valid, `AppZbPersistence_Init(...)` restores it and the stack tries to rejoin the previous network.
3. Without usable persistent state, the application waits for a short press on __SW1__ before starting a join attempt.
4. The join-status LED blinks while a join or rejoin is in progress.
5. A failed startup is retried after the configured startup-failure delay until a compatible coordinator is reachable and accepting joins.

A long press on __SW1__ sends a Zigbee leave request. After the leave completes, periodic stack timers are disabled and the router requires a new startup before it can join again.


### __Pressure Measurement Processing__

After successful startup, the server initializes the current value to `1014 hPa` and starts its periodic measurement timer. In the current source code, the timer period is __30 seconds__.

The timer callback schedules a sequencer task. The task then:

1. Reads a physical pressure sensor if `BOARD_IS_WITH_PRESS_SENSOR` is defined; otherwise it simulates a sensor.
2. In simulation mode, gets one random byte and changes the current value by `+10 hPa` or `-10 hPa`.
3. Clamps the value to the cluster range.
4. Toggles the application OK LED (green on the Nucleo mapping).
5. Updates `ZCL_PRESS_MEAS_ATTR_MEAS_VAL` in the local server cluster.

The pressure update and the reporting transmission are independent. If pressure updates are stopped, the reporting engine may continue to transmit the last attribute value.


### __Reporting Exchange with the Coordinator__

The coordinator allocates the corresponding Pressure Measurement client cluster on endpoint `17`. After its application startup, it waits __20 seconds__ and sends a ZCL Configure Reporting request to the router's short address.

The request configures one normal-direction record for `ZCL_PRESS_MEAS_ATTR_MEAS_VAL`:

- Data type: signed 16-bit integer.
- Minimum interval: __30 seconds__.
- Maximum interval: __30 seconds__.
- Reportable change: `0`.

The router acknowledges this request and reports using the received configuration. The coordinator report callback validates the payload and endpoint, identifies the Measurement Value attribute, and processes the received pressure value.

```text
Coordinator / Pressure Measurement Client              Router / Pressure Measurement Server
-----------------------------------------              -------------------------------------
Forms the centralized network
            <------------------------------------------  Joins or rejoins the network
Waits 20 seconds after application startup
Configure Reporting (Measurement Value, 5 s)  ------->  Stores reporting configuration
            <------------------------------------------  Configure Reporting response
            <------------------------------------------  Measurement Value report every 5 s
Parses and logs the received value                       Updates its local value every 30 s
```


### __Persistence and Recovery__

The project is compiled with `APPLICATION_USE_PERSISTENCE`. At boot, the application asks the persistence layer to restore saved Zigbee state and enables notifications so changed network state can be saved.

With compatible retained state on both boards, a normal reset or power cycle lets the router rejoin the same network automatically. The coordinator also uses persistence; after both nodes reconnect, the coordinator configures reporting and reports resume.

__SW3__ erases all persistence data, waits one second for completion, and resets the MCU. The router then behaves as an uncommissioned device and must join a network again.


### __User Controls and LED Indications__

| Control or indication | Router/server behavior |
|---|---|
| __SW1, short press__ | Starts a join/rejoin attempt when network startup is not complete. |
| __SW1, long press__ | Sends a Zigbee leave request. |
| __SW2__ | Toggles the periodic pressure-update timer. |
| __SW3__ | Erases persisted Zigbee data, waits one second, and resets the MCU. |
| __Blue LED__ | Join-status indication on the Nucleo mapping; it blinks during joining/rejoining. |
| __Green LED__ | Toggles when the pressure-update task runs. |
| __Red, green, and blue LEDs toggling__ | Indicates an application error condition. |

On the coordinator, __SW2__ opens a join window. This is useful when the router persistence has been erased and the router must commission again.


### __Recommended Operating Sequence__

1. Initially erase flash on all boards, then program the two applications.
2. Start the coordinator (`Zigbee_PressMeas_Client_Coord_Persist`) first and wait for it to become ready.
3. Start the router/server. If it has no valid persistent state, short-press __SW1__ to start joining; otherwise it automatically attempts rejoin.
4. Wait for the router to complete startup. It begins updating its local pressure value every 30 seconds.
5. After the coordinator 20-second delay, observe the Configure Reporting exchange and then reports every 30 seconds in UART traces or with a Zigbee sniffer.
6. Use __SW2__ on the router to stop or restart pressure changes. The existing reporting configuration remains active.
7. Use __SW3__ to remove saved Zigbee state. For a clean new demonstration, erase persistence on all participating devices and recommission them.


### __Hardware and Software environment__

This example targets STM32WBA55xx devices and has been tested with an STMicroelectronics STM32WBA55CGA Nucleo board. The STM32CubeIDE project uses the STM32WBA Zigbee stack, Zigbee PRO FFD library, and Zigbee cluster library.

For UART traces through the ST-LINK Virtual COM Port, use:

| Setting | Value |
|---|---|
| Baud rate | 115200 baud |
| Data bits | 8 |
| Stop bits | 1 |
| Parity | None |
| Flow control | None |
| Line ending | LF |


## __Note__

- A router whose persistence was erased cannot recover the old network by itself. The coordinator must be running and accepting joins; use its __SW2__ to open a join window when necessary.
- Erasing persistence on only one node can leave the coordinator and router with incompatible network state. Erase persistence on all participating devices for a clean recommissioning sequence.
- By default, this application runs with Zigbee PRO stack R23.
  If you want to run this application using Zigbee PRO stack R22, you should replace ZigBeeProR23_FFD.a by ZigBeeProR22_FFD.a and ZigBeeProR23_RFD.a by ZigBeeProR22_RFD.a and ZigBeeClusters.a by ZigBeeClustersR22.a in the build environment.
  Also, set in the project setup compilation flag CONFIG_ZB_REV=22.
