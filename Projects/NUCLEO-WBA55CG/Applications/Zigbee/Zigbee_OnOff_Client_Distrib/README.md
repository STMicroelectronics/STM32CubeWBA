## __Zigbee_OnOff_Client_Distrib Application Description__

How to use the OnOff cluster on a device acting as a Client within a distributed Zigbee network.
    
The purpose of this application is to show how to create a Zigbee distributed network, and how to communicate from one node to another one using the OnOff cluster. Once the Zigbee mesh network is created, the user can send requests from the client to the server through the push button in order to make the LED toggling.

### __Keywords__

Connectivity, Zigbee, Zigbee protocol, 802.15.4 protocol, distributed network  

### __Hardware Requirements__

For this application it is requested to have:  

* One STM32WBA55xx Nucleo board loaded with application : **Zigbee_OnOff_Server_Distrib**  
* One or more STM32WBA55xx board loaded with application : **Zigbee_OnOff_Client_Distrib**  

<pre>
    
    

              Device 1                                      Device 2
        
             ---------                                      ---------
             |       |       ZbZclOnOffClientToggleReq      |       |
      PushB=>|Client | -----------------------------------> |Server | =>LED
             |       |                                      |       |
             |       |                                      |       |
              --------                                      ---------
 
</pre> 

### __Application Setup__

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
	1. Start the first board (it can be either Device1 or Device2), wait for four seconds.   

	2. Start the second board, wait for four seconds.  
Do the same for the other boards if applicable.  
&rarr; At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons.   
**Note:** In order to create correctly the zigbee network, it is important to NOT switch on all the boards at the same time. Otherwise independent network can be created because we use distributed network configuration.  

	3. It is now possible to send OnOff Cluster commands from the client to the server in multicast mode by pressing on the SW1 push button.  
You must see the RED LED (LED3) toggling on the server side.  

<pre>

              ----------             -----------
              |Device 1 |............|Device 2 |           
              |(distr.) |            |(distr.) |
              -----------            -----------
                  .                       .
                  .                       .
                  .                       .
             -----------                  .
             |Device 3 |...................      
             |(distr.) |
             -----------
</pre>	
	   
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
