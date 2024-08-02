## __Zigbee_OccupSensing_Server_Router Application Description__

How to use the Occupancy Sensing on a device acting as a Server within a Centralized Zigbee network.
    
The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the Occupancy Sensing cluster. 
Once the Zigbee mesh network is created, the Server sends regularly the Occupancy to Client. The user can indicate if the Room is empty or not on Server by using push buttons SW1 and SW2.

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, Occupancy Sensing cluster  
     
### __Hardware Requirements__

For this application it is requested to have:  

* One STM32WBA55xx Nucleo board loaded with application : **Zigbee_OccupSensing_Client_Coord**  
* One or more STM32WBA55xx board loaded with application : **Zigbee_OccupSensing_Server_Router**  

### __Demo use case__

* This demo shows how to use Occupancy Sensing cluster:
	* The Occupancy Sensing Server updates regularly (500 ms) its **ZCL_OCC_ATTR_OCCUPANCY** attributes (via the measure of a sensor or by random).  
	
    * The Occupancy Sensing Client sends a Report Request to obtain regularly the **ZCL_OCC_ATTR_OCCUPANCY** attributes.  
	
	* The Occupancy Sensing Server sends regularly (5 s) a Report with the latest value of **ZCL_OCC_ATTR_OCCUPANCY** attributes.  
	  
* 1 read-only attributes is used :
    * Room Occupancy Value (16-bit integer)  
	
<pre>
    
 
               Coord.                                                                       Router
             +-----------+                                                               +-----------+
             |           |                                                               |           |                                       
             | OccupSens |                                                               | OccupSens |
             [ Client    |                                                               | Server    |  - Occupancy Sensing Server during Init 
             |           |                                                               |           |    launch a 500 ms Periodic Timer
             |           |                                                               |           |  
             |           |                                                               |           |  - Every 500 ms
             |           |                                                               |           |    * Read the Occupancy Sensor (if exist)
             |           |                                                               |           |      or simulate it with RNG.
             |           |                                                               |           |    * <= ZbZclAttrIntegerWrite(ZCL_OCC_ATTR_OCCUPANCY) 
             |           |  After a Router was connected :                               |           |
             |           |  ZbZclAttrReportConfigReq(ZCL_OCC_ATTR_OCCUPANCY, 5sec)       |           |
             |           | ------------------------------------------------------------> |           |
             |           | <------------------------------------------------------------ |           |
             |           |                                                               |           |
             |           | <-------------- Report (every 5 seconds) -------------------- |           |
             |           |                                                               |           |             
             |           |                                                               |           | <= PushB SW1 : Indicate Room Occupancy.
             |           |                                                               |           |			          -> ZbZclAttrIntegerWrite(ZCL_OCC_ATTR_OCCUPANCY)
             |           |                                                               |           |			 
             |           |                                                               |           | <= PushB SW2 : Indicate Room Empty.		 
             |           |                                                               |           |			          -> ZbZclAttrIntegerWrite(ZCL_OCC_ATTR_OCCUPANCY)
             |           |                                                               |           |			 
             +-----------+                                                               +-----------+
  

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network, so in this demo application it is the device running Zigbee_OccupSensing_Client_Coord application.  
    
	2. Wait for the Blue LED ON.  
	
    3. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator.
	Do the same for the other boards if applicable.    
&rarr; At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons.     
	 
	4. It is now possible to change Room Occupancy by pressing SW1 or SW2 on Server. This change is reported to Client.  
			
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
