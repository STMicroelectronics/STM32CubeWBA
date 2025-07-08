## __Zigbee_PowerConfig_Client_Coord Application Description__

How to use the Power Config cluster on a device acting as a Coordinator within a Centralized Zigbee network.  
    
The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the Power Config cluster. 
Once the Zigbee mesh network is created, the Server sends regularly the Battery availability in Percent to Client. 

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, Power config cluster
     
### __Hardware Requirements__

For this application it is requested to have:  

* One STM32WBA65xx Nucleo board loaded with application : **Zigbee_PowerConfig_Client_Coord**  
* One or more STM32WBA65xx board loaded with application : **Zigbee_PowerConfig_Server_SED**  

### __Demo use case__

* This demo shows how to use Occupancy Sensing cluster:
	* The Power Config Server updates regularly (30 s) its **ZCL_POWER_CONFIG_ATTR_BATTERY_PCT** attributes (via the measure of a sensor or by random).  
	
    * The Power Config Client sends a Report Request to obtain regularly the **ZCL_POWER_CONFIG_ATTR_BATTERY_PCT** attributes.  
	
	* The Power Config Server sends regularly (60 s) a Report with the latest value of **ZCL_POWER_CONFIG_ATTR_BATTERY_PCT** attributes.  
	  
* 1 read-only attributes is used :
    * Battery Percent Value (8-bit unsigned integer)  
	
<pre>
    
 
               Coord.                                                                             SED
           +-------------+                                                                    +-------------+
           |             |                                                                    |             |                                       
           | PowerConfig |                                                                    | PowerConfig |
           |   Client    |                                                                    |   Server    |  - PowerConfig Server during Init 
           |             |                                                                    |             |    launch a 30 s Periodic Timer
           |             |                                                                    |             |  
           |             |                                                                    |             |  - Every 30 s
           |             |                                                                    |             |    * Read the battery (if exist)
           |             |                                                                    |             |      or simulate it with RNG.
           |             |                                                                    |             |    * <= ZbZclAttrIntegerWrite(ZCL_POWER_CONFIG_ATTR_BATTERY_PCT) 
           |             |  After a Router was connected :                                    |             |
           |             |  ZbZclAttrReportConfigReq(ZCL_POWER_CONFIG_ATTR_BATTERY_PCT, 60sec)|             |    Green LED toggling (if CFG_FULL_LOW_POWER=2)
           |             | -----------------------------------------------------------------> |             |
           |             | <----------------------------------------------------------------- |             |
           |             |                                                                    |             |
           |             | <-------------- Report (every 60 seconds) -------------------------|             |
           |             |                                                                    |             | <= PushB SW1 : Start/Restart 30 s Periodic Timer.		          
           |             |                                                                    |             | 		 
           |             |                                                                    |             | <= PushB SW2 : Stop 30 s Periodic Timer.			 
           |             |                                                                    |             | 	 
           |             |                                                                    |             |			 
           +-------------+                                                                    +-------------+
  

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA65xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network, so in this demo application it is the device running Zigbee_PowerConfig_Client_Coord application.  
    
	2. Wait for the Blue LED ON.  
	
    3. Start the second board. This board is configured as Zigbee SED and will be attached to the network created by the coordinator.
	Do the same for the other boards if applicable.    
&rarr; At this stage, the Zigbee network is automatically created, and traces on coordinator indicate that new device is added.       
if you want to have the Led Blue on on SED side when attached to the network, you can modify the **CFG_FULL_LOW_POWER** to the value **2**.  	 
	4. On Client side, every 60 s, the battery voltage is reported from the Server. 
	To stop battery voltage evolution on Server, push SW2 push button, and to restart it push SW1.
			
**Note:** When LED Red, Green and Blue are toggling it is indicating an error has occurred on application.

### __Hardware and Software environment__

* This example runs on STM32WBA65xx devices.  

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
