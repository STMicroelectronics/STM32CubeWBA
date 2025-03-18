## __Zigbee_TempMeas_Server_SED Application Description__

How to use the Temperature Measurement cluster on a device acting as a Server within a Centralized Zigbee network.  
    
The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the Temperature Measurement cluster. 
Once the Zigbee mesh network is created, the Server sends regularly the Temperature to Client. The Server is a SED device, no traces are available.  

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, Temperature Measurement cluster  

### __Hardware Requirements__

For this application it is requested to have:  

* One STM32WBA65xx Nucleo board loaded with application : **Zigbee_TempMeas_Client_Coord**  
* One or more STM32WBA65xx board loaded with application : **Zigbee_TempMeas_Server_SED**  

### __Demo use case__

* This demo shows how to use Temperature Measurement cluster:
	* The Temperature Measurement Server update regularly (500 ms) its **ZCL_TEMP_MEAS_ATTR_MEAS_VAL** attributes (via the measure of a sensor or by random).  
	
    * The Temperature Measurement Client send a Report Request to obtains regularly the **ZCL_TEMP_MEAS_ATTR_MEAS_VAL** attributes.  
	
	* The Temperature Measurement Server send regularly (5 seconds) a Report with the latest value of **ZCL_TEMP_MEAS_ATTR_MEAS_VAL** attributes.  
	  
* 1 read-only attributes is used :
    * Temperature Value (16-bit integer)  
	
<pre>
    
    

               Coord.                                                                    SED
             +----------+                                                              +----------+
             |          |                                                              |          |                                       
             | TempMeas |                                                              | TempMeas |
             [ Client   |                                                              | Server   |  - Temperature Server during Init 
             |          |                                                              |          |    launch a 500 ms Periodic Timer
             |          |                                                              |          |  
             |          |                                                              |          |  - Every 500 ms 
             |          |                                                              |          |    * Read the Temperature Sensor (if exist)
             |          |                                                              |          |      or simulate it with RNG.
             |          |                                                              |          |    * <= ZbZclAttrIntegerWrite(ZCL_TEMP_MEAS_ATTR_MEAS_VAL) 
             |          |  After a SED was connected :                                 |          |
             |          |  ZbZclAttrReportConfigReq(ZCL_TEMP_MEAS_ATTR_MEAS_VAL, 5sec) |          |
             |          | -----------------------------------------------------------> |          |
             |          | <----------------------------------------------------------- |          |
             |          |                                                              |          |
             |          | <-------------- Report (every 5 seconds) ------------------- |          |
             |          |                                                              |          |             
             |          |                                                              |          | 	 
             |          |                                                              |          |			 
             |          |                                                              |          |		 
             |          |                                                              |          |			 
             +----------+                                                              +----------+
  

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA65xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network, so in this demo application it is the device running Zigbee_TempMeas_Client_Coord application.  
    
	2. Wait for the Blue LED ON.  
	
    3. Start the second board. This board is configured as Zigbee SED and will be attached to the network created by the coordinator.
	Do the same for the other boards if applicable.      
	 
	4. On Server side, every 500 ms, the Temperature Measured is read .
	On Client side, every 5 seconds, the Temperature Measured is reported from the Server.
  
**Note:** To have the better consumption as possible, this SED device do not manage LED.
		
### __Hardware and Software environment__

* This example runs on STM32WBA65xx devices.  

* This example has been tested with an STMicroelectronics STM32WBA65RI_Nucleo board and can be easily tailored to any other supported device and development board.  

* On STM32WBA65RI_Nucleo, the jumpers must be configured as described in this section. Starting from the top left position up to the bottom right position, the jumpers on the Board must be set as follows:
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

* To have the better consumption as possible, this SED device do not have trace. 
  
