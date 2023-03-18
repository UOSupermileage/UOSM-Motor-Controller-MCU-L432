/*
 * TaskManager.c
 *
 *  Created on: Aug 6, 2022
 *      Author: mingye chen
 *
 *  This Task manages all other tasks by checking flags and running the appropriate tasks
 */
#include "TaskManager.h"
#include "SafetyTask.h"
#include "InternalCommsTask.h"
#include "MotorTask.h"
#include "Profiles.h"

void RunTaskManager( void )
{
#ifdef PROFILE_ICOMMS
	InitInternalCommsTask();
#else
	DebugPrint("iComms Task is disabled. Enable in Profiles.h");
#endif

#ifdef PROFILE_SAFETY
	InitSafetyTask();
#else
	DebugPrint("Safety Task is disabled. Enable in Profiles.h");
#endif

#ifdef PROFILE_MOTOR
	InitMotorTask();
#else
	DebugPrint("Motor Task is disabled. Enable in Profiles.h");
#endif
}
