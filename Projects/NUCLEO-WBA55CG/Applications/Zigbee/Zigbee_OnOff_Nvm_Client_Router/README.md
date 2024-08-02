## __Zigbee_OnOff_Nvm_Client_Router Application Description__

How to use the OnOff cluster on a device acting as a Client within a Centralized Zigbee network.

The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the OnOff cluster. Once the Zigbee mesh network is created, the user can send requests from the Client to the Server through push button SW1 in order to make the LED toggling. Devices can also reboot from persistence.

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, OnOff cluster

### __Hardware Requirements__

For this application it is requested to have at least:

* One STM32WBA55xx Nucleo board loaded with application **Zigbee_OnOff_Nvm_Server_Coord**
* One or more STM32WBA55xx board loaded with application **Zigbee_OnOff_Nvm_Client_Router**
</br>

<pre>
  
                 Router                                          Coord.
               +--------+                                      +--------+
               |        |                                      |        |
               | OnOff  |                                      | OnOff  |        
               |  Nvm   |                                      |  Nvm   | 
               | Client |                                      | Server |
               |        |                                      |        |
   PushB SW1=> |        |    ZbZclOnOffClientToggleReq         |        |
               |        | -----------------------------------> |        | => LED Toggle
               |        |                                      |        |
   PushB SW2=> | Erase  |                                      |        | 
               | NVM &  |                                      |        |
               | Reboot |                                      | Erase  | <= PushB SW2
               |        |                                      | Nvm &  |
               |        |                                      | Reboot |
               |        |                                      |        |        
               |        |                                      | Start  | <= PushB SW3
               |        |                                      | Join   |
               |        |                                      | 60s    |
               |        |                                      |        |        
               +--------+                                      +--------+

</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
    1. Start the first board. It must be the coordinator of the Zigbee network so in this demo application it is the device running Zigbee_OnOff_Nvm_Server_Coord application.
       If the device starts from persistent data, the Green LED turns ON.
       Else, wait for the Blue LED ON.

    2. Start the second board. At this stage the second board tries to start from persistence this leads to two possible outcomes:
        1. Persistent data read from NVM is valid -> The router takes back is role in the network.
       The Green LED turns ON indicating a successful restart form persistence.

        2. No Persistent data found or persistent data read from NVM is invalid -> The router must join the network.
       The Blue LED toggles indicting an attempt to join.
       The Blue LED turns ON when the router has successfully joined.

    3. It is now possible to send OnOff Cluster commands from the Client to the Server in multicast mode by pressing on the SW1 push button. 
       You must see the Red LED toggling on the server side. Persistent data is regularly saved to NVM.

    4. Devices can be rebooted at any time and are expected to successfully restart from persistence.
     The Server should restore its cluster attributes in the event of a reboot.

    5. By pressing on SW2 (possible at any time), the user deletes the persistent data in NVM and the device reboots immediately without persistence. 

    6. After a startup from persistence on the coordinator, press SW3 to open a join window for 60s to join new routers.

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
 