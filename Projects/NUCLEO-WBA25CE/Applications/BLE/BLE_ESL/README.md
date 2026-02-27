## __BLE_ESL Application Description__

This example demonstrates how to use an STM32WBA2 as Electronic Shelf Label device using Periodic Advertising with Responses (PAwR) feature. In particular,
it implements the ESL role of the standard profile (Electronic Shelf Label, aka ESL).

Each ESL device is addressed by a Group ID (7-bit value) and an ESL ID (8-bit value). Addresses are assigned by the ESL Access Point (AP) during the
configuration phase of the node through the provisioning procedure. The ESL application starts as a simple Peripheral advertising. Once an ESL Access
Point device connects to the Peripheral ESL, the ESL is provisioned with the information required to synchronize with the Access Point. This information
consists of ESL address, key materials and absolute time. Using the Periodic Advertising Sync Transfer procedure, the ESL can synchronize with the periodic
advertising train and then disconnect the link. In particular, the device synchronizes only with the PAwR subevent corresponding to the group (e.g. ESL in
group 0 is synchronized only with subevent 0). Once synchronized with the PAwR train, the ESL device can receive and execute ESL commands.

The ESL can store some images in Flash. Images can be transferred from to the AP using the Object Transfer Profile.
By default a maximum of three images can be saved.

The following commands can be used for tests (working only if CFG_LPM_SUPPORTED is 0):
 - *ABSTIME*: Get Current Absolute Time
 - *SRVNEEDED*: Set Service Needed bit to True
 - *UNSYNC*: Set The ESL state to Unsynchronized

### __Keywords__
  
Connectivity, BLE, BLE protocol, BLE PAwR, ESL

### __Hardware and Software environment__
  
  - This application runs on STM32WBA2 Nucleo board.
  - Another STM32WBA2 Nucleo board is necessary to run BLE_ESL_AP application.

### __How to use it?__
  
In order to make the program work:  

 - Open the project with your preferred toolchain
 - Rebuild all files and load your image into target memory (erase all the Flash before downloading, to initialize sectors used to store images)
 - Run the example.
 - Optionally, a serial terminal (with settings 115200-8-N-1), can be opened to see some info.
 - Use another Nucleo board running BLE_ESL_AP.
 - Add the ESL to the network by launching the appropriate command on the AP (e.g. AT+ADD).
 - Once the BLE_ESL has been synchronized, ESL commands can be sent from the AP by specifying the assigned GROUP_ID and ESL_ID.

For more info on how to send commands from the AP, see the related documentation.

### __Notes__

ATTENTION: ESL removes any bonding information after a reset is performed and it becomes unassociated. This is required because the absolute time set by the AP is lost.

