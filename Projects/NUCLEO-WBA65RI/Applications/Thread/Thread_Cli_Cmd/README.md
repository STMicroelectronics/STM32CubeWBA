
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## __Thread_Cli_Cmd Application Description__

How to control the Thread stack via Cli commands.

### __Keywords__

Connectivity, 802.15.4 protocol, Thread, CLI 

### __Hardware and Software environment__

* This example runs on STM32WBA65xx devices.  

* This example has been tested with an STMicroelectronics STM32WBA65RI_Nucleo board and can be easily tailored to any other supported device and development board.  

### __How to use it?__
  
The Cli (Command Line Interface) commands are sent via an UART from an HyperTerminal (PC) to the STM32WBA65xx Nucleo Board.  
       
 
<pre>
   ___________________________ 
  |                           |                               
  |           ________________|                                _____________________________________
  |          |USART1          |                               |     HyperTerminal 1                 |
  |          |                |                               |=> Used to ctrl the stack via Cli Cmd|
  |          |                |                               |                                     |
  |          |                |                               |                                     |
  |          |                |_______________________________|                                     |
  |          |                |______________________   ______| ST_Link virtual port                |
  |          |                |     ST_Link Cable    | |      |                                     |             
  |          |                |                      | |      |                                     |
  |          |                |                      | |      |                                     |
  |          |                |                      | |      |                                     |             
  |          |________________|                      | |      |_____________________________________|          
  |                           |                      | |       
  |           _______________ |                      | |       _____________________________________
  |          |USART2          |                      | |      |     HyperTerminal 2 (Optional)      |
  |          |                |                      | |______|=> Used to display traces            |   
  |          |                |                      |________|                                     |
  |          |                |                               |                                     | 
  |          |                |                               |                                     |
  |          |                |                               |                                     |             
  |          |                |                               |                                     |
  |          |                |                               |                                     |          
  |          |                |                               |                                     |           
  |          |________________|                               |_____________________________________|          
  |                           |                                                         
  |                           |                      
  |                           |                      
  |_STM32WBA65xx_Nucleo_______|                      
                 

</pre>  


- Through the Cli commands, it is possible to control and configure the Thread stack.
- On the HyperTerminal 1, the user can type the command 'help' in order to display the list of 
all available cli commands. Additional information can be found looking at the OpenThread 
web site: [https://openthread.io/guides/](https://openthread.io/guides/) 
- On the HyperTerminal 2, traces are displayed.  
If you don't have the second HyperTerminal automatically, go to STM32CubeProgrammer and upgrade the firmware.  
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

At this point, the user can check the state of its device by using the cli command 'state':
```
  >state
   leader
   Done
```
**Note:** With **MTD** config, we have only Child role.  

- When running on two STM32WBA65xx Nucleo boards the same Thread_Cli_Cmd application, and by playing the same scenario as described above on both boards, the first board should reach the state 'leader', while the second one should reach the state 'child'. 
 
  
**Note:** The application needs to ensure that the SysTick time base is always set to 1 millisecond to have correct HAL operation.    
  

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

