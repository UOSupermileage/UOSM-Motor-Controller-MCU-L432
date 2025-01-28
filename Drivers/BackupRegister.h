//
// Created by Jeremy Cote on 2025-01-15.
// Handles data backups for recovering from faults
//

// STM32L432 has 32 backup registers.
// Each 32-bit register can store 128 bytes of data

#ifndef USERDRIVERS_BACKUPREGISTER_H_
#define USERDRIVERS_BACKUPREGISTER_H_

#include "ApplicationTypes.h"

typedef enum {
    Status_NoFault = 0,
    Status_HardFault,
    Status_MemoryFault,
    Status_BusFault,
    Status_UsageFault,
    Status_NMIFault
} FaultStatus;

typedef enum {
    BackupData_FaultStatus = 0
} BackupData;

void Backup_SetFaultStatus(FaultStatus status);
FaultStatus Backup_GetFaultStatus();

void Backup_SaveData(BackupData, uint32_t data);
uint32_t Backup_GetData(BackupData);

void Backup_Unlock();
void Backup_Lock();

#endif // USERDRIVERS_BACKUPREGISTER_H_
