## __Zigbee_PressMeas_Client_Coord_Persist Application Description__


This application turns an STM32WBA55xx board into a Zigbee __coordinator__ for a centralized network. It implements the ZCL __Pressure Measurement client__ cluster and is intended to operate with one or more boards running `Zigbee_PressMeas_Server_Router_Persist`, which implement the corresponding Pressure Measurement server cluster.

The coordinator forms or restores the Zigbee network, authorizes joining on request, detects router joins, configures pressure reporting on the joining router, and logs the received pressure values. Zigbee state is stored in non-volatile flash so the coordinator can restore its network after a reset or power cycle.


### __Keywords__

Zigbee, Zigbee PRO, centralized network, coordinator, router, ZCL, Pressure Measurement cluster, attribute reporting, persistence, STM32WBA55xx, NUCLEO-WBA55xx.


### __Hardware Requirements__

- One STM32WBA55xx Nucleo board programmed with `Zigbee_PressMeas_Client_Coord_Persist`.
- One or more STM32WBA55xx boards programmed with `Zigbee_PressMeas_Server_Router_Persist`.
- A USB connection for programming and, optionally, an ST-LINK Virtual COM Port connection to view application traces.


### __Application Roles and Compatibility__

| Application | Zigbee role | ZCL role | Responsibility |
|---|---|---|---|
| `Zigbee_PressMeas_Client_Coord_Persist` | Coordinator | Pressure Measurement client | Forms the centralized network, opens the joining window, detects joining devices, configures reporting, and receives pressure reports. |
| `Zigbee_PressMeas_Server_Router_Persist` | Router | Pressure Measurement server | Joins or rejoins the network, extends the mesh, maintains the pressure attribute, and sends reports according to the configuration received from the coordinator. |

Both applications use the Home Automation profile, endpoint `17`, the Pressure Sensor device identifier, group address `0x0001`, the Home Automation preconfigured link key, channel `14`, and a default transmit power of `0 dBm`.


### __Coordinator Code Architecture__

The coordinator-specific behavior is implemented mainly in the following files:

- `STM32_WPAN/App/app_zigbee.c`: Zigbee-stack initialization, network formation, persistence recovery, join/leave notifications, permit-join requests, and startup retry handling.
- `STM32_WPAN/App/app_zigbee_endpoint.c`: endpoint and client-cluster creation, device tracking, reporting configuration, report parsing, and button actions.
- `STM32_WPAN/App/app_zigbee_persistence_nvm.c`: flash-based Zigbee persistence, including integrity checks and recovery of the latest valid saved state.
- `STM32_WPAN/App/app_zigbee_utility.c`: reusable ZDO and binding helper functions.

At startup, `APP_ZIGBEE_ConfigEndpoints()` creates endpoint `17` and allocates a `ZbZclPressMeasClient` cluster. Its report callback checks the received attribute encoding and destination endpoint before decoding and logging the signed 16-bit Measurement Value.


### __Network Formation and Recovery__

The application starts with `ZbStartTypeForm`, therefore it is the coordinator that creates the centralized Zigbee network.

1. The Zigbee stack and Home Automation endpoint are initialized.
2. The persistence-rejoin task first searches flash for the latest valid saved Zigbee state.
3. If valid state is found, `ZbStartupPersist()` restores the coordinator state and resumes the previous network.
4. If no valid state is available, the coordinator starts network formation on channel `14`.
5. Startup failures are retried after a 500 ms delay. After a successful startup, the join-status LED is turned on.
6. The coordinator registers network join and leave filters so that router activity can be handled by the application.

The coordinator can accept standard devices using the Home Automation preconfigured link key. The UART command path also supports an install code in the form `IC:<ExtendedAddress>:<InstallCodeWithCRC>`; when it is accepted, a device-specific trust-center link key is added and a 30-second join window is opened.


### __Pressure Reporting Exchange__

When a router joins, the coordinator receives a Zigbee join indication, saves the router short address, and starts a one-shot 20-second timer. When the timer expires, the coordinator sends a ZCL Configure Reporting request to that router on endpoint `17` for `ZCL_PRESS_MEAS_ATTR_MEAS_VAL`.

The request contains one normal-direction reporting record:

- Data type: signed 16-bit integer.
- Minimum interval: __30 seconds__.
- Maximum interval: __30 seconds__.
- Reportable change: `0`.

The router responds to the configuration request and subsequently reports its Measurement Value. The client callback validates the parsed data length and destination endpoint, decodes the value as an `int16_t`, logs the source extended address and pressure in hPa, and toggles the OK LED.

```text
Coordinator / Pressure Measurement Client              Router / Pressure Measurement Server
-----------------------------------------              -------------------------------------
Forms or restores the centralized network
            <------------------------------------------  Joins or rejoins the network
Detects a new router and waits 20 seconds
Configure Reporting (Measurement Value, 30 s) ------->  Stores reporting configuration
            <------------------------------------------  Configure Reporting response
            <------------------------------------------  Measurement Value reports
Validates, logs, and indicates received values          Updates its local pressure value every 30 s
```

The current application retains only one router short address for automatic reporting configuration. If several routers join, the most recently detected joining device becomes the target of the next Configure Reporting request. A rejoin indication is logged but does not itself start a new reporting-configuration timer.


## __Persistence and Reset Behavior__

Persistence is enabled for the coordinator. When the Zigbee stack signals a state change, the application obtains the stack persistence data and appends it to a 24 KiB flash area at the end of program flash. Each record includes a sequence counter, data size, and CRC values. At boot, the newest valid record is located and passed to the stack for restoration.

With compatible retained data on the coordinator and router, both devices can recover their previous network membership after a reset or power cycle. The router's reporting configuration is also part of its Zigbee state; after recovery, reporting can continue using the retained configuration.

Pressing __SW3__ erases the complete local persistence area, waits one second, and resets the MCU. The coordinator then creates a new network on the next startup. Devices that still retain the old network state must be recommissioned, or their persistence must also be erased for a clean demonstration.


### __User Controls and LED Indications__

| Control or indication | Coordinator/client behavior |
|---|---|
| __SW1__ | No coordinator-specific application action is defined. |
| __SW2__ | Opens a network permit-join window for 60 seconds, but only after the coordinator has formed or restored a network. |
| __SW3__ | Erases local Zigbee persistence, waits one second, and resets the MCU. |
| __Blue LED__ | Join-status indication; it is switched on after successful coordinator startup. |
| __Green LED__ | Toggles when a valid Pressure Measurement report is processed. |
| __Blue and green LEDs alternating__ | Indicates a fatal application error. |

On the paired router/server, __SW1__ starts joining after a fresh reset (short press) or requests a leave (long press), __SW2__ starts or stops pressure updates, and __SW3__ erases its persisted state.


### __Recommended Operating Sequence__

1. For an initial demonstration, erase flash or use __SW3__ on all boards to remove old Zigbee state.
2. Program one board with `Zigbee_PressMeas_Client_Coord_Persist` and one or more boards with `Zigbee_PressMeas_Server_Router_Persist`.
3. Start the coordinator and wait until it has formed the network.
4. Press __SW2__ on the coordinator to open its 60-second joining window.
5. Start a router/server. If it has no valid persistent state, short-press its __SW1__ to begin joining.
6. After the coordinator detects the router, wait 20 seconds for its Configure Reporting request.
7. Observe the successful configuration response and the pressure reports in the UART traces. The server updates its simulated or physical pressure value every 30 seconds by default.
8. To repeat commissioning from a known state, erase persistence on every participating device before restarting the sequence.


### __Hardware and Software environment__

This application targets STM32WBA55xx devices and is configured for an STM32WBA55CGA Nucleo board. The STM32CubeIDE project uses STM32Cube FW_WBA V1.10.0, the STM32WBA Zigbee stack, the Zigbee PRO FFD library, and the Zigbee cluster library.

The default project configuration enables application logging through USART1. For traces through the ST-LINK Virtual COM Port, use:

| Setting | Value |
|---|---|
| Baud rate | 115200 baud |
| Data bits | 8 |
| Stop bits | 1 |
| Parity | None |
| Flow control | None |
| Line ending | LF |


### __Note__

- The coordinator opens its joining window only when __SW2__ is pressed and it is already on a Zigbee network. Start the coordinator before commissioning a router whose persistence was erased.
- Erasing persistence on one device only can leave the coordinator and routers with incompatible network state. Erase persistence on all participating devices when a clean recommissioning sequence is required.
- The coordinator configures reporting after a **new join**. If a router only rejoins and does not retain a valid reporting configuration, force a new join or add an application-specific reconfiguration mechanism.
- By default, this application runs with Zigbee PRO stack R23.
  If you want to run this application using Zigbee PRO stack R22, you should replace ZigBeeProR23_FFD.a by ZigBeeProR22_FFD.a and ZigBeeProR23_RFD.a by ZigBeeProR22_RFD.a and ZigBeeClusters.a by ZigBeeClustersR22.a in the build environment.
  Also, set in the project setup compilation flag CONFIG_ZB_REV=22.

