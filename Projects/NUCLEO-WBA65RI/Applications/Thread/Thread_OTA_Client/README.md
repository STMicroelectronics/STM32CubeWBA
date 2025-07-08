## __Thread_OTA_Client Application Description__

How to update Over The Air (OTA) FW application and Copro Wireless binary using Thread (Server side).

### __Keywords__

Connectivity, 802.15.4 protocol, Thread, OTA, COAP

### __Hardware and Software environment__

* This example runs on STM32WBA65xx devices.  

* This example has been tested with an STMicroelectronics STM32WBA65RI_Nucleo board and can be easily tailored to any other supported device and development board.  

### __How to use it?__

In order to make the program work, you must do the following:  
 
- Connect 2 STM32WBA65xx_Nucleo boards to your PC 
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
 
This Thread application provides implementation example for Updating FW on Application
side and on Copro Wireless side.
Thread_OTA_Client can only communicate with Thread_OTA_Server. 



Following diagram describes the Update procedure:

<pre>

  ____________________________________                                 ______________________________________          
  |  Device 1                         |                                | Device 2                            |
  |  Thread_OTA_Server                |                                | Thread_OTA_Client                   |
  |___________________________________|                                |_____________________________________|
  |                                   |                                |                                     |
  | Before starting this example,     |                                | At startup, Application can performs|
  | the Thread_Coap_Generic_Ota FW    |                                | a delete of FLASH memory sectors    |
  | must be written at following      |                                | starting from @ = 0x08100000 to SFSA|
  | @ = 0x08100000                    |                                | limit by pressing on SW3            |
  |                                   |                                |                                     |  
  |                                   |                                |                                     |  
  | Get Mesh-Local EID                |                                |                                     |
  |  (Endpoint Identifier) of         |                                |                                     |
  |  Thread_OTA_Client device         |                                |                                     |
  |                                   |                                |                                     |
  | Push Button  ----->               |                                |                                     |                          
  |  - SW1 (OTA for APP Update)       |                                |                                     |
  |                                   |                                |                                     |
  |                                   | ====== COAP REQUEST =========> |                                     |
  |                                   | Resource: "FUOTA_PROVISIONING" |                                     |
  |                                   | Mode: Multicast                |                                     |
  |                                   | Type: Confirmable              |     returns Mesh-Local EID          |
  |                                   | Code: Get                      |                                     |
  |                                   | Payload  : OtaContext          |                                     |
  |                                   |                                |                                     |
  |Thread_OTA_Client Mesh-Local       | <=====COAP CONFIRMATION ====== |                                     |
  |         EID received              |                                |                                     |
  |                                   |                                |                                     |
  |                                   |                                |                                     |
  | Send FUOTA parameters:            | ============> COAP =========>  |                                     |
  |   - Base address for the download | Resource: "FUOTA_PARAMETERS"   |                                     |
  |   - Magic Keyword                 | Mode: Unicast                  |    Saves FUOTA parameters           |
  |                                   | Type: Confirmable              |     and confirms                    |
  |                                   | Code: Put                      |                                     |
  |                                   |                                |                                     |
  |   Waits for confirmation          | <=====COAP CONFIRMATION ====== |                                     |
  |                                   |                                |                                     |
  | Once confirmation received        |                                |                                     |
  |   ->Starts FUOTA data transfer    |                                |                                     |
  |                                   |                                |                                     |
  |                                   |                                |                                     |
  |--> Data Transfer                  | ============> COAP =========>  |    BLUE LED TOGGLING                |
  ||   (400 bytes Payload)            | Resource: "FUOTA_SEND"         |                                     |
  ||                                  | Mode: Unicast                  |                                     |
  ||                                  | Type: Confirmable              |   Each time data buffer is received |
  ||                                  | Code: Put                      |    writes it to FLASH memory        |
  ||                                  | Payload  : Buffer[]            |                                     |
  ||                                  |                                |                                     |
  ||            Ack received          | <=====COAP CONFIRMATION ====== |                                     |
  ||                |                 |                                |                                     |
  ||                V                 |                                |                                     |
  ||                /\                |                                |                                     |
  ||               /  \               |                                |                                     |
  ||              /    \              |                                |                                     |
  ||             /      \             |                                |                                     |
  ||            /        \            |                                |                                     |
  ||   NO      / END OF   \           |                                |                                     |
  | --------- /  TRANSFER? \          |                                |                                     |
  |           \  (MAGIC    /          |                                |                                     |
  |            \  KEYWORD /           |                                |                                     |
  |             \ FOUND) /            |                                |                                     |
  |              \      /             |                                |                                     |
  |               \    /              |                                |                                     |
  |                \  /               |                                |                                     |
  |                 \/                |                                |                                     |
  |             YES |                 |                                |                                     |
  |                 |                 |                                |                                     |
  |                 V                 |                                |                                     |
  |  Display FUOTA Transfer details:  |                                |    At the end of data transfer      |
  |    - Payload size used            |                                |     - BLUE LED OFF                  |
  |    - Transfer Time                |                                |     - Reboot the application        |
  |    - Average Throughput           |                                |                                     |
  |                                   |                                |                                     |
  |                                   |                                | On Reboot:                          |
  |                                   |                                | -if there is a new valid image(     |
  |                                   |                                |   Thread_Coap_Generic_Ota or any    | 
  |                                   |                                |   application supporting Ota at     | 
  |                                   |                                |   @ 0x08100000) the system will boot| 
  |                                   |                                |   on this new image, otherwise, it  | 
  |                                   |                                |   will boot on the Thread_OTA_Client| 
  |                                   |                                |   application.                      |
  |                                   |                                |                                     |
  |                                   |                                |                                     |
  |                                   |                                |                                     |
  |                                   |                                |                                     | 
  |                                   |                                |                                     | 
  |                                   |                                |                                     |  
  --------------------------------------                               ---------------------------------------     
  | Role : Leader                     |                                | Role : Child                        |
  |                                   |                                |                                     |
  | LED : Green                       |                                | LED : Red                           |
  |                                   |                                |                                     |
  |___________________________________|                                |_____________________________________|
       

  
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





