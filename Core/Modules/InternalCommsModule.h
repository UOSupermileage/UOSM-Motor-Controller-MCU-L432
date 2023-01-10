/*
 * InternalCommsModule.h
 *
 *  Created on: Aug 6, 2022
 *      Author: mingye chen
 */

#ifndef MODULES_INTERNALCOMMSMODULE_H_
#define MODULES_INTERNALCOMMSMODULE_H_

#include "CANDriver.h"
#include "ApplicationTypes.h"

PUBLIC result_t IComms_Init();
PUBLIC result_t IComms_ReceiveNextMessage(iCommsMessage_t * rxMsg);
PUBLIC uint8_t IComms_HasRxMessage();
PUBLIC void IComms_Update();
PUBLIC result_t IComms_Transmit(iCommsMessage_t * txMsg);


#endif /* MODULES_INTERNALCOMMSMODULE_H_ */
