## __Thread_Udp Application Description__

How to transfer data using UDP.

### __Keywords__

Connectivity, 802.15.4 protocol, Thread, UDP.

### __Hardware and Software environment__

* This example runs on STM32WBA55xx devices.  

* This example has been tested with an STMicroelectronics STM32WBA55CGA_Nucleo board and can be easily tailored to any other supported device and development board.  

### __How to use it?__

In order to make the program work, you must do the following:  
 
- Connect 2 STM32WBA55xx_Nucleo boards to your PC 
- Open your preferred toolchain 
- Rebuild all files and load your image into target memory
- Run the application   
 
**Note:** when LED1, LED2 and LED3 are toggling it is indicating an error has occurred on application. 

To get the traces in real time, you can connect an HyperTerminal to the STLink Virtual Com Port.

Debug Traces are available on USART1 (STLink Virtual Com Port):

    /**USART1 GPIO Configuration
    PB12     ------> USART1_TX
    PA8     ------> USART1_RX
    */
    115200 bps, data 8 bit, parity none, stop bit 1, flow control none


Thread CLI is available on LPUART:

    /**LPUART1 GPIO Configuration    
    PA2     ------> LPUART1_TX
    PA1     ------> LPUART1_RX 
    */
    115200 bps, data 8 bit, parity none, stop bit 1, flow control none 
 
It demonstrates the ability to transfer a block of data through UDP.
 
Nodes are split into two forwarding roles: Router or End Device.
The Thread Leader is a Router that is responsible for managing the set of Routers in a 
Thread network. An End Device (or child) communicates primarily with a single Router. 

In our Application, which uses two devices, one device will act as a Leader (Router) 
and the other one will act as an End Device (child mode)
 
Test 1:
After the reset of the 2 boards, one board will be in Leader mode (Green LED2 ON) and 
the other one will be in Child mode (Red LED3 ON).
Once the child mode is established for one of the devices, the user pushes SW2 button
to start a UDP sending procedure in multicast mode.
If the pattern is correctly received on the Leader side Blue LED toggles its previous
state (ON if it was OFF and OFF if it was ON).

Test 2:
Note: The same test can be performed from Leader to Child by pushing SW1 button 
 
<pre>

-Test 1
  ___________________________                       ___________________________
  |  Device 1               |                       | Device 2                |
  |_________________________|                       |_________________________|  
  |                         |                       |                         |
  |                         |                       |  UDP open socket        |
  |                         |                       |  UDP bind to UDP_PORT   |
  |                         |                       |                         |
  |                         |<= Push Sw2            |                         |
  |                         |                       |                         |
  |     UDP send()          |======> UDP =========> |-------------            |
  |                         |UDP_PORT "1234"        |             |           |
  |                         |Address : ff02::1      |             |           |
  |                         |Payload: udpBuffer[]   |             |           |
  |                         |                       |             v           |
  |                         |                       |        UDP Receive ()   |
  |                         |                       |             |           |
  |                         |                       |             |           |
  |                         |                       |             v           |
  |                         |                       |        Blue Led TOGGLES |
  |                         |                       |                         |
  ---------------------------                       ---------------------------
  | Role : Child            |                       | Role : Leader           |
  |                         |                       |                         |
  | LED : Red               |                       | LED : Green             |
  |                         |                       |                         |
  |_________________________|                       |_________________________|


-Test 2
  ___________________________                       ___________________________
  |  Device 2               |                       | Device 1                |
  |_________________________|                       |_________________________|  
  |                         |                       |                         |
  |                         |                       |  UDP open socket        |
  |                         |                       |  UDP bind to UDP_PORT   |
  |                         |                       |                         |
  |                         |<= Push Sw1            |                         |
  |                         |                       |                         |
  |     UDP send()          |======> UDP =========> |-------------            |
  |                         |UDP_PORT "1234"        |             |           |
  |                         |Address : ff02::1      |             |           |
  |                         |Payload: udpBuffer[]   |             |           |
  |                         |                       |             v           |
  |                         |                       |        UDP Receive ()   |
  |                         |                       |             |           |
  |                         |                       |             |           |
  |                         |                       |             v           |
  |                         |                       |        Blue Led TOGGLES |
  |                         |                       |                         |
  ---------------------------                       ---------------------------
  | Role : Leader           |                       | Role : Child            |
  |                         |                       |                         |
  | LED : Green             |                       |    LED : Red            |
  |                         |                       |                         |
  |_________________________|                       |_________________________|
  
</pre> 

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





