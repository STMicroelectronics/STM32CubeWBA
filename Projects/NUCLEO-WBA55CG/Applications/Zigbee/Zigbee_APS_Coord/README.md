## __Zigbee_APS_Coord Application Description__

How to use the APS layer in an application acting as a Zigbee Coordinator within a centralized network.  

The purpose of this application is to show how to create a Zigbee centralized network, and how use the APSDE interface to the Zigbee stack directly, to send and receive raw APS messages between devices on the network.  
Once the Zigbee mesh  network is created, the user can send APS requests from the Zigbee router to the Zigbee coordinator through push buttons SW1.

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, APS layer 

### __Demo use case__

* This demo shows how to use APSDE interface on a customer application.
	* Packets are sent using the APSDE Data Request API.
	* Packets are received by a filter callback configured at the device initialization.

* The APS messages use a profile of 0xf000, which is in the manufacturer specific range for profile Ids and was arbitrarily chosen. In an actual product, the profile Id should be registered with the Zigbee Alliance.

* The router will send APS request commands (clusterID = 0x0000) to the coordinator (at address 0x0000) through push button SW1.  
If a device receives a request command, it sends a response command (cluster Id = 0x0001) back to the originating device.

* The payloads used in this application are arbitrary and not used by the sender or receiver.
APS frame exchange scheme is available through traces.  

### __Hardware Requirements__

For this application it is requested to have at least:

* One STM32WBA55xx Nucleo board loaded with application **Zigbee_APS_Coord**
* One or more STM32WBA55xx board loaded with application **Zigbee_APS_Router**
</br>

<pre>
	

              Device 1                                                                        Device 2
          
             +--------+                                                                      +--------+
             |        |                                                                      |        |                                       
             | Router |                                                                      | Coord  | 
             |        |                                                                      |        |
             |        |                                                                      |        |
             |        |                       ZbApsdeDataReqCallback()                       |        |
  PushB SW1=>|        | -------------------------------------------------------------------> |        |
             |        | <------------------------------------------------------------------- |        |
             |        |                                                                      |        |
             +--------+                                                                      +--------+

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network so in this demo application it is the device running Zigbee_APS_Coord application.  

	2. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator. 
Do the same for the other boards if applicable.  
&rarr; At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons.  

	3. Press on the push button (SW1) on Device 1 (Router board) and check the messages being exchanged via the traces. 

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

  




