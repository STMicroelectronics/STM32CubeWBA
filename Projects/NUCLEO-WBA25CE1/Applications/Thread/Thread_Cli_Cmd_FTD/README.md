
## __Thread_Cli_Cmd_FTD Application Description__

How to control the FTD (Full Thread Device) stack via Cli commands.

### __Keywords__

Connectivity, 802.15.4 protocol, Thread, CLI 

### __Hardware and Software environment__

* This example runs on STM32WBA25xx devices.  

* This example has been tested with an STMicroelectronics STM32WBA25CGA_Nucleo board and can be easily tailored to any other supported device and development board.  

### __How to use it?__
  
The Cli (Command Line Interface) commands are sent via an UART from an HyperTerminal (PC) to the STM32WBA25xx Nucleo Board.  
       
 
<pre>
    ___________________________ 
  |                           |     
  |           ________________|                       _____________________________________
  |          |USART1          |                      |     HyperTerminal 1                 |
  |          |                |                      |=> Used to ctrl the stack via Cli Cmd|
  |          |                |                      |                                     |
  |          |                |                      |                                     |
  |          |                |______________________|                                     |
  |          |                |______________________| ST_Link virtual port                |
  |          |                |     ST_Link Cable    |                                     |             
  |          |                |                      |                                     |
  |          |                |                      |                                     |
  |          |                |                      |                                     |             
  |          |________________|                      |_____________________________________|          
  |                           |                       
  |           _______________ |                       ______________________________________
  |          |LPUART1         |                      |     HyperTerminal 2 (Optional)       |
  |          |                |                      |=> Used to display traces             |   
  |          |                |                      |                                      |
  |          |                |______________________|                                      | 
  |          |                |                      |                                      |
  |          |                |     ST_Link Cable    | ST_Link virtual port                 |             
  |          |                |                      |                                      |
  |          |                |______________________|                                      |          
  |          |                |                      |                                      |           
  |          |________________|                      |______________________________________|          
  |                           |                                                
  |                           |                      
  |                           |                      
  |_STM32WBA25xx_Nucleo_______|                      
                 

</pre>  


- Through the Cli commands, it is possible to control and configure the Thread stack.
- On the HyperTerminal, the user can type the command 'help' in order to display the list of 
all available cli commands. Additional information can be found looking at the OpenThread 
web site: [https://openthread.io/guides/](https://openthread.io/guides/) 

- As an example, the user can play the following scenario in order to properly initiate the Thread mesh network by typing the following commands: 
```
  >panid 0x1234
   Done
  >channel 12
   Done
  >ifconfig up
   Done
  >networkkey 00112233445566778899AABBCCDDEEFF
  >thread start
   Done
```  

At this point, you can check the device’s current role by entering the command: 'state':

a) Single Board Scenario
If you are using a single board running the Thread_Cli_Cmd_FTD application, after entering the commands above (panid, channel, etc.), typing state should return leader. 
This indicates that the device has created a new Thread network and is acting as its leader. Be careful that with the Thread_Cli_Cmd_MTD application alone, it is not possible to create a Thread network, and the device will not be able to reach the state 'leader' 

```
   >state
   leader
   Done
```

b) Adding a Second Board
To add another device to the network:
Flash a second board with either the Thread_Cli_Cmd_FTD or Thread_Cli_Cmd_MTD application.
Enter the same sequence of commands as above (panid 0x1234, channel 12, ifconfig up, networkkey 00112233445566778899AABBCCDDEEFF, thread start) on the second board.
After completing these steps, type 'state' on the second device. It should return 'child' or 'router', indicating that it has successfully joined the network created by the first board.
 
### __Traces__

* To get the traces you need to connect your Board to the Hyperterminal (through the STLink Virtual COM Port 2).  

* The UART must be configured as follows:  
<br>
BaudRate       = 115200 baud</br>
Word Length    = 8 Bits</br>
Stop Bit       = 1 bit</br>
Parity         = none</br>
Flow control   = none</br>
Terminal   "Go to the Line" : LF 
 
Caution: to get LPUART1 mapped on STLink VCP2, you must follow these steps:
- on MB2293 board: SB4, SB35, SB38, SB43 to be fitted
- option byte nSWBOOT0 must be unchecked (in user configuration) with CubeProgrammer

:::
:::

