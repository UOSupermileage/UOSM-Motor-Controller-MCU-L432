# Tasks

## Overview

<table>
<tr>
    <th>Task Name</th>
    <th>Frequency (ms)</th>
    <th>Priority</th>
    <th>Summary</th>
</tr>
<tr>
    <td>iComms</td>
    <td>200</td>
    <td>48 (Realtime)</td>
    <td>Poll CAN bus queue for new messages. Send messages in the Tx queue.</td>
</tr>
<tr>
    <td>Safety</td>
    <td>5000</td>
    <td>40 (High)</td>
    <td>Monitor system state for abnormal behaviour</td>
</tr>
<tr>
    <td>Motor</td>
    <td>200</td>
    <td>49 (Realtime1)</td>
    <td>Communicate with TMC chips our target velocity and other parameters</td>
</tr>
</table>

## Internal Communications

Responsabilties:

- Poll CAN bus for new messages
- Execute callback functions to handle received messages

## Safety

Monitor system health and alert users via software fault light and CAN messages.

The low frequency and priority are justified by the task's passive nature. Faults in the TMC chips are latched so they do not need to be polled frequently.

Responsabilities:

- Verifing SPI communication to both the TMC4671 and TMC6200
- Monitoring healthy temperature of both the TMC4671 and TMC6200

## Motor

Responsabilties:

- Communicate with the TMC4671 and TMC6200.
- Transfer throttle data to the TMC4671.

Behaviour is configured in the [MotorParamters.h Configuration File](/MotorParameters.md)

Functions prefixed by System are defined in the [Data Aggregation Module](https://github.com/UOSupermileage/UOSM-Motor-Controller-MCU-L432/blob/main/Core/Modules/DataAggregationModule.h)

Functions prefixed by Motor are defined in the [SPI Motor Driver](https://github.com/UOSupermileage/UOSM-Motor-Controller-MCU-L432/blob/main/Core/UserDrivers/SPIMotorDriver.h)

<div style="display: flex">
    <a style="margin-inline: auto; font-size: 1.5em" href="https://github.com/UOSupermileage/UOSM-Motor-Controller-MCU-L432/blob/main/Core/Tasks/MotorTask.c">MotorTask.c</a>
</div>

[MotorTask](/Core/Tasks/MotorTask.c ":include :type=code :fragment=task")
