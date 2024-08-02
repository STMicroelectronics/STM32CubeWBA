## __Zigbee_PressMeas_Server_Router Application Description__

How to use the Pressure Measure cluster on a device acting as a Server within a Centralized Zigbee network.  
    
The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the Pressure Measure cluster. 
Once the Zigbee mesh network is created, the Server sends regularly the Pressure to Client. The user can stop or restart Pressure evolution on Server through push buttons SW1 and SW2.    

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, Pressure Measure cluster

### __Hardware Requirements__

For this application it is requested to have:  

* One STM32WBA55xx Nucleo board loaded with application : **Zigbee_PressMeas_Client_Coord**  
* One or more STM32WBA55xx board loaded with application : **Zigbee_PressMeas_Server_Router**

### __Demo use case__

* This demo shows how to use Pressure Measure cluster:
	* The Pressure Measure Server update regularly (500 ms) its **ZCL_PRESS_MEAS_ATTR_MEAS_VAL** attributes (via the measure of a sensor or by random).  
	
    * The Pressure Measure Client send a Report Request to obtains regularly the **ZCL_PRESS_MEAS_ATTR_MEAS_VAL** attributes.  
	
	* The Pressure Measure Server send regularly (500 ms) a Report with the latest value of **ZCL_PRESS_MEAS_ATTR_MEAS_VAL** attributes.   
	  
* 1 read-only attributes is used :
    * Pressure Value (16-bit integer)  
	
	
<pre>
    
	

               Coord.                                                                       Router
             +-----------+                                                               +-----------+
             |           |                                                               |           |                                       
             | PressMeas |                                                               | PressMeas |
             [ Client    |                                                               | Server    |  - Pressure Server during Init 
             |           |                                                               |           |    launch a 500 ms Periodic Timer
             |           |                                                               |           |  
             |           |                                                               |           |  - Every 500 ms (Green Led toggling)
             |           |                                                               |           |    * Read the Pressure Sensor (if exist)
             |           |                                                               |           |      or simulate it with RNG.
             |           |                                                               |           |    * <= ZbZclAttrIntegerWrite(ZCL_PRESS_MEAS_ATTR_MEAS_VAL) 
             |           |  After a Router was connected :                               |           |
             |           |  ZbZclAttrReportConfigReq(ZCL_PRESS_MEAS_ATTR_MEAS_VAL, 5sec) |           |
             |           | ------------------------------------------------------------> |           |
             |           | <------------------------------------------------------------ |           |
             |           |                                                               |           |
             |           | <-------------- Report (every 5 seconds) -------------------- |           |
             |           |                                                               |           |             
             |           |                                                               |           | <= PushB SW1 : Start/Restart 500 ms Periodic Timer. (Green Led toggling)		 
             |           |                                                               |           |			 
             |           |                                                               |           | <= PushB SW2 : Stop 500 ms Periodic Timer. (Stop Green Led toggling)		 
             |           |                                                               |           |			 
             +-----------+                                                               +-----------+
  

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network, so in this demo application it is the device running Zigbee_PressMeas_Client_Coord application.  
    
	2. Wait for the Blue LED ON.  
	
    3. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator.
	Do the same for the other boards if applicable.    
&rarr;  At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons.     
	 
	4. It is now possible to remotely read Meter Identification server attributes on the client by pressing on the SW1/SW2 push buttons.
		
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
