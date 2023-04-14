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

#define MAX_PERCENTAGE 1000
#define MIN_PERCENTAGE 0

typedef enum
{
	RESULT_FAIL,
	RESULT_OK
} result_t;

typedef enum {
	ENABLED,
	DISABLED
} Enable_t;

typedef enum ErrorCode
{
	THROTTLE_TOO_HIGH
} ErrorCode;

typedef enum EventCode
{
	DEADMAN,
	TIMER
} EventCode;

typedef enum { Clear = 0, Set = 1 } flag_status_t;


typedef uint16_t length_t;
typedef uint16_t percentage_t;
typedef uint32_t velocity_t;


typedef struct
{
	uint16_t standardMessageID; // 11 bit max
	uint8_t dataLength; // max 8
	uint8_t data[8];
}iCommsMessage_t;


#endif /* _APPLICATIONTYPES_H_ */
