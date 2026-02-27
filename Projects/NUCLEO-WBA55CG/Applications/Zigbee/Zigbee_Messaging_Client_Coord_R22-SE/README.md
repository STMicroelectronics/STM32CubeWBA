## __Zigbee_Messaging_Client_Coord_R22-SE Application Description__

How to use the Messaging cluster on a device acting as a Coordinator within a Centralized Zigbee network.

The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the Messaging cluster. Once the Zigbee mesh network is created, the Server can send messages to the Client through push buttons SW1/SW2.

Caution : to use this application, you need to add the CBKE Keys. To obtains these keys, you need to create an account on Certicom site (www.certicom.com) and request a 'ZigBee 1.2 Test Certificate' for each devices.

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, Messaging cluster  

### __Hardware Requirements__

For this application it is requested to have at least:

* One STM32WBA55xx Nucleo board loaded with application **Zigbee_Messaging_Client_Coord_R22-SE**
* One or more STM32WBA55xx board loaded with application **Zigbee_Messaging_Server_Router_R22-SE**
</br>

<pre>
	
                 Router                                               Coord.
               +------------+                                      +------------+
               |            |                                      |            |
               | Messaging  |                                      | Messaging  |       
               | Server     |                                      | Client     | 
               |            |                                      |            |
               |            |                                      |            |
   PushB SW1=> |            |   Send "Hello" message               |            |=> immediate display
               |            | -----------------------------------> |            | 
               |            |                                      |            |
   PushB SW2=> |            |   Send "Goodbye" message             |            |=> display after 10 seconds
			         |            | -----------------------------------> |            |
			         |            |                                      |            |
   Long 	     |            |   Send "Goodbye" message             |            |
   PushB SW2=> |            | -----------------------------------> |            |=> display after 10 seconds and ask for confirmation
			         |            |                                      |            |
			         |            |   Send confirmation for message ID x |            |
			         |            | <----------------------------------- |            |<= PushB SW1
			         |            |                                      |            |
               +------------+                                      +------------+

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network so in this demo application it is the device running Zigbee_Messaging_Client_Coord application.  

	2. Wait for the Blue LED ON.  

	3. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator.
Do the same for the other boards if applicable.  
&rarr; At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons.  

	4. It is now possible to send messages by pressing buttons.   
On Router:  
	* **SW1**: Send **Hello** message and display it immediately.     
	* **SW2**: Send **Goodbye** message and display it after 10 seconds.    
	* **SW2 long press**: Send **Goodbye** message, display it after 10 seconds and ask for confirmation.  
On Coord:  
	* **SW1**: Send Confirmation to the Server.  
Note that by pressing SW3 on Router within the 10 seconds, you can cancel the message **Goodbye**.  
 

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

### __Note__

This application runs only Zigbee PRO stack R22-SE. 
