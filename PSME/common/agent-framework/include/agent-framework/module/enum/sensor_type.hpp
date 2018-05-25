/*!
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file sensor_type.hpp
 */

#pragma once



#include <cstdint>
#include <string>
#include <cstring>
#include <vector>
#include <ostream>



namespace agent_framework {
namespace model {
namespace enums {

/*!
 * @brief SensorType enum for Metric Definition
 * */
class SensorType {
public:

    using underlying_type = std::uint32_t;

    /*!
     * @brief Enum constructor. See LogEntry.v1_0_0.SensorType in metadata for reference
     * */
    enum SensorTypeEnum : std::uint32_t {
        PlatformSecurityViolationAttempt, Temperature, Voltage, Current, Fan, PhysicalChassisSecurity, Processor,
        PowerSupplyOrConverter, PowerUnit, CoolingDevice, OtherUnitsBasedSensor, Memory, DriveSlotOrBay,
        POSTMemoryResize, SystemFirmwareProgress, EventLoggingDisabled, SystemEvent, CriticalInterrupt,
        ButtonOrSwitch, ModuleOrBoard, MicrocontrollerOrCoprocessor, AddInCard, Chassis, ChipSet, OtherFRU,
        CableOrInterconnect, Terminator, SystemBootOrRestart, BootError, BaseOSBootOrInstallationStatus,
        OSStopOrShutdown, SlotOrConnector, SystemACPIPowerState, Watchdog, PlatformAlert, EntityPresence,
        MonitorASICOrIC, LAN, ManagementSubsystemHealth, Battery, SessionAudit, VersionChange, FRUState
    };


    /*!
     * @brief to string
     *
     * @return pointer to an array of chars
     * */
    const char* to_string() const;


    /*!
     * @brief From string
     * @param string Reference to a string that is going to be processed into enum
     * @return SensorType object
     * */
    static SensorType from_string(const std::string& string);


    /*!
     * @brief Gets all available Enum values as a vector of strings.
     * @return Returns std::vector of std::string containing Enum values.
     * */
    static std::vector<std::string> get_values();


    /*!
     * @brief Default constructor
     * */
    SensorType() = delete;


    /*!
     * @brief Constructor
     * @param value uint32_t value of enum
     * */
    constexpr SensorType(SensorTypeEnum value) : m_value(value) {}


    /*!
     * @brief Operation of getting enums
     * @return Enum
     * */
    constexpr operator SensorTypeEnum() const {
        return static_cast<SensorTypeEnum>(m_value);
    }


    /*!
     * @brief Is string an allowable value of this enum
     * @param string Value to be checked
     * @return true if the string is a valid value of enum, false otherwise
     * */
    static bool is_allowable_value(const std::string& string);


    friend std::ostream& operator<<(std::ostream& out_stream, const SensorType& in_enum) {
        out_stream << in_enum.to_string();
        return out_stream;
    }


private:
    std::uint32_t m_value{};
};
}
}
}
