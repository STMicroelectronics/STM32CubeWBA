## __Zigbee_OnOff_Client_Router_FreeRtos Application Description__

How to use the OnOff cluster on a device acting as a Client within a Centralized Zigbee network.

The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the OnOff cluster. Once the Zigbee mesh network is created, the user can send requests from the Client to the Server through Joystick SELECT in order to make a new message on LCD.
The application runs under FreeRtos operating system.

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, OnOff cluster, FreeRtos  

### __Hardware Requirements__

For this application it is requested to have at least:

* One STM32WBA55G-DK Demokit board loaded with application **Zigbee_OnOff_Server_Coord**
* One or more STM32WBA55G-DK Demokit board loaded with application **Zigbee_OnOff_Client_Router**
</br>

<pre>
	
                       Router                                          Coord.
                     +--------+                                      +--------+
                     |        |                                      |        |
                     | OnOff  |                                      | OnOff  |        
                     | Client |                                      | Server | 
                     |        |                                      |        |
                     |        |                                      |        |
   Joystick SELECT=> |        |    ZbZclOnOffClientToggleReq         |        |
                     |        | -----------------------------------> |        | => LED Toggle
                     |        |                                      |        |			   
                     +--------+                                      +--------+

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network so in this demo application it is the device running Zigbee_OnOff_Server_Coord application.  

	2. Wait for a message appears on the LCD.  

	3. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the coordinator.
     Do the same for the other boards if applicable.  
     &rarr; At this stage, a message on the LCD indicate network is being created. This usually takes about 15 seconds. It is important to wait this message becomes to continue.  

	4. It is now possible to send OnOff Cluster commands from the Client to the Server in multicast mode by pressing on the Joystick SELECT. 
     You must see a next message that appears on the LCD on the Server side. 

**Note:** When ALL LEDs are toggling it is indicating an error has occurred on application.

### __Hardware and Software environment__

* This example runs on STM32WBA55xx devices.  

* This example has been tested with an STMicroelectronics STM32WBA55G-DK board and can be easily tailored to any other supported device and development board.  

* On STM32WBA55G-DK, the jumpers must be configured as described in this section. Starting from the top left position up to the bottom right position, the jumpers on the Board must be set as follows:
<br>    
**JP4:**</br>
1-2:  [ON]</br>
3-4:  [OFF]</br>
5-6:  [OFF]</br>
7-8:  [OFF]</br>
<br>
**JP5:**</br>
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
