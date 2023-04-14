/*
 * InternalCommsModule.h
 *
 *  Created on: Aug 6, 2022
 *      Author: mingye chen
 */

#ifndef MODULES_INTERNALCOMMSMODULE_H_
#define MODULES_INTERNALCOMMSMODULE_H_


#include "ApplicationTypes.h"

PUBLIC result_t IComms_Init();
PUBLIC result_t IComms_ReceiveNextMessage(iCommsMessage_t * rxMsg);
PUBLIC uint8_t IComms_HasRxMessage();
PUBLIC void IComms_Update();
PUBLIC result_t IComms_Transmit(iCommsMessage_t * txMsg);
PUBLIC iCommsMessage_t IComms_CreateMessage(uint16_t standardMessageID, uint8_t dataLength, uint8_t data[8]);
PUBLIC iCommsMessage_t IComms_CreatePercentageMessage(uint16_t standardMessageID, percentage_t percentage);
PUBLIC iCommsMessage_t IComms_CreateErrorMessage(uint16_t standardMessageID, ErrorCode code, flag_status_t);
PUBLIC iCommsMessage_t IComms_CreateEventMessage(uint16_t standardMessageID, uint8_t code, uint8_t status);

#endif /* MODULES_INTERNALCOMMSMODULE_H_ */
