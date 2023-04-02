# UOSM-Motor-Controller

Firmware for motor controller.

```mermaid
---
title: Motor Controller Code Architecture
---
flowchart BT
    MotorTask["Motor Task"] --> DataAggregateModule["Data Aggregate Module"]
    iCommsTask["iComms task"] --> DataAggregateModule
    SafetyTask["Safety Task"] --> DataAggregateModule

    iCommsTask --> CANMessageLookupModule["CAN Message Lookup Module"]
    iCommsTask --> iCommsModule["iComms Module"]

    iCommsModule --> SafetyModule["Safety Module"]

    CANDriver["CAN Driver"] --> iCommsModule
    MCPDriver["MSP2515 Driver"] --> CANDriver

    SPIMotorDriver --> MotorTask
    SPIMotorDriver --> SafetyTask

    LEDStatusDriver["LED Status Driver"] --> SafetyTask
```

## Resources

<table>
<tr><th>Name</th><th>Description</th></tr>
<tr>
    <td><a href="https://github.com/trinamic/TMC-API">TMC API</a></td>
    <td>A collection of read/write functions for communicating with the TMC4671 and TMC6200 using SPI</td>
</tr>
</table>
