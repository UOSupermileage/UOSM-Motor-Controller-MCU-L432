/*
 * ApplicationTypes.h
 *
 *	custom data types
 *
 *  Created on: Sep 4, 2022
 *      Author: mingye
 */

#ifndef _APPLICATIONTYPES_H_
#define _APPLICATIONTYPES_H_

#include <stdint.h>

#define PUBLIC
#define PRIVATE

typedef enum
{
	RESULT_FAIL,
	RESULT_OK
} result_t;

typedef uint16_t length_t;


typedef struct
{
	uint16_t standardMessageID; // 11 bit max
	uint8_t dataLength; // max 8
	uint8_t data[8];
}iCommsMessage_t;


#endif /* _APPLICATIONTYPES_H_ */
