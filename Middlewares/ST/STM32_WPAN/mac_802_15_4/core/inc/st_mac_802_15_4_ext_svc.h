/**
 ******************************************************************************
 * @file    st_mac_802_15_4_ext_svc.h
 * @author  MCD Application Team
 * @brief   Contains STM32WB specificities requested to debug the 802.15.4
 *          MAC interface.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */

#ifndef _ST_MAC_802_15_4_EXT_SYS_H_
#define _ST_MAC_802_15_4_EXT_SYS_H_
#ifdef A_MAC
#include <stdint.h>
#include "st_mac_802_15_4_sap.h"
#include "st_mac_802_15_4_types.h"
#include "mac_temporary.h"

#define MAX_RTX_DEVICE 1 //Max number of DRx device
#define MAX_DTX_DEVICE 8 //Max number of DTx device

#define TX_ACK_REQUEST    0x61  //Frame control field for ack
#define TX_NOACK_REQUEST  0x01  // frame control field no ack

#define ACK_FCF    0x07 //Ack frame control field

#define EXT_PT_ID   0xEA  //Protocole ID (used for identify Extended Mac Network)

#define BASE_ADDRESS_SIZE    1
#define BASE_ADDRESS_DEFAULT 0xAA  //Base address default when device isn't registered 
#define BASE_ADDRESS_UID_OK  0xBB  //Base address default when device is registered 

#define MAX_UID_ADDRESS      8

//Different Uid Address possible
#define UID_ADDRESS_TAB      {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}

//Index of different element of frame
#define EXT_TX_FRAMECTRL_IDX     0
#define EXT_TX_PTID_IDX          1
#define EXT_TX_SEQNUM_IDX        2
#define EXT_TX_BASEADDRESS_IDX   3
#define EXT_TX_UIDADDRESS_IDX    4
#define EXT_TX_PYLDLEN_IDX       5
#define EXT_TX_PYLD_IDX          6

//Index of different element of ack frame
#define EXT_ACK_FRAMECTRL_IDX    0
#define EXT_ACK_PTID_IDX         1
#define EXT_ACK_SEQNUM_IDX       2
#define EXT_ACK_UIDRESERVED_IDX  3
#define EXT_ACK_PYLDLEN_IDX      4
#define EXT_ACK_PYLD_IDX         5


#define EXT_MAX_TX_LEN 127 // Max Frame Len 
#define EXT_TX_HEADER_LEN 6 // Header frame Len
#define EXT_MAX_TX_PAYLOAD_LEN (EXT_MAX_TX_LEN - EXT_TX_HEADER_LEN) //Payload max Len

#define EXT_MAX_ACK_LEN  127 // Max Ack Frame Len 
#define EXT_ACK_HEADER_LEN 5 // Ack Header Len
#define EXT_MAX_ACK_PAYLOAD_LEN (EXT_MAX_ACK_LEN - EXT_ACK_HEADER_LEN) //Ack Payload max len

#define EXT_CRC_LEN 2 //CRC Len

//DEfault Tx config
#define DEFAULT_TX_CONFIG  {.TxMode = EXT_TX_AUTO, .TxIfs = 192, .TxRetryTime = 512, .TxRetryCount = 3}

//Default DTx config, can be used in init function parameter
#define DEFAULT_DTX_CONFIG {.Mode = EXT_DTX, .AckMode = EXT_ACK_ENABLE, .Bitrate = EXT_RATE_2M, .Channel = 11, .Power = 10, .AckCfg = NULL, .TxCfg = NULL, .base_address = BASE_ADDRESS_DEFAULT, .uid_address = 0x00}
//Default DRx config, can be used in init function parameter
#define DEFAULT_DRX_CONFIG {.Mode = EXT_DRX, .AckMode = EXT_ACK_ENABLE, .Bitrate = EXT_RATE_2M, .Channel = 11, .Power = 10, .AckCfg = NULL, .TxCfg = NULL, .base_address = BASE_ADDRESS_DEFAULT, .uid_address = 0x00}
//Default Acknowledge Config
#define DEFAULT_ACK_CONFIG {.Timeout = 512, .TurnaroundTime = 192, .Len = 0x00}

//Default Tx Frame
#define DEFAULT_TX_FRAME   {.frame_ctrl = TX_ACK_REQUEST, .pt_id = EXT_PT_ID,  .sequence_number = 0x00, .base_address = BASE_ADDRESS_DEFAULT, .uid_address = 0x00, .payload_len = 0x0}

//DEfault Ack Frame
#define DEFAULT_ACK_FRAME  {.frame_ctrl = ACK_FCF, .pt_id = EXT_PT_ID,  .sequence_number = 0x00, .uid_reserved = 0x00, .payload_len = 0x00}

#define TXFIFO_SIZE 5
#define RXFIFO_SIZE 5

#define EXT_SCAN_ALL_CHANNEL_KPBS  0xffff000000000000 //Scan all channel (11 to 26)
#define EXT_SCAN_ALL_CHANNEL_MPBS  0xffffffffff000000 //Scan all channel (11 to 50)
/* Private typedef -----------------------------------------------------------*/
/**
 * @brief Extended Frame struct
 */
typedef struct
{
  /* Header */
  uint8_t frame_ctrl;      //0x01(data) or 0x03(command) or 0x07(extended) |  0x61(dataAckreq) or 0x63(commandAckreq) or 0x67(extendedAckreq) -> Mandatory for radio 
  uint8_t pt_id;           //0xEA
  uint8_t sequence_number; //Sequence number to identify packet and allow retransmission
  uint8_t base_address;    //BASE_ADDRESS_DEFAULT when DTX hasn't UID defined else BASE_ADDRESS_UID_OK
  uint8_t uid_address;     //Unique identificacion device -> must belong to UID_ADDRESS_TAB
  uint8_t payload_len;     //[0,EXT_MAX_TX_PAYLOAD_LEN]
  /* Payload */
  uint8_t payload[EXT_MAX_TX_PAYLOAD_LEN]; //Tx payload
}ST_MAC_ExtFrame;
 
/**
 * @brief Extended Acknwoledge Frame struct
 */
typedef struct
{
  /* Header */
  uint8_t frame_ctrl;      //0x07 -> Mandatory for radio Custom Ack
  uint8_t pt_id;           //0xEA
  uint8_t sequence_number; //Sequence number to identify packet and allow retransmission
  uint8_t uid_reserved;    //Unique identificacion device already reserved
  uint8_t payload_len;     //[0,EXT_MAX_ACK_PAYLOAD_LEN]
  /* Payload */ 
  uint8_t payload[EXT_MAX_ACK_PAYLOAD_LEN]; //Ack payload
}ST_MAC_ExtAckFrame;

/**
 * @brief Extended Rate, Extended Mac is able to work at diffenrent radio rate
 */
typedef enum {
  EXT_RATE_125K,
  EXT_RATE_256K,
  EXT_RATE_1M,
  EXT_RATE_2M
}ST_MAC_ExtBitRate_t;

/**
 * @brief Extended Mode Transmission or Reception
 */
typedef enum  {
  EXT_DTX,
  EXT_DRX
}ST_MAC_ExtMode_t;

/**
 * @brief Extended Transission Mode
 */
typedef enum  {
  EXT_TX_AUTO,
  EXT_TX_MANUAL
}ST_MAC_ExtTxMode_t;

/**
 * @brief Extended Ack mode of the DRx device 
 * EXT_ACK_ENABLE or EXT_ACK_DISABLE
 */
typedef enum {
  EXT_ACK_ENABLE,
  EXT_ACK_DISABLE
}ST_MAC_ExtAckMode_t;

/**
 * @brief Extended Ack config of the device Exammple : 
 * DEFAULT_ACK_CONFIG
 */
typedef struct {
  uint16_t TurnaroundTime;
  uint16_t Timeout;
  uint8_t Pyld[EXT_MAX_ACK_PAYLOAD_LEN];
  uint8_t Len;
}ST_MAC_ExtAckCfg;

/**
 * @brief Extended Mac Tx config of the device Exammple : 
 * DEFAULT_TX_CONFIG
 */
typedef struct {
  ST_MAC_ExtTxMode_t TxMode;
  uint16_t TxIfs;
  uint16_t TxRetryTime;
  uint8_t  TxRetryCount;
}ST_MAC_ExtTxCfg;


/**
 * @brief Extended Mac config of the device Exammple : 
 * DEFAULT_DTX_CONFIG
 * DEFAULT_DRX_CONFIG
 */
typedef struct {
  ST_MAC_ExtMode_t Mode;
  ST_MAC_ExtAckMode_t AckMode;
  ST_MAC_ExtBitRate_t Bitrate;
  uint8_t  Channel;  //rate dependent
  int8_t Power;
  ST_MAC_ExtAckCfg* AckCfg; //NULL if not needed
  ST_MAC_ExtTxCfg* TxCfg;
  uint8_t  uid_address;     //Unique identificacion device -> must belong to UID_ADDRESS_TAB
  uint8_t  base_address;
}ST_MAC_ExtConfig;

/**
 * @brief Current state of the device
 */
typedef enum {
  EXT_NOT_INIT,
  EXT_IDLE,
  EXT_TX,
  EXT_RX
}ST_MAC_ExtState;

/**
 * @brief Struct Callback dispatcher, user will receive this callback in case of 
 * Transmission success or failure for DTx
 * Reception for DRx
 */
typedef struct  {
	void (*ext_tx_success)(void);
	void (*ext_tx_failure)(void);
	void (*ext_rx_done)(uint8_t);
        void (*ext_scan_done)(uint64_t, uint8_t, uint8_t, uint8_t*);
}ST_MAC_ExtCallback_Dispatcher;


/**
 * @fn ST_MAC_ExtInit
 *
 * @brief Init Extended Mac service with given configuration and link user callback
 * Configuration example : DEFAULT_DRX_CONFIG, DEFAULT_DTX_CONFIG
 *
 * @param   *pStExtMacCfg : [in] configuration of extend mac 
 *          * pExtCallbackDispatcher : [in] user callback (ext_tx_success, ext_tx_failure, ext_rx_done)
 *
 * @retval MAC_Status_t
 */
MAC_Status_t ST_MAC_ExtInit(ST_MAC_ExtConfig* pStExtMacCfg, ST_MAC_ExtCallback_Dispatcher* pExtCallbackDispatcher);

/**
 * @fn ST_MAC_ExtSetPower
 *
 * @brief set Tx power in dbm
 *
 * @param   eExtPower : [in] Tx power in dbm (min -20dbm, max 10dbm)
 *
 * @retval MAC_Status_t
 */
MAC_Status_t ST_MAC_ExtSetPower(int8_t eExtPower);

/**
 * @fn ST_MAC_ExtSetRate
 *
 * @brief set radio Rate in Kbps
 *
 * @param   eExtRate : [in] radio in kbps (  EXT_RATE_125K, EXT_RATE_256K, EXT_RATE_1M, EXT_RATE_2M)
 *
 * @retval MAC_Status_t
 */
MAC_Status_t ST_MAC_ExtSetRate(ST_MAC_ExtBitRate_t eExtRate);

/**
 * @fn ST_MAC_ExtSetTxIfs
 *
 * @brief set Tx interframe spacing 
 * (minimal time between 2 transmission at radio level, time between last byte of first frame and first byte of second frame)
 *
 * @param   TxIfs : [in] Tx  interframe spacing (us) min : 140 max 1000
 *
 * @retval MAC_Status_t
 */
MAC_Status_t ST_MAC_ExtSetTxIfs(uint16_t TxIfs);

/**
 * @fn ST_MAC_ExtSetChannel
 *
 * @brief Set radio frequency channel (dependent of radio rate)
 * EXT_RATE_125K, EXT_RATE_256K : min 11 max 26
 * EXT_RATE_1M, EXT_RATE_2M : min 11 max 50

 * @param   uExtChannel : [in] channel
 *
 * @retval MAC_Status_t
 */
MAC_Status_t ST_MAC_ExtSetChannel(uint8_t uExtChannel);

/**
 * @fn ST_MAC_ExtSetAck
 *
 * @brief Set ack mode : Custom Ack or Mac Ack. Only custom ack supported for the moment
 *
 * @param   eAckMode : [in] ST_MAC_ExtAckMode_t Enable or disable
 *         *pCustomAckcfg : [in] Ack config example DEFAULT_ACK_CONFIG
 *
 * @retval MAC_Status_t
 */
MAC_Status_t ST_MAC_ExtSetAck(ST_MAC_ExtAckMode_t eAckMode, ST_MAC_ExtAckCfg* pCustomAckcfg);

/**
 * @fn ST_MAC_ExtStartStopAck
 *
 * @brief Stop or Start acknowledge for DRx device
 *
 * @param   eAckMode : [in] ST_MAC_ExtAckMode_t Enable or disable
 *
 * @retval MAC_Status_t
 */
MAC_Status_t ST_MAC_ExtStartStopAck(ST_MAC_ExtAckMode_t eAckMode);

/**
 * @fn ST_MAC_ExtPushTxFIFO
 *
 * @brief Push Frame in Tx FIFO, 
 *      if EXT_TX_AUTO is set then the frame will be send as soon as possible
 *      else user needs to call ST_MAC_ExtStartTx()
 *
 * @param   *Payload : [in] Payload to send 
 *          Len      : [in] Size of the payloas
 *          eAck     : [in] ST_MAC_ExtAckMode_t Frame must be acknowledge by DRx or not
 * @retval MAC_Status_t
 */
MAC_Status_t ST_MAC_ExtPushTxFIFO(uint8_t* Payload, uint8_t Len, ST_MAC_ExtAckMode_t eAck);

/**
 * @fn ST_MAC_ExtStartTx
 *
 * @brief Use to start transmission if EXT_TX_MANUAL is set
 *
 * @param  void
 *
 * @retval MAC_Status_t
 */
MAC_Status_t ST_MAC_ExtStartTx(void);

/**
 * @fn ST_MAC_ExtStartRx
 *
 * @brief Use to start reception for DRx device
 *
 * @param  void
 *
 * @retval MAC_Status_t
 */
MAC_Status_t ST_MAC_ExtStartRx(void);

/**
 * @fn ST_MAC_ExtStopRx
 *
 * @brief Use to stop reception for DRx device
 *
 * @param  void
 *
 * @retval MAC_Status_t
 */
MAC_Status_t ST_MAC_ExtStopRx(void);

/**
 * @fn ST_MAC_GetState
 *
 * @brief Get state of Device
 *
 * @param  void
 *
 * @retval ST_MAC_ExtState (EXT_NOT_INIT, EXT_IDLE, EXT_TX, EXT_RX)
 */
ST_MAC_ExtState ST_MAC_GetState(void);

/**
 * @fn ST_MAC_ExtPopRxFIFO
 *
 * @brief Get out data of RxFIFO, you must give the uid of the DTx device
 *
 * @param   uid : [in] uid of the device you want to pop the data out the RxFIFO
 *
 * @retval ST_MAC_ExtFrame* Rx Frame 
 */
ST_MAC_ExtFrame* ST_MAC_ExtPopRxFIFO(uint8_t uid);

/**
 * @fn ST_MAC_ExtStartScan
 *
 * @brief Use to start scan on different channel for DRx device, 
 * Scan callback function will be called when scan is finished
 *
 * @param  channel_mask : (uint64_t) mask must significant bit correspond to channel 11
 * example : 0x9010000000000000 -> channel 11, 14, 22: 
 *           EXT_SCAN_ALL_CHANNEL_KPBS = 0xffff000000000000 -> channel 11 to 26
 *           EXT_SCAN_ALL_CHANNEL_MPBS = 0xffffffffff000000 -> channel 11 to 50
 *          
 * @param ScanDurationChannel : Scanning time per channel (us) (increase Scanning time increase accuracy)

 * @retval MAC_Status_t
 */
MAC_Status_t ST_MAC_ExtStartScan(uint64_t channel_mask, uint32_t ScanDurationChannel);

#endif /*  A_MAC */
#endif /* _ST_MAC_802_15_4_EXT_SYS_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/