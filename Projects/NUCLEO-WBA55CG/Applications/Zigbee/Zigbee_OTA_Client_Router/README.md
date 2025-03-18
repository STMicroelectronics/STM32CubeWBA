## __Zigbee_OTA_Client_Router Application Description__

How to use the OTA cluster on a device acting as a Client within a Centralized Zigbee network.

The purpose of this application is to show how to create a Zigbee centralized network, and how to communicate from one node to another one using the OTA cluster. Once the Zigbee mesh network is created, the user can send requests from the Client to the Server or vice-versa through push button SW1 on the Client or Server respecfully in order to initiate an OTA upgrade. The Client will jump to the OTA image with SW2.

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, OTA cluster

### __Hardware Requirements__

For this application it is requested to have at least:

* One STM32WBA55xx Nucleo board loaded with applications:
  * **Zigbee_OTA_Server_Coord** at **0x08000000** and 
  * **Zigbee_OnOff_Client_Router_OTA** at **0x08080000** (this application is transmitted in the OTA transfer)
* One or more STM32WBA55xx board loaded with application **Zigbee_OTA_Client_Router** also called ZR_OTA in this readme
</br>

<pre>

                                    Router                                                               Coord.
                                  +--------+                                                           +--------+
                                  | ZR_OTA |                                                           |        |
                                  | Client |                                                           | OTA    |
                                  |        |                                                           | Server |
                                  |        |                                               Push SW2 =&gt; |        | =&gt; Print Available Images for OTA Transfer
                                  |        |                                                           |        |
                      PushB SW1=> | Client | => Green LED Start Toggling                               |        |
                                  |        |                                                           |        |
                                  |        |               ZbZclOtaClientQueryNextImageReq             |        |
                                  |        | --------------------------------------------------------> |        |
                                  |        |                                                           |        |
                                  |        |                                    Start OTA Procedure <= |        |
                                  |        |                                                           |        |
                                  |        |               ZbZclOtaServerImageNotifyReq                |        |
      Green LED Start Toggling <= |        | <-------------------------------------------------------- |        |
                                  |        |               ZbZclOtaClientQueryNextImageReq             |        |
                                  |        | --------------------------------------------------------> |        |
                                  |        |                                                           |        |
                                  |        |                           OTA OnGoing till OTA Upgrade <= |        |
     binary is valid - log end <= |        |                    (Section 11.12 OTA Upgrade Diagram)    |        |
                                  | ZR_OTA |                                                           |        |
                                  | and    |                                                           |        |
                                  |(image) |                                                           |        |
                                  |        |                                                           |        |
                     PushB SW2 => |        | => Reboot and Jump to OTA Image                           |        |
                                  |        |     Image will connect if Coord get reset        Reset => |        | => Allow OTAImage (OnOff Router) to connect
                                  |OTAImage|                                                           |        |
                                  |  and   |                                                           |        |
                                  |(ZR_OTA)|                                                           |        |
                                  |        |                                                           |        |
                                  |        |                                                           |        |
                                  |OTAImage|                                                           |        |
                                  |  and   |                                                           |        |
                                  |(ZR_OTA)|                                                           |        |
                         Reset => |        | => Reboot and Jump to ZR_OTA                              |        |
                                  |        |     Image will connect if Coord get reset        Reset => |        | =>; Allow ZR_OTA to connect
                                  | ZR_OTA |                                                           |        |
                                  | and    |                                                           |        |
                                  |(image) |                                                           |        |
                                  |        |                                                           |        |
                                  +--------+                                                           +--------+
                                  
</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application:
    1. Start the first board. It must be the Coordinator of the Zigbee network. So, in this demo application it is the device running the Zigbee_OTA_Server_Coord application.

    2. Wait for the Blue LED to turn ON.

    3. Start the second board. This board is configured as Zigbee router and will be attached to the network created by the Coordinator. Do the same for the other boards if applicable.
&rarr; At this stage, the Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED turn ON before pushing buttons.

    4. It is now possible to send OTA Cluster commands from the Client to the Server or vice-versa in unicast or broadcast mode by pressing on the SW1 push button on the Client or Server respecfully. You will see the Green LED toggling on the Client during OTA. Note that ZR_OTA is deleting the OTAImage - if any OTAImage was existing - before processing to OTA process. When the upgrade finishes, you shall see the Green LED stop toggling on the Client followed by validation of the image on the Uart log, endding by: image is valid. The Client will NOT jump to the OTA image.
    
    5. Once the Client has received the OTA image, it is possible to soft reset using push button SW2 on the Client to jump to the OTA image. 

    6. Once the Client has received the OTA image, it is also possible to hard reset using the Reset push button on the Client to restore the OTA Client and if existing it will NOT delete the OTA image to be abble to later switch to OTAImage with SW2.
    
    7. By pressing the SW2 push button on the Server, all availible images for download on the Server will be logged.

**Note:** When the Red, Green, and Blue LED are toggling, it is indicates an error has occurred in the application.

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

* To get the traces, you need to connect your Board to the Hyperterminal (through the STLink Virtual COM Port).

* The UART must be configured as follows:
<br>
BaudRate       = 115200 baud</br>
Word Length    = 8 Bits</br>
Stop Bit       = 1 bit</br>
Parity         = none</br>
Flow control   = none</br>
Terminal   "Go to the Line" : &lt;LF&gt;
