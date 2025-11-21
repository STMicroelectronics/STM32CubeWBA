## __Zigbee_Basic_Client_Coord Application Description__

How to use the Basic Attributes of an OnOff cluster on a device acting as a Client within a Centralized Zigbee network.  
    
The purpose of this application is to show how to create a Zigbee centralized network, and how to read Basic attributes of an OnOff Client cluster. Once the Zigbee mesh network is created, the user can send requests from the Server to the Client through push buttons SW1 & SW2 in order to obtain Attributes.  

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, OnOff cluster, basic attributes 

### __Hardware Requirements__

For this application it is requested to have:  

* One STM32WBA55xx Nucleo board loaded with application : **Zigbee_Basic_Client_Coord**  
* One or more STM32WBA55xx board loaded with application : **Zigbee_Basic_Server_Router**  

<pre>
	
                 Router                                          Coord.
               +--------+                                      +--------+
               |        |                                      |        |
               | OnOff  |                                      | OnOff  |        
               | Client |                                      | Server | 
               |  with  |                                      |        |
               | Basic  |                                      | Basic  |
               | Attrb. |                                      | Client |
               |        |                                      |        |
   PushB SW1=> |        |    ZbZclOnOffClientToggleReq         |        |
               |        | -----------------------------------> |        | => Red LED Toggle
               |        |                                      |        |
               |        |                                      |        |
               |        |             ZbZclReadReq             |        | <= PushB SW1
               |        | <----------------------------------- |        | 
			   |        | -----------------------------------> |        | => Display Manufacturer Name
               |        |                                      |        |			   
               |        |                                      |        |			   			   
               |        |             ZbZclReadReq             |        | <= PushB SW2
               |        | <----------------------------------- |        | 
			   |        | -----------------------------------> |        | => Display ZCL Version & Power Source
               |        |                                      |        |			   			   
               +--------+                                      +--------+

</pre> 

### __Application Setup__  

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network so in this demo application it is the device running Zigbee_Basic_Client_Coord application.  

	2. Wait for the Blue LED ON.  

	3. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator.
Do the same for the other boards if applicable.  
&rarr; At this stage, the Zigbee network is automatically created and Blue LED is ON on all devices.  

	4. <p>**On Router**, It is now possible to send OnOff Cluster commands from the Client to the Server in multicast mode by pressing on the SW1 push button. 
You must see the Rer LED toggling on the Server side.  
**On Coord.**, it is now possible to send a ***ZCLReadRequest*** to obtain & display the **Manufacturer** of Router board by pressing SW1, or to send a ***ZCLReadRequest*** to obtain & display the **ZCL Version** and the **Power Supply** of Router board by pressing SW2.</p>  
   
**Note:** When LED Red, Green and Blue are toggling it is indicating an error has occurred on application.

## Hardware and Software environment

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


 



