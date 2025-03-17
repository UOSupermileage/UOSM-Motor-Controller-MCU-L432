//
// Created by Jeremy Cote on 2025-01-15.
//

#include "BackupRegister.h"

extern RTC_HandleTypeDef hrtc;

void Backup_SetFaultStatus(FaultStatus status) {
    // Enable write access to backup region
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, status);
}

FaultStatus Backup_GetFaultStatus() {
    // TODO: Confirm that implicitly casting from backup register to FaultStatus is valid
    return HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
}

void Backup_SaveData(BackupData reg, uint32_t data) {
    HAL_RTCEx_BKUPWrite(&hrtc, reg, data);
}

uint32_t Backup_GetData(BackupData reg) {
    return HAL_RTCEx_BKUPRead(&hrtc, reg);
}

void Backup_Unlock() {
    HAL_PWR_EnableBkUpAccess();
}

void Backup_Lock() {
    HAL_PWR_DisableBkUpAccess();
}