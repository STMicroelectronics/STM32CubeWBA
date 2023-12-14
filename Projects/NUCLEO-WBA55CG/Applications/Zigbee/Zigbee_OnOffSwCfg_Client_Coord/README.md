## __Zigbee_OnOffSwCfg_Client_Coord Application Description__

How to use the OnOff Switch Config cluster on a device acting as a Client within a Centralized Zigbee network. 
    
The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the OnOff Switch Config cluster. 
Once the Zigbee mesh network is created, the user can send requests from the Client to the Server through push buttons SW1/SW2 in order to read or change attribute SWITCH ACTION.     

Note: The OnOff Switch Config Server Cluster needs always to be integrated in a device with a OnOff Client Cluster.

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, OnOff cluster, Switch Config cluster

### __Hardware Requirements__

For this application it is requested to have:  

* One STM32WBA55xx Nucleo board loaded with application : **Zigbee_OnOffSwfCfg_Client_Coord**  
* One or more STM32WBA55xx board loaded with application : **Zigbee_OnOffSwfCfg_Server_Router**  

<pre>
    
	
                            Coord.                                           Router
                          +--------+                                       +--------+
                          |        |                                       |        |
                          | OnOff  |                                       | OnOff  |        
                          | SwCfg  |                                       | SwCfg  | 
  	         		      | Client |                                       | Server | 
                          |        |                                       |        |
                          | OnOff  |                                       | OnOff  |        
                          | Server |                                       | Client | 
                          |        |                                       |        | 'OnOff Switch Config' during init : default Switch Action Toggle(SW1)
                          |        |                                       |        |   <= ZbZclAttrIntegerWrite(ZCL_ONOFF_SWCONFIG_ATTR_ACTIONS)
                          |        |                                       |        |			   
                          |        |   ZbZclOnOffClientToggleReq           |        |
        Red LED Toggle <= |        | <------------------------------------ |        | <= PushB SW1
                          |        |                                       |        |
                          |        |                                       |        |						  
	                      |        |   ZbZclWriteReq                       |        |
    * Change Action Mode  |        |   (ZCL_ONOFF_SWCONFIG_ATTR_ACTIONS)   |        |
            PushB SW2 =>  |        | ------------------------------------> |        | => Switch Action : from Toggle(SW1) to On(SW1)/Off(SW2)
                          |        |                                       |        |	 		   
                          |        |   ZbZclOnOffClientOnReq               |        |
            Red LED ON <= |        | <------------------------------------ |        | <= PushB SW1
                          |        |                                       |        |	 		   
                          |        |   ZbZclOnOffClientOffReq              |        |
           Red LED OFF <= |        | <------------------------------------ |        | <= PushB SW2
                          |        |                                       |        |
                          |        |                                       |        |						  
                          |        |   ZbZclReadReq                        |        |
                          |        |   (ZCL_ONOFF_SWCONFIG_ATTR_ACTIONS)   |        |
            PushB SW1 =>  |        | ------------------------------------> |        |
      Display Setting <=  |        | <------------------------------------ |        |
                          |        |                                       |        |						  
                          |        |                                       |        |						  
	                      |        |   ZbZclWriteReq                       |        |
    * Change Action Mode  |        |   (ZCL_ONOFF_SWCONFIG_ATTR_ACTIONS)   |        |
            PushB SW2 =>  |        | ------------------------------------> |        | => Switch Action : from On(SW1)/Off(SW2) to Off(SW1)/On(SW2)
                          |        |                                       |        |	 		   
                          |        |   ZbZclOnOffClientOnReq               |        |
            Red LED ON <= |        | <------------------------------------ |        | <= PushB SW2
                          |        |                                       |        |	 		   
                          |        |   ZbZclOnOffClientOffReq              |        |
           Red LED OFF <= |        | <------------------------------------ |        | <= PushB SW1
                          |        |                                       |        |
                          |        |                                       |        |						  
	                      |        |   ZbZclWriteReq                       |        |
    * Change Action Mode  |        |   (ZCL_ONOFF_SWCONFIG_ATTR_ACTIONS)   |        |
            PushB SW2 =>  |        | ------------------------------------> |        | => Switch Action : from Off(SW1)/On(SW2) to Toggle(SW1)
                          |        |                                       |        |	 		   
                          |        |   ZbZclOnOffClientToggleReq           |        |
        Red LED Toggle <= |        | <------------------------------------ |        | <= PushB SW1
                          |        |                                       |        |	 		   
                          |        |                                       |        |	 		   		   
                          +--------+                                       +--------+

  
</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network, so in this demo application it is the device running Zigbee_OnOff_Server_Coord application.  
    
	2. Wait for the Blue LED ON.  
	
    3. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator.
	Do the same for the other boards if applicable.    
&rarr; At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons.     
	 
	4. It is now possible to send OnOff Cluster commands from the Client to the Server in multicast mode by pressing on the SW1 or push button. You must see the Red LED state change on the Server side in function of **Switch Action** mode. 
	
	5. It's possible to display the Switch Config of an **OnOff Switch Config Server** by pressing the SW1 button on **OnOff Switch Config Client**.
	   
    6. It's possible to change (increment) the Switch Config of a **OnOff Switch Config Server** by pressing the SW2 button on **OnOff Switch Config Client**.
		
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
