## __Zigbee_DoorLock_Server_Coord Application Description__

How to use the DoorLock cluster  on a device acting as a Server within a Centralized Zigbee network.
    
The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the DoorLock cluster. Once the Zigbee mesh network is created, the user can send requests from the Client to Server (through push buttons SW1, SW2 and SW3), and Server to Client (push buttons SW1).  

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, DoorLock cluster 

### __Demo use case__  

This demo shows how to use DoorLock cluster.  

* The DoorLock Server set up its initial configuration:
	* The lock state is locked.
	* The DoorLock normal and passage mode are supported.
	* The operating mode is set to normal.
	* The Auto Relock Time is set to 5 seconds.
	* The DoorLock alarm supports <Forced Door Open under Door Locked Condition> event.

* When the DoorLock Server lock state is locked, the **Green LED** is ON.
  The DoorLock Server can enable passage mode by pressing SW2 (passage mode activated for 10 seconds).
  When passage mode is activated on server, the Red LED is ON.

* The DoorLock Client can remotely: 
   * Create a new user (with PIN code).
   * Lock/Unlock the door (including unlock requests with a wrong PIN).

*  When the DoorLock client receives a DoorLock alarm notification, the Red LED is ON during 500ms.
    
**Note:** When the DoorLock Server is in passage mode, the PIN code is not checked and the lock remains open after an unlock.  
  
### __Hardware Requirements__

For this application it is requested to have:  

* One STM32WBA55xx Nucleo board loaded with application : **Zigbee_DoorLock_Server_Coord**  
* One or more STM32WBA55xx board loaded with application : **Zigbee_DoorLock_Client_Router**  

<pre>
    

                Router                                                                      Coord.
              +----------+                                                                +----------+
              |          |                                                                |          |
              | DoorLock |                                                                | DoorLock |
              | Client   |                                                                | Server   |  - DoorLock Server during Init 
              |          |                                                                |          |    <= ZbZclAttrIntegerWrite(ZCL_DOORLOCK_ATTR_LOCKSTATE)
              |          |                                                                |          |    <= ZbZclAttrIntegerWrite(ZCL_DOORLOCK_ATTR_DOORSTATE)
              |          |                                                                |          |	 <= ZbZclAttrIntegerWrite(ZCL_DOORLOCK_ATTR_SUPPORTED_MODES)
              |          |                                                                |          |    <= ZbZclAttrIntegerWrite(ZCL_DOORLOCK_ATTR_MODE)
              |          |                                                                |          |    <= ZbZclAttrIntegerWrite(ZCL_DOORLOCK_ATTR_AUTO_RELOCK)
              |          |                                                                |          |			 
              |          |                                                                |         || => GREEN LED ON
              |          |                                                                |  Door   ||
              |          |                                                                |  Lock   ||
              |          |    /* Door Lock client/server normal mode interaction */       |         ||			 
              |          |                                                                |         ||
              |          |            ZbZclDoorLockClientUnlockReq(PIN)                   |         ||
  PushB SW1=> |          | -------------------------------------------------------------> |         || => GREEN LED OFF
              |          | <------------------------------------------------------------- |          |
              |          |                                                                |  Door    |
              |          |                                                                |  UnLock  |
              |          |                                                                |          |	/* When Auto Relock Time is reached */
              |          |                                                                |          | <= ZbZclAttrIntegerWrite(ZCL_DOORLOCK_ATTR_LOCKSTATE)
              |          |                                                                |         || => GREEN LED ON
              |          |                                                                |  Door   ||
              |          |                                                                |  Lock   ||
              |          |            ZbZclDoorLockClientUnlockReq(PIN)                   |         ||
  PushB SW1=> |          | -------------------------------------------------------------> |         || => GREEN LED OFF
              |          | <------------------------------------------------------------- |          |
              |          |                                                                |  Door    |
              |          |                                                                |  UnLock  |
              |          |                                                                |          |
              |          |            ZbZclDoorLockClientLockReq(PIN)                     |          |
  PushB SW2=> |          | -------------------------------------------------------------> |         || => GREEN LED ON
              |          | <------------------------------------------------------------- |  Door   ||
              |          |                                                                |  Lock   ||
              |          |                                                                |         ||
              |          |            ZbZclDoorLockClientUnlockReq(WRONG_PIN)             |         ||
  PushB SW3=> |          | -------------------------------------------------------------> |         || => GREEN LED remains ON
              |          | <------------------------------------------------------------- |         ||
              |          |                                                                |         ||
              |          |                                                                |         ||
              |          |                                                                |         ||			 
              |          |    /* Door Lock client/server passage interaction */           |         ||
              |          |                                                                |         ||
              |          |            ZbZclDoorLockClientUnlockReq(PIN)                   |         ||
  PushB SW1=> |          | -------------------------------------------------------------> |         || => GREEN LED OFF
              |          | <------------------------------------------------------------- |          |
              |          |                                                                |  Door    |
              |          |                                                                |  UnLock  |			 
              |          |                                                                |          |			 
              |          |            ZbZclDoorLockClientLockReq(PIN)                     |          |
  PushB SW2=> |          | -------------------------------------------------------------> |         || => GREEN LED ON
              |          | <------------------------------------------------------------- |         ||
              |          |                                                                |  Door   ||
              |          |                                                                |  Lock   ||
              |          |                                                                |         ||			 
              |          |            ZbZclDoorLockClientUnlockReq(WRONG_PIN)             |         ||
  PushB SW3=> |          | -------------------------------------------------------------> |         ||
              |          | <------------------------------------------------------------- |         ||
              |          |                                                                |         ||
              |          |                                                                |         ||
              |          |    /* Forced Door Open under Door Locked Condition alarm */    |         ||  
              |          |                                                                |         || <= PushB SW1 : 
              |          |                                                                |         || <= ZbZclAttrIntegerWrite(ZCL_DOORLOCK_ATTR_DOORSTATE)
              |          |                                                                |         ||
              |          |                       ZbZclClusterSendAlarm                    |         ||
Red LED ON <= |          | <------------------------------------------------------------- |         ||
              |          |                                                                |         ||
              |          |                                                                |         ||
              |          |                                                                |         || <= PushB SW2 : Passage Mode during 10 sec.
              |          |                                                                |         ||
              |          |            ZbZclDoorLockClientUnlockReq(WRONG_PIN)             |         ||
  PushB SW3=> |          | -------------------------------------------------------------> |         || => GREEN LED OFF 
              |          | <------------------------------------------------------------- |          | (It is possible to unlock the door, even if the
              |          |                                                                |  Door    |  pin is wrong because of the 'passage' mode)
              |          |                                                                |  UnLock  |
              |          |                                                                |          |
              |          |                                                                |          | => Expiration of Passage mode after 10 sec.
              |          |                                                                |          | 
              |          |                                                                |          | 
              |          |            ZbZclDoorLockClientLockReq(PIN)                     |          |
  PushB SW2=> |          | -------------------------------------------------------------> |         || => GREEN LED ON 
              |          | <------------------------------------------------------------- |  Door   ||
              |          |                                                                |  Lock   ||
              |          |            ZbZclDoorLockClientUnlockReq(WRONG_PIN)             |         ||
  PushB SW3=> |          | -------------------------------------------------------------> |         || => GREEN LED remains ON
              |          | <------------------------------------------------------------- |         ||
              |          |                                                                |         || 
              +----------+                                                                +----------+
  
  
</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network so in this demo application it is the device running Zigbee_DoorLock_Server_Coord application.  

	2. Wait for the Blue LED ON.  

	3. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator.
Do the same for the other boards if applicable.  
&rarr; At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons.  

	4. It is now possible to remotely read DoorLock server attributes on the client by pressing on the SW1/SW2 push button.
   
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
Terminal   "Go to the Line" : &lt;LF&gt;  

