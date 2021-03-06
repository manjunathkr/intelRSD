/*!
 * @brief Chassis Bmc implementation.
 *
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
 * @file chassis/include/status/bmc.hpp
 */

#pragma once

#include "agent-framework/state_machine/bmc.hpp"
#include "ipmi/management_controller.hpp"
#include "agent-framework/module/utils/optional_field.hpp"

using agent_framework::model::attribute::ConnectionData;

namespace agent {
namespace chassis {

class Bmc : public agent_framework::Bmc {
public:

    using State = agent_framework::state_machine::BmcState;

    Bmc(const std::string& manager_uuid,
        const ConnectionData& conn,
        Bmc::Duration state_update_interval,
        ReadPresenceFn read_presence = always_false,
        ReadOnlineStatusFn read_online_state = always_false);

    ipmi::IpmiController& ipmi() {
        return m_ipmi;
    }

    OptionalField<std::uint16_t> get_platform_id() const {
        return m_platform_id;
    }

protected:

    bool on_become_online(const Transition&) override;
    bool on_extraction(const Transition&) override;
    bool on_become_offline(const Transition&) override;

    void reset_tasks() override;

private:

    ipmi::ManagementController m_ipmi;
    OptionalField<std::uint16_t> m_platform_id{};
};

}
}
