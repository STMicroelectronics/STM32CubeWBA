## __Zigbee_OnOff_Server_Coord Application Description__

How to use the OnOff cluster on a device acting as a Server within a Centralized Zigbee network.

The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the OnOff cluster. Once the Zigbee mesh network is created, the user can send requests from the Client to the Server through push button SW1 in order to make the LED toggling.

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, OnOff cluster

### __Hardware Requirements__

For this application it is requested to have at least:

* One B-WBA6M-WPAN Module board loaded with application **Zigbee_OnOff_Server_Coord**
* One or more B-WBA6M-WPAN Module board loaded with application **Zigbee_OnOff_Client_Router**
</br>

<pre>
	
                 Router                                          Coord.
               +--------+                                      +--------+
               |        |                                      |        |
               | OnOff  |                                      | OnOff  |        
               | Client |                                      | Server | 
               |        |                                      |        |
               |        |                                      |        |
   PushB SW1=> |        |    ZbZclOnOffClientToggleReq         |        |
               |        | -----------------------------------> |        | => LED Toggle
               |        |                                      |        |			   
               +--------+                                      +--------+

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA6Mxx devices.

* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network so in this demo application it is the device running Zigbee_OnOff_Server_Coord application.  

	2. Wait for the Blue LED ON.  

	3. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator.
     Do the same for the other boards if applicable.
     At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons.    

	4. It is now possible to send OnOff Cluster commands from the Client to the Server in multicast mode by pressing on the SW1 push button. 
     You must see the Blue LED toggling on the Server side.  

**Note:** When Blue continious toggling it is indicating an error has occurred on application.

### __Hardware and Software environment__

* This example runs on B-WBA6M-WPAN Modules.  

* This example has been tested with an STMicroelectronics B-WBA6M-WPAN Module board and can be easily tailored to any other supported device and development board.  

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
