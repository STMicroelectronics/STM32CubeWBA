## __Thread_Commissioning Application Description__

How to build Thread application based on Coap messages.

### __Keywords__

Connectivity, 802.15.4 protocol, Thread, COAP

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

If you want to control this application, you can directly send and retrieve Cli commands connecting an HyperTerminal with the FTDI cable as following :  

- TX GPIO 17  
- RX GPIO 18  
 
In a Thread network, nodes are split into two forwarding roles: **Router** or **End Device**.    
The Thread **Leader** is a Router that is responsible for managing the set of Routers in a Thread network.    
An End Device (or child) communicates primarily with a **single Router**.    

In our Application which uses two devices, one device will act as a Leader (the Commissioner) and the other one will act as an End Device(the Joiner). 

 Step 1 : At start both devices are not attached to any Thread network. LED1 (blue) is 
          toggling slowly waiting for a user action. Both devices are in 'disabled' mode.

 Step 2 : On one device (Device1) , press on the SW1 push button. Following this action, Device1 will
          reach the 'leader' state inside the Thread network after a few seconds and act as commissioner.
          LED2 (green) should light on. 

          Note : Device1 behaves as if the user had typed the following Cli commands once it has reached
                 the 'leader' state.
                  > commissioner start
                  > commissioner joiner add * MY_PASSWD
                 The * wildcard allows any Joiner with the specified Joiner Credential to commission onto the network. 
                 The Joiner Credential is used (along with the Extended PAN ID and Network Name) to generate the 
                 Pre-Shared Key for the Device (PSKd). The PSKd is then used to authenticate a device during Thread 
                 commissioning.

 Step 3 : On the other device (Device2), press on the SW3 push button. Following this action, Device2 will
          act as joiner using the same credential as specified on the commissioner.
           
          Note : Device2 behaves as if the user had typed the following Cli command: 
                  > joiner start MY_PASSWD

 Step4 : The commissioning is now proceeding. This step can take more than 100 seconds. LED1 (blue) is 
         toggling quickly on Device2.

 Step5 : Once the commissioning is completed, the LED1 (blue) stays 'On' on both devices and the Thread network 
         is now established. One device acts as a Leader (Green LED on) and the other device acts as a Child (Red LED on)  
 
<pre>
	
  ___________________________                       ___________________________
  |  Device A               |                       | Device B                |
  |_________________________|                       |_________________________|  
  |   Blue LED Toggling     |                       |   Blue LED Toggling     |
  |                         |                       |                         |
  |                SW1 -->  |== commissioner start= | BLUE LED TOGGLE (ON/OFF)|
  |                         |                     | |                         |
  |   "Leader" state        |<====================| |                         |
  |   Green LED ON          |                       |                         |
  |                         |                       |                         |
  |                         |                       |                         |
  |                         |                       |                         |
  |                         |                       |                         |
  |   Commissiong  <--|<=== Joiner Start =====| <-- SW 3                      |
  |   Completed             |                       |  BLUE LED Fast Toggling |
  |          |              |                       |                         |
  |           ------------->|=====================> |  Joined the Network     |
  |   Blue LED ON           |                       |  Blue LED ON            |
  |   Green LED ON          |                       |  Red LED ON             |
  |                         |                       |                         |  
  ---------------------------                       ---------------------------
  | Role : Commissioner     |                       | Role : Joiner           |
  |                         |                       |                         |
  | LED : Green             |                       | LED : Red               |
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





