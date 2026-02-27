
## __Thread_Cli_Cmd_MTD Application Description__

How to control the MTD (Minimal Thread Device) stack via Cli commands.

### __Keywords__

Connectivity, 802.15.4 protocol, Thread, CLI 

### __Hardware and Software environment__

* This example runs on STM32WBA55xx devices.  

* This example has been tested with an STMicroelectronics STM32WBA55CGA_Nucleo board and can be easily tailored to any other supported device and development board.  

### __How to use it?__
  
The Cli (Command Line Interface) commands are sent via an UART from an HyperTerminal (PC) to the STM32WBA55xx Nucleo Board.  
       
 
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
  |          |USART2          |                      |     HyperTerminal 2 (Optional)       |
  |          |                |                      |=> Used to display traces             |   
  |          |                |                      |                                      |
  |          |CN4 (Pin37) RX  |______________________|TX                                    | 
  |          |                |                      |                                      |
  |          |                |     RS232 Cable      |                                      |             
  |          |                |                      |                                      |
  |          |CN4 (Pin35) TX  |______________________|RX                                    |          
  |          |                |                      |                                      |           
  |          |________________|                      |______________________________________|          
  |                           |                                                
  |                           |                      
  |                           |                      
  |_STM32WBA55xx_Nucleo_______|                      
                 

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
If you are using a single board running the Thread_Cli_Cmd_MTD application, after entering the commands above (panid, channel, etc.), typing state should return detached. 
This indicates that the device has created a new Thread network and is acting as its detached. Be careful that with the Thread_Cli_Cmd_MTD application alone, it is not possible to create a Thread network, and the device will not be able to reach the state 'leader' 

```
   >state
   detached
   Done
```

b) Adding a Second Board
To add another device to the network:
Flash a second board with the Thread_Cli_Cmd_FTD application.
Enter the same sequence of commands as above (panid 0x1234, channel 12, ifconfig up, networkkey 00112233445566778899AABBCCDDEEFF, thread start) on the second board.
After completing these steps, type 'state' on both devices. FTD board should return 'leader' and MTD should return 'child' or 'router', indicating that it has successfully joined the network created by the FTD board.
 
### __Traces__

* To get the traces you need to connect your Board to the Hyperterminal (through the STLink Virtual COM Port).  

* The UART must be configured as follows:  
<br>
BaudRate       = 115200 baud</br>
Word Length    = 8 Bits</br>
Stop Bit       = 1 bit</br>
Parity         = none</br>
Flow control   = none</br>
Terminal   "Go to the Line" : LF 
 


:::
:::

