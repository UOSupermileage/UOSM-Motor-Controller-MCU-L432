/*
 * InternalCommsModule.h
 *
 *  Created on: Aug 6, 2022
 *      Author: mingye chen
 */

#ifndef MODULES_INTERNALCOMMSMODULE_H_
#define MODULES_INTERNALCOMMSMODULE_H_

#include <CANDriver.h>
#include <ApplicationTypes.h>

PUBLIC result_t ICommsInit();
PUBLIC uint8_t ICommsMessageAvailable();
PUBLIC result_t ICommsTransmit(iCommsMessage_t * txMsg);
PUBLIC result_t ICommsReceive(iCommsMessage_t * rxMsg);

#endif /* MODULES_INTERNALCOMMSMODULE_H_ */
