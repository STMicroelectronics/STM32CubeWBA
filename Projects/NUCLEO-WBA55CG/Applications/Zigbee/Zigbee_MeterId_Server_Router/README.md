## __Zigbee_MeterId_Server_Router Application Description__

How to use the Meter Identification cluster on a device acting as a Server within a Centralized Zigbee network.

The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using  Meter Identification cluster. Once the Zigbee mesh network is created, the user can send requests from the client to the server through push buttons SW1 and SW2.  

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, Meter Identification cluster 

### __Hardware Requirements__

For this application it is requested to have:  

* One STM32WBA55xx Nucleo board loaded with application : **Zigbee_MeterId_Client_Coord**  
* One or more STM32WBA55xx board loaded with application : **Zigbee_MeterId_Server_Router**  

### __Demo use case__

* This demo shows how to use Meter Identification cluster:  
	* The Meter Identification Server set up its read-only attributes (read-only attributes local write allowed).  
	* The Meter Identification Client can remotely read them by requesting a remote read.  
* 4 read-only attributes are used :
	* Company Name (ZCL string with 16 characters)  
	* Meter Type ID (16-bit integer)  
	* Data Quality ID (16-bit integer)  
	* Meter POD Attribute (ZCL string with 16 characters)  
	
<pre>
    

               Coord.                                                                  Router
             +---------+                                                             +---------+
             |         |                                                             |         |                                       
             | MeterID |                                                             | MeterID |
             [ Client  |                                                             | Server  |  During MeterId Server Init :
             |         |                                                             |         |  <-- ZbZclAttrStringWriteShort(ZCL_METER_ID_ATTR_COMPANY_NAME)
             |         |                                                             |         |  <-- ZbZclAttrIntegerWrite(ZCL_METER_ID_ATTR_METER_TYPE_ID)
             |         |                                                             |         |  <-- ZbZclAttrIntegerWrite(ZCL_METER_ID_ATTR_DATA_QUAL_ID)
             |         |                                                             |         |  <-- ZbZclAttrStringWriteShort(ZCL_METER_ID_ATTR_POD)
             |         |                                                             |         |
             |         |                                                             |         |
             |         |                                                             |         |
             |         |    ZbZclReadReq(ZCL_METER_ID_ATTR_COMPANY_NAME)             |         |
  PushB SW1=>|         | ----------------------------------------------------------> |         |
             |         | <---------------------------------------------------------- |         |
             |         |                                                             |         | 
             |         |    ZbZclReadReq(ZCL_METER_ID_ATTR_METER_TYPE_ID,            |         |
             |         |   ZCL_METER_ID_ATTR_DATA_QUAL_ID & ZCL_METER_ID_ATTR_POD)   |         |
  PushB SW2=>|         | ----------------------------------------------------------> |         |
             |         | <---------------------------------------------------------- |         |
             |         |                                                             |         |
             +---------+                                                             +---------+
  

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network so in this demo application it is the device running Zigbee_MeterId_Client_Coord application.  

	2. Wait for the Blue LED ON.  

	3. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator.
Do the same for the other boards if applicable.  
&rarr; At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons.  

	4. It is now possible to remotely read Meter Identification server attributes on the client by pressing on the SW1/SW2 push buttons.  
	   
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
 