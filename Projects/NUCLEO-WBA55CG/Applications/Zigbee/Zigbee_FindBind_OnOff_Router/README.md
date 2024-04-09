## __Zigbee_FindBind_OnOff_Router Application Description__

How to use the Finding and Binding  feature on a device acting as a Client with Router role within a Centralized Zigbee network.

The purpose of this application is to show how Finding and Binding (F&B) works on a Zigbee centralized network. It will demonstrate how F&B initiator will automatically create bindings  to bindable clusters on the F&B target. Once clusters are bound, they will be able to communicate through bindings which will toggle LEDs and output messages through the serial interface.  
  
For this specific application:  
  
* ZC embeds following clusters:  
	* ZCL Scenes Server  
	* ZCL Messaging Server  
	* ZCL IAS Warning Device Server  
	* ZCL OnOff Server  
		
* ZC Will have the following functionality:  
	* SW1: Turn on identify mode for 30 seconds (For F&B)  
	* SW2: Send ZCL Display Message Command via its bindings (Sending will fail if no bindings to this cluster exists)  
		
* ZC Has the following LEDs:  
	* RED: IAS Warning status  
	* GREEN: OnOff attribute state  
	* BLUE:  
		* ON: Network formation success  
		* OFF: Network formation failure

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, OnOff cluster, Scenes cluster, Messaging cluster, IAS Warning Device cluster, Identify cluster, Finding and Binding


### __Hardware Requirements__

For this application it is requested to have:  

* One STM32WBA55xx Nucleo board loaded with application : **Zigbee_Find_Bind_Coord**  
* One or more STM32WBA55xx board loaded with application : **Zigbee_Find_Bind_OnOff_Router**  
* One or more STM32WBA55xx board loaded with application : **Zigbee_Find_Bind_IAS_Router**  

<pre>
 
            Router IAS                                      Coordinator

             ---------                                      ---------
             |       |       ZCL commands                   |       |
      PushB=>|Client | <----------------------------------> |Server | =>LED
             |       |                  |                   |       | =>UART
             |       |                  |                   |       |
              --------                  |                   ---------
                                        |
            Router OnOff                |
                                        |
             ---------                  |
             |       |                  |
      PushB=>|Client |<-----------------
             |       |
             |       |
              --------


 ---------------------------------      ------------------------------    ------------------------
 | Zigbee_Find_Bind_OnOff_Router |      |Zigbee_Find_Bind_IAS_Router |    |Zigbee_Find_Bind_Coord |
 ---------------------------------      ------------------------------    ------------------------
             |                                       |                             | 
             |                                       |                 Power up  =>|
             |                                       |                             |Allocate 
             |                                       |                             | -OnOff
             |                                       |                             | -IdentifyServer, 
             |                                       |                             | -ScenesServer,
             |                                       |                             | -IasWdServer,
             |                                       |                             | -MsgServer, 
             |                                       |                             |Clusters
             |                                       |                             |
             |                                       |                             |Creation of the network
             |                                       |                             |(Blue LED goes On)
             |                                       |                             |
             |                                       |                      SW1  =>|Turn on the                                                       
             |                                       |                             |Identify Mode 
             |                                       |                             |(valid during 30 Seconds)
             |                                       |                             |
             |                           Power up  =>|                             |
             |                                       |                             |
             |                                       |Allocate                     |
             |                                       | -MsgClient                  |
             |                                       | -IdentifyClient             |
             |                                       |Clusters.                    |
             |                                       |                             |           
 Power up  =>|                                       |                             |
             |Allocate                               |                             |
             |-OnOffClient                           |                             |
             |-IdentifyClient                        |                             |
             |-ScenesClient                          |                             |
             |Clusters                               |                             |
             |                                       |                             |
             |After a few seconds,                   |After a few seconds,         |
             |the blue LED should light on           |the blue LED should light on |
             |(The network is now established)       |(The network is now establ.) |
             |                                       |                             |
             |Once the find and bind procedure       |                             |
             |is completed, the green LED goes On    |                             | 
             |(Binding entries created = 3)          |                             |
             |                                       |                             | 
             |(Note : On ZR1, the find and bind is   |                             |
             |initiated automatically)               |                             |
             |                                       |                             |
             |                                       |                             |
             |                                SW1 => |Initiate the                 |
             |                                       |find and bind procedure      |
             |                                       |                             |
             |                                       |Once the find and bind       |
             |                                       |procedure is completed,      |
             |                                       |the green LED goes On        |
             |                                       |(Binding entries created = 2)|    
             |                                       |                             |
             |                                       |Allocate                     |
             |                                       |-IAS_client cluster          |
             |                                       |Clusters                     |
             |                                       |once the first find          |
             |                                       |and bind as been done        |
             |                                       |                             |
             |                                       |                             |
             |                            Display msg|<----------------------------|<= SW2
             |                      (refer to traces)|                             |
             |                                       |                             |
             |                                       |                             |
             |                                 SW1 =>|Initiate a new               |
             |                                       |find and bind (adding IAS)   |
             |                                       |(Binding entries created = 3)|
             |                                       |                             |
             |                                       |                             |
             |                                       |                             |
             |                                       |    Sending Warning command  |
             |                                 SW2 =>|---------------------------->|
             |                                       |                             |Red LED
             |                                       |                             |goes on 
             |                                       |                             |for 3 sec
             |                                       |                             |
       SW1 =>|---------------------------------------------------onOff togle------>|Green LED
             |                                       |                             |toggles
             |                                       |                             |
             |                                       |                             |
       SW2 =>|---------------------------------------------------Store Scene------>| Store Green LED status
             |                                       |                             | (either on or off)
             |                                       |                             |
       SW1 =>|---------------------------------------------------onOff togle------>|Green LED
             |                                       |                             |toggles
             |                                       |                             |
             |                                       |                             |
       SW3 =>|---------------------------------------------------Recall Scene----->| If the Green LED status
             |                                       |                             | stored previously via the 
             |                                       |                             | store scene command was on,
             |                                       |                             | the green LED will go back 
             |                                       |                             | to on
             |                                       |                             |
             

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
	1. On power, the Zigbee **Coordinator** (ZC) will initialize the stack and allocate the following clusters on endpoint 10:
		* ZCL Scenes Server
		* ZCL Messaging Server
		* ZCL IAS Warning Device Server
		* ZCL OnOff Server
	The application will then form the network and will turn on the **BLUE LED** on successful network formation.
	Should the network formation fail, the BLUE LED will remain off and the application will reattempt network formation after five seconds.

	2. On **ZC**, the user can press the following button at any time after successful network formation:
		* SW1 : Turn on Identify Mode for 30 Seconds
	**NOTE:** This button needs to be pressed before any of the routers perform **finding and binding**.

	3. On power, Zigbee **OnOff Router** will initialize the stack and allocate the following clusters on endpoint 14:
		* ZCL Scenes Client
		* ZCL OnOff Client
	The application will then attempt to join a network (Blue LED blinks), and start the Finding and Binding procedure as an Initiator. 
	The ZR-OnOff will turn on the **BLUE LED** on successful network join, and turn on the **GREEN LED** on successful F&B procedure.
	Should the network join or F&B procedure fail, the RED LED will turn on and the application will attempt to send a Network Leave, and then attempt to join and F&B after five seconds.

	4. On power, Zigbee **IAS Router** will initialize the stack and allocate the following clusters on endpoint 12:
		* ZCL Messaging Client
	The application will then attempt to join a network. The ZR-IAS will turn on the **BLUE LED** on successful network join. 
	Should the network join fail the **RED LED** will turn on, and then attempt to join after five seconds.

	5. On Zigbee **IAS Router**, the user need to press the following button :
		* SW1: Start the Finding and Binding procedure as an Initiator. It’s turn on the **GREEN LED** on successful F&B procedure.  
	
	6. On Zigbee **OnOff Router**, the user can press the following buttons at any time after successful network join and F&B to send ZCL commands via the established bindings:
		* SW1: ZCL OnOff Toggle Command &rarr; ZC will change the current value of the OnOff attribute (and the GREEN LED) upon reception of the ZCL OnOff Toggle Command.
		* SW2: ZCL Store Scenes Command &rarr; ZC will store the current value of the OnOff attribute upon reception of the ZCL Store Scenes Command.
		* SW3: ZCL Recall Scenes Command &rarr; ZC will restore the current value of the OnOff attribute (and the GREEN LED) upon reception of the ZCL recall Scenes Command.  
		
	7. On Zigbee **IAS Router**, the user can press the following button at any time after successful network join and F&B :
		* SW3: Send a ZCL Get Last Message command from the ZCL Messaging Client via the established bindings.
	This will prompt the ZC Messaging Server to send a Display Last Message command, and will display messages via the UART.

	8. On **ZC**, the user can press the following button at any time after successful network formation:
		* SW2: Send ZCL Display Message command via Bindings
	**NOTE:** This command will fail unless the messaging server is bound to a client.

	9. On Zigbee **IAS Router**, via application specific means, it will acknowledge that it has successfully performed F&B from previous steps.
		* Pressing SW1 again : will allocate the ZCL IAS Warning Device Server cluster on endpoint 12, and will re-perform Finding and Binding as an Initiator. 
		The **GREEN LED** will stay on if the F&B procedure is successful. Otherwise the GREEN LED will turn off, and the user can press SW1 again to reattempt F&B.  
	
	10. On Zigbee **IAS Router**, the user can press the following button at any time after successful second F&B :
		* SW2: Send a ZCL Start Warning Message command via the established bindings.
	This will prompt the ZC WD Server to turn on the RED LED for the duration of the Warn Device Command.  
	
	  
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
