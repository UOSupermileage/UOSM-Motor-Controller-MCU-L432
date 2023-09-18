//
// Created by Jeremy Cote on 2023-09-11.
//

#ifndef UOSM_TELEMETRY_CANLOGENTRY_HPP
#define UOSM_TELEMETRY_CANLOGENTRY_HPP

#include "Identifiable.hpp"
#include "CANMessageLookUpModule.h"

enum CANLogEntryFormat {
    CAN_DECIMAL,
    CAN_HEX
};

/**
 * @class CANLogEntry
 * @brief Represents a log entry for CAN messages.
 *
 * An instance of the CANLogEntry class represents a CAN message type
 * and their associated data. It formats and stores log messages for further processing and display.
 */
class CANLogEntry: Identifiable {
private:
    /**
     * The maximum length of the message.
     */
    const uint8_t messageLength;

    /**
     * The formatted message.
     */
    char* message;

    /**
     * The type of the CAN message.
     */
    ICommsMessageLookUpIndex type;

public:
    /**
     * @brief Constructor for a CANLogEntry with single-value data.
     * @param type The type of CAN message.
     * @param value The value associated with the message.
     * @param style The formatting style for the log message (default is 0). 0 is decimal. 1 is hexadecimal.
     */
    CANLogEntry(ICommsMessageLookUpIndex type, uint32_t value, CANLogEntryFormat style): messageLength(20), type(type) {
        message = new char[messageLength];
        const char* format = style == CAN_DECIMAL ? "%s: [%d]" : "%s: [%04x]";
        sprintf(message, format, prettyType(), value);
    }

    /**
     * @brief Constructor for a CANLogEntry with two-value data.
     * @param type The type of CAN message.
     * @param a The first value associated with the message.
     * @param b The second value associated with the message.
     * @param style The formatting style for the log message (default is 0). 0 is decimal. 1 is hexadecimal.
     */
    CANLogEntry(ICommsMessageLookUpIndex type, uint32_t a, uint32_t b, uint8_t style): messageLength(24), type(type) {
        message = new char[messageLength];
        const char* format = style == CAN_DECIMAL ? "%s: [%d] [%d]" : "%s: [%04x] [%04x]";
        sprintf(message, format, prettyType(), a, b);
    }

    /**
     * @brief Copy constructor for creating a copy of a CANLogEntry object.
     * @param other The CANLogEntry object to copy.
     */
    CANLogEntry(const CANLogEntry& other): messageLength(other.messageLength), type(other.type) {
        message = new char[messageLength];
        strcpy(message, other.message);
    }


    /**
     * @brief Get a human-readable string representation of the CAN message type.
     * @return A string describing the CAN message type.
     */
    const char* prettyType() {
        switch (type) {
            case THROTTLE_DATA_ID:
                return "Throttle";
            case SPEED_DATA_ID:
                return "Speed";
            case MOTOR_RPM_DATA_ID:
                return "RPM";
            case EVENT_DATA_ID:
                return "Event";
            case ERROR_DATA_ID:
                return "Error";
            case CURRENT_VOLTAGE_DATA_ID:
                return "Cur/Volt";
        }

        return "ExecEr";
    }

    /**
     * @brief Get the formatted message
     * @return the formatted message
     */
    const char* getMessage() const {
        return message;
    }
};

#endif //UOSM_TELEMETRY_CANLOGENTRY_HPP
