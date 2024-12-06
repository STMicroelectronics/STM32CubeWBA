## __Thread_Coap_OTA Application Description__

How to build Thread application based on Coap messages.

### __Keywords__

Connectivity, 802.15.4 protocol, Thread, COAP, OTA

### __Hardware and Software environment__

* This example runs on STM32WBA55xx devices.  

* This example has been tested with an STMicroelectronics STM32WBA55CGA_Nucleo board and can be easily tailored to any other supported device and development board.  

* This application is comptable with Thread_OTA_Server application, which means the generated binary should include a magic word and CRC for the binary image at
  the end of the image.

### __How to use it?__

In order to make the program work, you must do the following:

- Install Srec cat tool using this link https://srecord.sourceforge.net/download.html
- Connect 2 STM32WBA55xx_Nucleo boards to your PC 
- Open your preferred toolchain 
- Rebuild all files and load your image into target memory
- Run the application   
 
**Note:** when LED1, LED2 and LED3 are toggling it is indicating an error has occurred on application. 

If you want to control this application, you can directly send and retrieve Cli commands connecting an HyperTerminal with the FTDI cable as following :  

- TX GPIO 17  
- RX GPIO 18  
 
In a Thread network, nodes are split into two forwarding roles: **Router** or **End Device**.    
The Thread **Leader** is a Router that is responsible for managing the set of Routers in a Thread network.    
An End Device (or child) communicates primarily with a **single Router**.    

In our Application which uses two devices, one device will act as a Leader (Router) and the other one will act as an End Device(mode child). 

After the reset of the 2 boards, one board will be in Leader mode (**Green LED2 ON**).    
The other one will be in Child mode (**Red LED3 ON**).  

Let's name indifferently one board **A** and one board **B**.   

- press the SW1 Push-Button on board A to send a **COAP command (Non-Confirmable)** from board A to board B.  
The board B will receive COAP commands to toggle its **blue LED1**.
- press the SW2 Push-Button on boad A to send a **COAP command (Confirmable)** from board A to board B.  
The board B will receive COAP commands and send to board A a **Coap Data response** and toggle its **blue LED1**.

Same COAP commands can be sent from board B to board A.    
 
<pre>
	
  ___________________________                       ___________________________
  |  Device A               |                       | Device B                |
  |_________________________|                       |_________________________|  
  |                         |                       |                         |
  |                         |                       |                         |
  |                SW1 -->  |======> COAP =========>| BLUE LED TOGGLE (ON/OFF)|
  |                         | Resource "light"      |                         |
  |                         | Mode: Multicast       |                         |
  |                         | Type: Non-Confirmable |                         |
  |                         | Code: Put             |                         |
  |                         |                       |                         |
  |                         |                       |                         |
  |                SW2 -->  |=====> COAP ==========>|-------->                |
  |                         | Resource "light"      |         |               |
  |                         | Mode: Multicast       |  CoapRequestHandler()   |
  |                         | Type: Confirmable     |         |               |
  |                         | Code: Put             |         |               |
  |                         |                       |         v               |
  |                         |                       |  CoapSendDataResponse() |
  |                         |                       |         |               |
  |                         |                       |         v               |
  | CoapDataRespHandler()<--|<===== COAP <==========| <-------                |
  |                         |                       | BLUE LED TOGGLE (ON/OFF)| 
  |                         |                       |                         |  
  ---------------------------                       ---------------------------
  | Role : Child            |                       | Role : Leader           |
  |                         |                       |                         |
  | LED : Red               |                       | LED : Green             |
  |                         |                       |                         |
  |_________________________|                       |_________________________|

  
</pre> 


IMPORTANT PREREQUISITES:
Thread_Coap_OTA should be loaded at @0x08080000 and Thread_OTA_Client should be existed at @0x08000000.

If Thread_Coap_OTA receive **COAP command (Confirmable) "FUOTA_REBOOT"** from Thread_OTA_Server, it will reset 
and run the Thread_OTA_Client.


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





