## __Thread_SED_Coap_Multicast Application Description__

How to build Thread sleepy end device application based on Coap messages.

### __Keywords__

Connectivity, 802.15.4 protocol, Thread, COAP

### __Hardware and Software environment__

* This example runs on STM32WBA65xx devices.  

* This example has been tested with an STMicroelectronics STM32WBA65RI_Nucleo board and can be easily tailored to any other supported device and development board.  

### __How to use it?__

In order to make the program work, you must do the following:  
 
- Connect 2 STM32WBA65xx_Nucleo boards to your PC 
- Open your preferred toolchain 
- Rebuild all files of Thread_Coap_Generic and load your image into target memory
- Rebuild all files of Thread_SED_Coap_Multicast and load your image into target memory
- Run the application   
 
**Note:** when LED1, LED2 and LED3 are toggling it is indicating an error has occurred on application. 
 
In a Thread network, nodes are split into two forwarding roles: **Router** or **End Device**.    
The Thread **Leader** is a Router that is responsible for managing the set of Routers in a Thread network.    
An End Device (or child) communicates primarily with a **single Router**.    

In our Application which uses two devices, one device will act as a Leader (Router) and the other one will act as an End Device(mode child). 

After the reset of the Thread_Coap_Generic board will be in Leader mode (**Green LED2 ON**).    
The Thread_SED_Coap_Multicast board will be in Child mode (**Red LED3 ON**) after association.  
 
Then Thread_SED_Coap_Multicast board will send a **COAP command (Non-Confirmable)** every second to Thread_Coap_Generic board.  
The Thread_Coap_Generic board will receive COAP commands to toggle its **blue LED1**.

Thread_SED_Coap_Multicast is designed as low power device, you can switch low power mode in app_conf.h
    
 
<pre>
	
  ___________________________                       ___________________________
  |  Device Coap_SED        |                       | Device Coap_Generic     |
  |_________________________|                       |_________________________|  
  |                         |                       |                         |
  |                         |                       |                         |
  |                         |======> COAP =========>| BLUE LED TOGGLE (ON/OFF)|
  |                         | Resource "light"      |                         |
  |                         | Mode: Multicast       |                         |
  |                         | Type: Non-Confirmable |                         |
  |      Timer 1 sec        | Code: Put             |                         |
  |                         |                       |                         |
  |                         |                       |                         |
  |                         |                       |                         |
  |                         |======> COAP =========>| BLUE LED TOGGLE (ON/OFF)|
  |                         | Resource "light"      |                         |
  |                         | Mode: Multicast       |                         |
  |                         | Type: Non-Confirmable |                         |
  |                         | Code: Put             |                         |  
  ---------------------------                       ---------------------------
  | Role : Child            |                       | Role : Leader           |
  |                         |                       |                         |
  | LED : Red               |                       | LED : Green             |
  |                         |                       |                         |
  |_________________________|                       |_________________________|

  
</pre> 

### __Traces__

* To get the traces you need to connect your Board to the Hyperterminal (through the STLink Virtual COM Port).  
* Low power must be deactivated
* The UART must be configured as follows:  
<br>
BaudRate       = 115200 baud</br>
Word Length    = 8 Bits</br>
Stop Bit       = 1 bit</br>
Parity         = none</br>
Flow control   = none</br>
Terminal   "Go to the Line" : &lt;LF&gt;  





