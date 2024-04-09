## __Zigbee_Messaging_Client_Coord Application Description__

How to use the Messaging cluster on a device acting as a Coordinator within a Centralized Zigbee network.

The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the Messaging cluster. Once the Zigbee mesh network is created, the Server can send messages to the Client through push buttons SW1/SW2.

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, Messaging cluster  

### __Hardware Requirements__

For this application it is requested to have at least:

* One STM32WBA55xx Nucleo board loaded with application **Zigbee_Messaging_Client_Coord**
* One or more STM32WBA55xx board loaded with application **Zigbee_Messaging_Server_Router**
</br>

<pre>
	
                 Router                                               Coord.
               +------------+                                      +------------+
               |            |                                      |            |
               | Messaging  |                                      | Messaging  |       
               | Server     |                                      | Client     | 
               |            |                                      |            |
   Short       |            |   Send "Hello" message               |            |
   PushB SW1=> |            | -----------------------------------> |            |=> immediate display
               |            |                                      |            | 
			   |            |                                      |            |
   Long        |            |   Send "Hello" message               |            |
   PushB SW1   |            | -----------------------------------> |            |=> immediate display and ask for confirmation
			   |            |                                      |            |
			   |            |   Send confirmation for message ID 1 |            |
			   |            | <----------------------------------- |            |<= PushB SW1
			   |            |                                      |            |
			   |            |                                      |            |
   Short       |            |   Send "Goodbye" message             |            |
   PushB SW2=> |            | -----------------------------------> |            |=> start wait of 10 seconds
			   |            |                                      |            |
			   |            |                                      |            |=> after 10 seconds, display message
			   |            |                                      |            |
   Long 	   |            |   Send "Goodbye" message             |            |
   PushB SW2=> |            | -----------------------------------> |            |=> start wait of 10 seconds 
			   |            |                                      |            |
			   |            |                                      |            |=> after 10 seconds, display message
			   |            |                                      |            |=> ask for confirmation
			   |            |                                      |            |
			   |            |  Send confirmation for message ID 2  |            |
			   |            | <----------------------------------- |            |<= PushB SW1
			   |            |                                      |            |
			   |            |                                      |            |
   Short or    |            |                                      |            |
   Long 	   |            |                                      |            |
   PushB SW2=> |            |  Send "Goodbye" message              |            |
			   |            | -----------------------------------> |            |=> start wait of 10 seconds
			   |            |                                      |            |
			   |            |  Send Cancel "Goodbye" message       |            |
   PushB SW3=> |            | -----------------------------------> |            |=> remove "Goodbye" message
			   |            |                                      |            |
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
By default, this application runs with Zigbee PRO stack R23.

If you want to run this application using Zigbee PRO stack R22, you should replace ZigBeeProR23_FFD.a by ZigBeeProR22_FFD.a and ZigBeeProR23_RFD.a by ZigBeeProR22_RFD.a and ZigBeeClusters.a by ZigBeeClustersR22.a in the build environment.

Also, set in the project setup compilation flag CONFIG_ZB_REV=22. 
