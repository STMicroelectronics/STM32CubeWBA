## __BLE_ESL_AP Application Description__

This example demonstrates how to use an STM32WBA2 to communicate with Electronic Shelf Label devices using Periodic Advertising with Responses (PAwR) feature. 
In particular, it implements the Access Point role of the standard profile (Electronic Shelf Label, aka ESL).

Each ESL is addressed by a Group ID and an ESL ID. Once connected to the Peripheral, the AP configures the ESL by writing its ESL ID and assigning it to a group. Then the Periodic Advertising Synchronization Transfer (PAST) procedure is started to let the Peripheral synchronize with the PAwR train, without the need to do an energy-intensive scan.
Once the procedure is completed, the link can be disconnected.

To control the AP, an AT-like command interface is used.
Each command ends with \<CR\>. 
An "OK" or "ERROR" is given after a command is received. When a response is received from the peer, this is notified on the terminal with a string like 
*+\<RESP\>:\<group_id\>,\<esl_id\>,\<status\>\[,\<return_params\>\]*. 
Commands can be sent in broadcast with all the ESLs in the group, by specifying 0xFF as ESL ID.  

*ATE* can be typed to enable local echo.

The number of groups is equal to the number of PAwR subevents, which can be configured with PAWR_NUM_SUBEVENTS macro in *app_conf.h* (default is 4). The value that can be specified for \<group_id\> must be a value between 0 and (PAWR_NUM_SUBEVENTS - 1). Only for \<esl_id\> there is the possibility to use the value FF for broadcast messages.

The following AT commands can be used to send standard ESL commands (see Electronic Shelf Label Service specification):

- *AT+PING=\<group_id\>,\<esl_id\>*: Ping
- *AT+UNASSOC=\<group_id\>,\<esl_id\>*: Unassociate from AP
- *AT+SRVRST=\<group_id\>,\<esl_id\>*: Service Reset
- *AT+FRST*: Factory Reset (for the connected ESL)
- *AT+UPDCMP*: Update Complete (for the connected ESL)
- *AT+SENS=\<group_id\>,\<esl_id\>,\<sensor_index\>*: Read Sensor Data
- *AT+REFRESH=\<group_id\>,\<esl_id\>,\<display_index\>*: Refresh Display 
- *AT+IMG=\<group_id\>,\<esl_id\>,\<display_index\>,\<image_index\>*: Display Image
- *AT+IMGTIM=\<group_id\>,\<esl_id\>,\<display_index\>,\<image_index\>,\<absolute_time\>*: Display Timed Image
- *AT+LED=\<group_id\>,\<esl_id\>,\<led_index\>,\<led_component_hex\>,\<pattern_hex\>,\<off_period\>,\<on_period\>,\<repeat_type\>,\<repeat_duration\>*: LED Control
- *AT+LEDTIM=\<group_id\>,\<esl_id\>,\<led_index\>,\<led_component_hex\>,\<pattern_hex\>,\<off_period\>,\<on_period\>,\<repeat_type\>,\<repeat_duration\>,\<absolute_time\>*: LED Timed Control

In addition to the previous list of AT commands, the following AT commands can be used to send proprietary ESL commands:

- *AT+TXT=\<group_id\>,\<esl_id\>,\<text\>*: Set text
  - \<text\> can be a string with maximum 15 characters
- *AT+PRICE=\<group_id\>,\<esl_id\>,\<price\>*: Set price

The following AT commands can be used to perform special operations and for tests:

- *AT+SCAN*: Scan for ESLs
- *AT+ADD=\<addr_type>,\<address>,\<group_id>,\<esl_id>*: Perform ESL device provisioning
- *AT+CONN=\<group_id\>,\<esl_id\>*: Connect to an ESL (ESL enters *updating state*) 
- *AT+RECONF=\<new_group_id\>,\<new_esl_id\>*: Reconfigure the connected ESL with a new address
- *AT+INFO*: Read all the Information Characteristics from the connected ESL
- *AT+DISPLAYINFO*: Read the Display Information Characteristic from the connected ESL
- *AT+SENSORINFO*: Read the Sensor Information Characteristic from the connected ESL
- *AT+LEDINFO*: Read the LED Information Characteristic from the connected ESL
- *AT+CLRNVM*: Delete all bonding information
- *AT+ABSTIME?*: Read current absolute time
- *AT+OTPSEARCH*: Discover images on the connected server on the ESL
- *AT+OTPSEARCH=\<name\>*: Search and select the specified image on the connected ESL
- *AT+OTPMETA*: Read metadata for current object
- *AT+OTPSTART=\<truncate\>*: Open an L2CAP channel to transfer an image to the connected ESL. Set \<truncate\> to 1 to truncate image, otherwise set it to 0.
- *AT+OTPWRITE=\<size\>*: Send image data, up to the given size in bytes (maximum is 5000). Data to be sent is stored inside **image.c** file.
- *AT+OTPCLOSE*: Close L2CAP channel to transfer image data. It should be issued when there are not other images to be sent.
- *AT+HELP*: List of AT commands

For each AT command, an ESL command is queued and sent as soon as possible with PAwR. 
The AP can also write commands to the ESL Control Point (ECP) characteristic of a connected ESL (while in *Updating state*).

### __Keywords__
  
Connectivity, BLE, BLE protocol, BLE PAwR, ESL

### __Hardware and Software environment__
  
  - This application runs on STM32WBA2 Nucleo board.
  - At least one additional STM32WBA2 Nucleo board is necessary to run BLE_ESL application.

### __How to use it?__
  
In order to make the program work:  

 - Open the project with your preferred toolchain.  
 - Rebuild all files and load your image into target memory  (erase all the Flash before downloading, to initialize sectors used to store ESL database).  
 - Open a serial terminal (with settings 115200-8-N-1 and \<CR\> transmission at end of line).  
 - Reset the board to run the example.  
 - Run BLE_ESL on another Nucleo board.  
 - Launch *ATE* to enable echo.  
 - Launch *AT+SCAN* to discover the ESL: something like *+SCAN: 0,0280E1AA0001* should be printed on the terminal.  
 - Start ESL provisioning, e.g. with *AT+ADD=0,0280E1AA0001,1,2*, where 1 is the group ID and 2 is the ESL ID.  
 - Once provisioning is completed, ESL disconnects. Commands can be sent on the terminal by specifying the assigned GROUP_ID and ESL_ID to:  
   - ping the board  
   - control an LED  
   - read some sensor data  
   - set an image (only for testing purpose: images cannot be displayed on Nucleo boards)  
 - Data can be sent with Object Transfer Profile to provide an image:  
   - Connect to an associated ESL, e.g. with *AT+CONN=1,2*.  
   - Select an image to be written by providing an image name, e.g. *AT+OTPSEARCH=Image 1*.  
   - Open channel to write data with *AT+OTPSTART=0*.  
   - Send data with *AT+OTPWRITE=5000*.  
   - Close channel with AT+OTPCLOSE.  
   - Close connection and put ESL back to synchronized state with *AT+UPDCMP*.  

