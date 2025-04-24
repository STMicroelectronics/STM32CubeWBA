## __ZigbeeDirect_OnOff Application Description__
</br>
Zigbee Direct is a communication protocol designed to enable direct communication between Zigbee devices and smartphones 
or tablets without the need for a dedicated Zigbee hub or gateway.
Inside the Zigbee Direct ecosystem, you have two main actors : the ZVD (Zigbee Virtual Device) and the ZDD 
(Zigbee Direct Device). On this Application, the ZVD joins the ZDD Zigbee network and controls the Zigbee device by 
sending On and Off commands.


### __Keywords__

Connectivity, ZigbeeDirect, Zigbee protocol, BLE, 802.15.4 protocol

### __Hardware Requirements__

For this application it is requested to have at least:

* One STM32WBA55xx or STM32WBA65xx board loaded with the application **ZigbeeDirect_OnOff**
* One STM32WBA55xx or STM32WBA65xx board loaded with the application **ZigbeeDirect_OnOff_Server_Router_Identify**(For demo use case 2 only)
* One IOS device loaded with Ubisys Smart Home application



### __Demo use case 1__

</br>

<pre>


                       ZVD (Zigbee Virtual Device)          ZDD (Zigbee Direct Device)                                                       
                   +-------------------------------+           +-----------------+
                   |                               |           |                 |
                   | Ubisys Smart Home Application |           | WBA5 or WBA6    |
                   |      (running on IOS)         |           | Nucleo board    |
                   |                               |           |                 |
 Use the dedicated |                               |           |                 |
 smartphone apps =>|                               | --------> |                 |=> LED toggle
 to control the    |                               |           |                 |
 LED on the ZDD    |                               |           |                 |
 switch item       |                               |           |                 |
                   |-------------------------------|           |-----------------|


</pre> 

### __Application Setup__

* First, open the project, build it and load your generated application on your STM32WBAxx device 
* To run the application :

    1. Start the board. This board is configured and is acting as a Zigbee Direct Device. 
    2. Launch the Ubisys Application and follow the instructions described in the chapter " Demo 1 procedure" of following wiki page :
       https://wiki.st.com/stm32mcu/wiki/Connectivity:Ubisys_Smart_Home_Application_Zigbee_Direct_Setup


### __Demo use case 2__

</br>

<pre>

                       ZVD (Zigbee Virtual Device)          ZDD (Zigbee Direct Device)             ZD (Zigbee Device)                                                       
                   +-------------------------------+           +-----------------+              +-----------------+
                   |                               |           |                 |              |                 |
                   | Ubisys Smart Home Application |           | WBA5 or WBA6    |              | WBA5 or WBA6    |
                   |      (running on IOS)         |           | Nucleo board    |              | Nucleo board    |
                   |                               |           |                 |              |                 |
     Use the    => |                               | --------> |                 |=> LED toggle |                 |
    dedicated      |                               |           |                 |              |                 |
 smartphone apps   |                               |           |                 |              |                 |
  to control the   |                               |           |                 |              |                 |
  LED on the ZDD   |                               |           |                 |              |                 |
   switch item  => |                               | --------> |                 | -----------> |                 |=> LED toggle
                   |                               |           |                 |              |                 |
                   |-------------------------------|           |-----------------|              |-----------------|

</pre> 

### __Application Setup__

* First, open the ZDD project , build it and load your generated application on your STM32WBA55xx or STM32WBA65xx devices.
* To run the application :
    1. Start the first board. This board is configured as Zigbee Direct Device. 
    Launch the Ubisys Application and follow the instructions described in the chapter " Demo 2 procedure" of following wiki page :
       https://wiki.st.com/stm32mcu/wiki/Connectivity:Ubisys_Smart_Home_Application_Zigbee_Direct_Setup

### __Help Commands__

* To get help, please have a look a the following commands.
  Open an HyperTerminal connected to the WBA board and enter the command Help :
  You will get access to the following commands :  

           APP_ZIGBEE_App_Task :    adv      ; Re-enable BLE advertisements
           APP_ZIGBEE_App_Task :    help     ; Print this help
           APP_ZIGBEE_App_Task :    on       ; Turn OnOff cluster LED ON
           APP_ZIGBEE_App_Task :    off      ; Turn OnOff cluster LED OFF
           APP_ZIGBEE_App_Task :    pjoin    ; Enable permit-join via ZDO broadcast
           APP_ZIGBEE_App_Task :    status   ; print overall status
           APP_ZIGBEE_App_Task :    swreset  ; board reset

### __Hardware and Software environment__

* This example runs on STM32WBA55xx and STM32WBA65xx devices.  

* This example has been tested with an STMicroelectronics STM32WBA55CGA_Nucleo and STM32WBA65xx_Nucleo board and can be easily tailored to any other supported device and development board.

* On STM32WBA55CGA_Nucleo or on STM32WBA65RI_Nucleo, the jumpers must be configured as described in this section. Starting from the top left position up to the bottom right position, the jumpers on the Board must be set as follows:
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

* To get more information on the subject, please take a look at our ST Wiki page : https://wiki.st.com/stm32mcu/wiki/Connectivity:Ubisys_Smart_Home_Application_Zigbee_Direct_Setup


* To get the traces and having access to the 'help' function you need to connect your Board to the HyperTerminal (through the STLink Virtual COM Port).  

* The UART must be configured as follows:  
<br>
BaudRate       = 115200 baud</br>
Word Length    = 8 Bits</br>
Stop Bit       = 1 bit</br>
Parity         = none</br>
Flow control   = none</br>
Terminal   "Go to the Line" </br> 





