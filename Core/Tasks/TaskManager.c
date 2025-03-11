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
#include "TempTask.h"
#include "DataAggregationModule.h"

void RunTaskManager( void )
{
	InitDataAggregator();
        InitMotorTask();
	InitInternalCommsTask();
	InitSafetyTask();
        InitTempTask();
}

