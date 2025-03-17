/*
 * CANDriver.h
 *
 *  Created on: Aug 6, 2022
 *      Author: mingye chen
 *      Forked from
 */

#ifndef USERDRIVERS_CANDRIVER_H_
#define USERDRIVERS_CANDRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "ApplicationTypes.h"
#include "UOSMCoreConfig.h"


typedef union {
    struct {
        uint8_t idType;
        uint32_t id;
        uint8_t dlc;
        uint8_t data[8];
    } frame;
    uint8_t array[14];
} uCAN_MSG;

#define dSTANDARD_CAN_MSG_ID_2_0B 1
#define dEXTENDED_CAN_MSG_ID_2_0B 2

bool CANSPI_Initialize(void);

void CANSPI_Sleep(void);

uint8_t CANSPI_Transmit(iCommsMessage_t* txMsg);

uint8_t CANSPI_Receive(iCommsMessage_t* rxMsg);

uint8_t CANSPI_messagesInBuffer(void);

uint8_t CANSPI_isBussOff(void);

uint8_t CANSPI_isRxErrorPassive(void);

uint8_t CANSPI_isTxErrorPassive(void);

/**
 * Convert message contents into a 32 bit unsigned integer
 * @param msg
 * @return
 */
uint32_t readMsg(iCommsMessage_t* msg);

iCommsMessage_t createMsg(uint16_t msgId, uint32_t data);

#ifdef __cplusplus
}
#endif

#endif /* USERDRIVERS_CANDRIVER_H_ */
