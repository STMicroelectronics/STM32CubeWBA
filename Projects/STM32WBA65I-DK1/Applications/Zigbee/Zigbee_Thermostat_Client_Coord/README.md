## __Zigbee_Thermostat_Client_Coord Application Description__

How to use the Thermostat cluster on a device acting as a Server within a Centralized Zigbee network.  
    
The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the Thermostat cluster. 
Once the Zigbee mesh network is created, the Server sends regularly the Temperature to Client. The user can manipulate a Menu through push buttons SW1, SW2 and  SW3. 

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, Thermostat cluster  

### __Hardware Requirements__

For this application it is requested to have:  

* One STM32WBA65xx Nucleo board loaded with application : **Zigbee_Thermostat_Client_Coord**  
* One or more STM32WBA65xx board loaded with application : **Zigbee_Thermostat_Server_Router**  

### __Demo use case__

* This demo shows how to use Thermostat cluster:
	* The Thermostat Server update regularly (500 ms) its **ZCL_THERM_SVR_ATTR_LOCAL_TEMP** attributes (via the measure of a sensor or by random).  
	
    * The Thermostat Client send a Report Request to obtains regularly the **ZCL_THERM_SVR_ATTR_LOCAL_TEMP** attributes.  
	
	* The Thermostat Server send regularly (5 s) a Report with the latest value of **ZCL_THERM_SVR_ATTR_LOCAL_TEMP** attributes.  
	  
* 1 read-only attributes is used :
    * Temperature Value (16-bit integer)  
	
<pre>
    
    

               Coord.                                                                    Router
           +------------+                                                                +------------+
           |            |                                                                |            |                                       
           | Thermostat |                                                                | Thermostat |
           [   Client   |                                                                | Server     |  - Temperature Server during Init 
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
  

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA65xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network, so in this demo application it is the device running Zigbee_Thermostat_Client_Coord application.  
    
	2. Wait for the Blue LED ON.  
	
    3. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator.
	Do the same for the other boards if applicable.    
&rarr;  At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons.     
	 
	4. It is now possible to change the configuration of Thermostat by pressing on the SW1/SW2/SW3 push buttons.  
	
	5. The following Menu is used to change Thermostat configuration:  
		* Main Menu is composed by:  
			* **Menu Cooling** : display & manage Cooling Temp(ZCL_THERM_SVR_ATTR_OCCUP_COOL_SETPOINT). Default is *26° C*.
			* **Menu Heating** : display & manage Heating Temp(ZCL_THERM_SVR_ATTR_OCCUP_HEAT_SETPOINT). Default is *21° C*.
			* **Menu Control** : display & manage Control (ZCL_THERM_SVR_ATTR_CONTROL_SEQ_OPER). Default is *Cooling and Heating*.
			* **Menu SysMode** : display & manage System Mode (ZCL_THERM_SVR_ATTR_SYSTEM_MODE). Default is *Auto*.
			* **Exit**         : Exit of Menu.
		* Sub-Menu **Control** is composed by :
			* Cooling only
			* Cooling + Reheat
            * Heating only,
            * Heating + Reheat
            * Cooling and Heating
            * Cooling/Heating+Reheat
          
        * Sub-Menu **SysMode** is composed by :
            * OFF
            * Auto
            * Not Used
            * Cooling
            * Heating
            * Ext Heating
            * Pre-Cooling
            * Fan
            * Dry
	6. To navigate on Menu you need to :
		* Press SW1 to enter in Menu, enter in Sub-Menu or validate a value and exit menu.
		* Press SW3 to navigate down in the Menu or Sub-Menu or decrease a value.
		* Press SW2 to navigate up in the Menu or Sub-Menu or increase a value.
		* If a SWx is not pressed after 10 seconds, exit Menu.  
		N.B. : during the display of Menu, others reports are not displayed (Temperature update for example).  
		
<pre> 


                      |
                   SW1|
                      v	                                                                                                   	                                                                                         			  
           +---------------------+   SW2    +-------------------+   SW2       +-------------------+     SW2                 +-------------------+        SW2          +-----------+
           |  Trace msg: Cooling |<---------| Trace msg: Heating|<------------|Trace msg: Control |<------------------------| Trace msg: SysMode|<--------------------|   Exit    |
           |                     |--------->|                   |------------>|                   |------------------------>|                   |-------------------->|           |    
		   +---------------------+   SW3    +-------------------+    SW3      +-------------------+           SW3           +-------------------+        SW3          +-----------+
		              |                           SW1 |                               |                                                  |
		          SW1 |                               |                          SW1  |                                               SW1|
		              v                               v                      (Default)|    +-------------------+                (Default)|      +-----------------+ 
	+------------------------------+	      Same as Cooling menu                    |    |    Cooling Only   |   SW1                   |      |     OFF         |   SW1   
  	| Trace msg : Cooling temp = X |-----|                                            |    |                   |-------|                 |      |                 |-------|    
    |                              |     |                                            |    +-------------------+       |      	     	 |      +-----------------+       |     
	+------------------------------+     | SW3                                        |        |            ^          |                 |          |         ^           |     
                    |                    |                                            |        |SW3         |SW2       |                 |          |SW3      |SW2        | 
                 SW2|                    |                                            |        v            |          |                 |          v         |           | 
                    v                    v                                            |    +-------------------+       |                 |      +-----------------+       |  
		+-------------------+  SW3  +-------------------+                             |    | Cooling + Reheat  |   SW1 |                 |----->|     Auto        |   SW1 | 
	|-->| Action: X=X-0.5   |------>|  Action: X=X+0.5  |<---|                        |    |                   |-------|                        |                 |-------| 
	|	|                   |<------|                   |    |                        |    +-------------------+       |                        +-----------------+       |  
	|	+-------------------+  SW2  +-------------------+    |                        |        |            ^          |                            |         ^           | 
	| SW2  |   |                      |               |  SW3 |                        |        |SW3         |SW2       |                            |SW3      |SW2        | 
	|------|   |                      |               |------|                        |        v            |          |                            v         |           | 
		       | SW1                  | SW1                                           |    +-------------------+       |                        +-----------------+       |  
			   v				      v		            							  |    |   Heating Only    |   SW1 |                        |     Not Used    |   SW1 | 
       +-----------------------------------------+                                    |    |                   |-------|                        |                 |-------| 					    
	   | Trace msg: Exit from Menu with new value|                                    |    +-------------------+       |  				        +-----------------+       |  
	   | 										 |                                    |        |            ^     	   |  				            |        ^       	  | 
	   +-----------------------------------------+                                    |        |SW3         |SW2       |                            |SW3     |SW2         | 
	                                                                                  |        v            |          |                            v        |            | 
																					  |    +-------------------+       |                        +-----------------+       |  
																					  |    | Heating + Reheat  |  SW1  |                        |     Cooling     |  SW1  | 
																					  |    |                   |-------|                        |                 |-------| 
																					  |    +-------------------+       |                        +-----------------+       |     
																					  |        |            ^          |              and so on (see above description)   | 
						Timeout=10s 												  | 	   |SW3         |SW2       |                                                  |
						Exit from Menu without  									  |	       v            |          |                                                  |
						change in all cases											  |  +----------------------+      |                                                  |
																					  |->| Cooling and Heating  |  SW1 |    +-----------------------------------------+   |
																						 |                      |------|--->| Trace msg: Exit from Menu with new value|   |
																						 +----------------------+      |    |                                         |<--|
																					           |            ^          |    +-----------------------------------------+
																							   |SW3         |SW2       |
																							   v            |          |
																				  +-------------------------------+    |
																				  | Cooling and Heating + Reheat  | SW1|
																				  |                               |----|
																				  +-------------------------------+
																				
			  					  


</pre> 	
		
**Note:** When LED Red, Green and Blue are toggling it is indicating an error has occurred on application.

### __Hardware and Software environment__

* This example runs on STM32WBA65xx devices.  

* This example has been tested with an STMicroelectronics STM32WBA65CGA_Nucleo board and can be easily tailored to any other supported device and development board.  

* On STM32WBA65CGA_Nucleo, the jumpers must be configured as described in this section. Starting from the top left position up to the bottom right position, the jumpers on the Board must be set as follows:
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
