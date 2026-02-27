## __Zigbee_PressMeas_Server_Router_Persist Application Description__

How to use the Pressure Measure cluster on a device acting as a Server within a Centralized Zigbee network.  
    
The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the Pressure Measure cluster. 
Once the Zigbee mesh network is created, the Server sends regularly the Pressure to Client. The user can stop or restart Pressure evolution on Server through push buttons SW1 and SW2.

With the Zigbee Persistence, the user can stop and restart each Device (Coord or Router). After the restart, the Device reconnect to the ZB network, and the reports continue.
The ZB Persistence is erased by press push button SW3.

N.B. : 
 * It's recommended to complete erase the Flash before download the application firmware on each Device.
 * After a restart of PressMeas Server (Router), reports are send every 60 seconds.
 * After an Erase of ZB Persistence on the Device, it's preferable to erase the other Device also (else there are not aligned).


### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, Pressure Measure cluster, Persistence


### __Hardware Requirements__

For this application it is requested to have:  

* One STM32WBA55xx Nucleo board loaded with application : **Zigbee_PressMeas_Client_Coord_Persist**  
* One or more STM32WBA55xx board loaded with application : **Zigbee_PressMeas_Server_Router_Persist**

### __Demo use case__

* This demo shows how to use Pressure Measure cluster:
  * The Pressure Measure Server update regularly (500 ms) its **ZCL_PRESS_MEAS_ATTR_MEAS_VAL** attributes (via the measure of a sensor or by random).  

  * The Pressure Measure Client send a Report Request to obtains regularly the **ZCL_PRESS_MEAS_ATTR_MEAS_VAL** attributes.  
 
  * The Pressure Measure Server send regularly (60 seconds) a Report with the latest value of **ZCL_PRESS_MEAS_ATTR_MEAS_VAL** attributes.  
 
 
<pre>
    
               Coord.                                                                       Router
             +-----------+                                                               +-----------+
             |           |                                                               |           |                                       
             | PressMeas |                                                               | PressMeas |
             [ Client    |                                                               | Server    |  - Pressure Server during Init 
             | after 15s |                                                               | after 15s |    launch a 500 ms Periodic Timer
             |  Blue Led |                                                               |  Blue Led |  
             |           |                                                               | after n*s |  - Every 500 ms (Green Led toggling)
             |           |                                                               | Green Led |    * Read the Pressure Sensor (if exist)
             |           |                                                               | toggling  |      or simulate it with RNG.
             |           |                                                               |           |    * <= ZbZclAttrIntegerWrite(ZCL_PRESS_MEAS_ATTR_MEAS_VAL) 
             |           |  After a Router was connected :                               |           |
             |           |  ZbZclAttrReportConfigReq(ZCL_PRESS_MEAS_ATTR_MEAS_VAL, 5sec) |           |
             |           | ------------------------------------------------------------> |           |
             |           | <------------------------------------------------------------ |           |
             |           |                                                               |           |
             |           | <-------------- Report (every 5 seconds) -------------------- |           |
             |           |                                                               |           |
             |           |                                                               |           | <= PushB SW2 : Stop 500 ms Periodic Timer.(Stop Green Led toggling but does not stop reporting)
 PushB SW2=> |           | Open a new window of commissioning                            |           |           
             |           |  (Most of the time, without visible action                    |           | <= PushB SW1 : Restart 500 ms Periodic Timer.(Green Led toggling)
             |           |  since network already set)                                   |           |
             |           |                                                               |           |
 PushB SW3=> | Delete    |                                                               |           |
             |  persist  |                                                   PushB SW3=> | Delete    |
             |    and    |                                                               |  persist  | => attempt for a new commissioning after delete persist
             | (Blue off)|                                                               | (Blue off)|    and restart of the application.
             |  restart  |                                                               |   and     |     If the window of commissioning is closed then
             | after 15s |                                                               |  restart  |      retry again and again the commissioning process.
             |  Blue Led |                                                               |   and     |     If the window of commissioning is open then
 PushB SW2=> |           | Open a new window of commissioning                            | after 15s |      a new commissioning will succeed. 
             |           |                                                               |  Blue Led |
             |           |                                                               |           |
             |           |  After a Router was connected :                               | after n*s |
             |           |  ZbZclAttrReportConfigReq(ZCL_PRESS_MEAS_ATTR_MEAS_VAL, 5sec) | Green Led |
             |           | ------------------------------------------------------------> | toggling  |
             |           | <------------------------------------------------------------ |           |
             |           |                                                               |           |
             |           | <--- Report (every 1 minute - see limitation) --------------- |           |
             |           |                                                               |           |
             +-----------+                                                               +-----------+
  

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.

* To run the application :
  1. Start the first board. It must be the coordinator of the Zigbee network, so in this demo application it is the device running Zigbee_PressMeas_Client_Coord_Persist application.  
    
  2. Wait for the Blue LED ON.  

  3. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator.
 
  Do the same for the other boards if applicable.    
  
  --> At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons. 
  
  4. It is now possible to see the green led toggling on the Server side (periodicity of 0.5s) and periodic tx/rx frames on wireshark or on Uart log every 5 seconds.

  5. Retention on the Coordinator can be observed with a sequence of Zigbee_PressMeas_Client_Coord_Persist.PowerOff followed by Zigbee_PressMeas_Client_Coord_Persist.PowerOn followed by a re-connection to the Zigbee_PressMeas_Server_Router_Persist and periodic tx/rx frames on wireshark or on Uart log every 1 minute.

**Note:** When LED Red, Green and Blue are toggling it is indicating an error has occurred on application.

### __Hardware and Software environment__

* This example runs on STM32WBA55xx devices.  

* This example has been tested with an STMicroelectronics STM32WBA55CGA_Nucleo board and can be easily tailored to any other supported device and development board.  

* On STM32WBA55CGA_Nucleo, the jumpers must be configured as described in this section. Starting from the top left position up to the bottom right position, the jumpers on the Board must be set as follows:
<br>    
**JP1:**</br>
1-2:  [ON]</br>
3-4:  [OFF]</br>
5-6:  [OFF]</br>
7-8:  [OFF]</br>
9-10: [OFF]</br>
<br>
**JP2:**</br>
1-2:  [ON]  

### __Traces__

* To get the traces you need to connect your Board to the Hyperterminal (through the STLink Virtual COM Port).  

* The UART must be configured as follows:  
<br>
BaudRate       = 115200 baud</br>
Word Length    = 8 Bits</br>
Stop Bit       = 1 bit</br>
Parity         = none</br>
Flow control   = none</br>
Terminal   "Go to the Line" : <LF>

