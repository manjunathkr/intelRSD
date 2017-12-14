/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * */

#include "agent-framework/module/constants/chassis.hpp"
#include "agent-framework/module/requests/common.hpp"
#include "agent-framework/module/responses/common.hpp"

#include "psme/rest/endpoints/chassis/chassis.hpp"
#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/chassis.hpp"
#include "psme/rest/server/error/error_factory.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {

json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Chassis.Chassis";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Chassis.v1_3_0.Chassis";
    r[Common::ID] = json::Value::Type::NIL;
    r[Chassis::CHASSIS_TYPE] = json::Value::Type::NIL;
    r[Common::NAME] = "Chassis";
    r[Common::DESCRIPTION] = "Chassis description";

    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Common::MODEL] = json::Value::Type::NIL;
    r[Common::SKU] = json::Value::Type::NIL;
    r[Common::SERIAL_NUMBER] = json::Value::Type::NIL;
    r[Common::PART_NUMBER] = json::Value::Type::NIL;
    r[Common::ASSET_TAG] = json::Value::Type::NIL;
    r[Chassis::INDICATOR_LED] = json::Value::Type::NIL;
    r[Chassis::POWER_STATE] = json::Value::Type::NIL;

    // Power and Thermal links are created only for Zone and Sled Chassis

    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    json::Value rs{};
    rs[Common::ODATA_TYPE] = "#Intel.Oem.Chassis";
    rs[Common::LOCATION][Common::ID] = json::Value::Type::NIL;
    rs[Common::LOCATION][Chassis::PARENT_ID] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE] = std::move(rs);

    r[Common::LINKS][Common::ODATA_TYPE] = "#Chassis.v1_2_0.Links";
    r[Common::LINKS][Chassis::CONTAINS] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::CONTAINED_BY] = json::Value::Type::NIL;
    r[Common::LINKS][Chassis::COMPUTER_SYSTEMS] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::MANAGED_BY] = json::Value::Type::ARRAY;
    r[Common::LINKS][Chassis::MANAGERS_IN_CHASSIS] = json::Value::Type::ARRAY;
    r[Common::LINKS][Chassis::POWERED_BY] = json::Value::Type::ARRAY;
    r[Common::LINKS][Chassis::COOLED_BY] = json::Value::Type::ARRAY;
    r[Common::LINKS][Chassis::DRIVES] = json::Value::Type::ARRAY;
    r[Common::LINKS][Chassis::STORAGE] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.ChassisLinks";
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Chassis::SWITCHES] = json::Value::Type::ARRAY;

    return r;
}


void fill_chassis_link(json::Value& r, const std::string& chassis_uuid) {
    r[Common::ODATA_ID] = endpoint::utils::get_component_url(agent_framework::model::enums::Component::Chassis, chassis_uuid);
}

void fill_chassis_links(json::Value& r, const std::vector<std::string>& uuids,
        OptionalField<std::string> omit_uuid = {}) {

    for (const auto& uuid : uuids) {
        if (!omit_uuid.has_value() || uuid != omit_uuid.value()) {
            json::Value link{};
            link[Common::ODATA_ID] =
                endpoint::utils::get_component_url(agent_framework::model::enums::Component::Chassis, uuid);
            r.push_back(std::move(link));
        }
    }
}

template <agent_framework::model::enums::ChassisType::base_enum CHASSIS_TYPE>
bool is_chassis_of_type(const agent_framework::model::Chassis& ch) {
    return CHASSIS_TYPE == ch.get_type();
}

std::string get_chassis_location_id(const agent_framework::model::Chassis& chassis) {
    if (chassis.get_location_id().has_value()) {
        return chassis.get_location_id().value();
    }
    return std::to_string(chassis.get_location_offset());
}

void fill_containing_links(const agent_framework::model::Chassis& chassis, json::Value& r) {

    auto is_rack = is_chassis_of_type<agent_framework::model::enums::ChassisType::Rack>;
    auto is_zone = is_chassis_of_type<agent_framework::model::enums::ChassisType::Zone>;
    auto is_drawer = is_chassis_of_type<agent_framework::model::enums::ChassisType::Drawer>;
    auto is_enclosure = is_chassis_of_type<agent_framework::model::enums::ChassisType::Enclosure>;
    auto is_child_chassis = [&is_rack, &is_drawer, &is_zone](const agent_framework::model::Chassis& c) {
        return !is_rack(c) && !is_drawer(c) && !is_zone(c);
    };
    auto& chassis_manager = agent_framework::module::get_manager<agent_framework::model::Chassis>();

    /* By default - set Location/Id and Location/ParentId */
    r[Common::OEM][Common::RACKSCALE][Common::LOCATION][constants::Chassis::PARENT_ID] = chassis.get_parent_id();
    r[Common::OEM][Common::RACKSCALE][Common::LOCATION][Common::ID] = get_chassis_location_id(chassis);

    try {
        if (is_rack(chassis)) {
            /* This is the top chassis and it contains all other chassis, does not contain itself */
            auto uuids = chassis_manager.get_keys();
            fill_chassis_links(r[Common::LINKS][Chassis::CONTAINS], uuids, chassis.get_uuid());
        }
        else if (is_zone(chassis)) {
            /* Zones are contained by the rack, and does not contain any other chassis */
            auto uuids = chassis_manager.get_keys(is_rack);
            if (uuids.size() > 0) {
                fill_chassis_link(r[Common::LINKS][Common::CONTAINED_BY], uuids.front());
            }
        }
        else if (is_drawer(chassis)) {
            /* Drawers contain everything but racks, zones and other drawers */
            auto uuids = chassis_manager.get_keys(is_child_chassis);
            fill_chassis_links(r[Common::LINKS][Chassis::CONTAINS], uuids);

            /* Drawers may be contained by racks */
            auto racks = chassis_manager.get_keys(is_rack);
            if (racks.size() > 0) {
                fill_chassis_link(r[Common::LINKS][Common::CONTAINED_BY], racks.front());
            }
        }
        else if (is_enclosure(chassis)) {
            /* Enclosures may contain everything but racks, zones and drawers, does not contain itself */
            auto uuids = chassis_manager.get_keys(is_child_chassis);
            fill_chassis_links(r[Common::LINKS][Chassis::CONTAINS], uuids, chassis.get_uuid());
        }
        else {
            /* Other cases -> chassis is to be contained by the drawer */
            auto uuids = chassis_manager.get_keys(is_drawer);
            if (uuids.size() > 0) {
                fill_chassis_link(r[Common::LINKS][Common::CONTAINED_BY], uuids.front());
                /* Inherit parent id */
                r[Common::OEM][Common::RACKSCALE][Common::LOCATION][constants::Chassis::PARENT_ID] =
                    get_chassis_location_id(chassis_manager.get_entry(uuids.front()));
            }
        }
    }
    catch (const std::exception& e) {
        log_error(GET_LOGGER("rest"), "Exception caught during filling Chassis"
            "links Contains and ContainedBy: " << e.what());
    }



}

void fill_links(const agent_framework::model::Chassis& chassis, json::Value& r) {
    // find manager of this chassis
    try {
        if (chassis.get_is_managed()) {
            json::Value managed_by{};
            managed_by[Common::ODATA_ID] = psme::rest::endpoint::utils::get_component_url(
                agent_framework::model::enums::Component::Manager, chassis.get_parent_uuid());
            r[Common::LINKS][Common::MANAGED_BY].push_back(managed_by);
        }
    }
    catch (const agent_framework::exceptions::InvalidUuid&) {}


    // find managers located in this chassis
    auto managers_in_chassis_uuids = agent_framework::module::get_manager<agent_framework::model::Manager>()
        .get_keys([&chassis](const agent_framework::model::Manager& manager) {
            return manager.get_location() == chassis.get_uuid();
        });
    for (const auto& manager_uuid : managers_in_chassis_uuids) {
        json::Value link{};
        link[Common::ODATA_ID] = psme::rest::endpoint::utils::get_component_url(
            agent_framework::model::enums::Component::Manager, manager_uuid);
        r[Common::LINKS][Chassis::MANAGERS_IN_CHASSIS].push_back(std::move(link));
    }


    // systems and storage subsystems in chassis
    auto& system_manager = agent_framework::module::get_manager<agent_framework::model::System>();
    auto& storage_manager = agent_framework::module::get_manager<agent_framework::model::StorageSubsystem>();
    auto system_uuids = system_manager.get_keys();
    for (const auto& system_uuid : system_uuids) {
        auto system = system_manager.get_entry(system_uuid);
        if (system.get_chassis() == chassis.get_uuid()) {
            json::Value link{};
            link[Common::ODATA_ID] = endpoint::PathBuilder(PathParam::BASE_URL)
                .append(constants::Root::SYSTEMS)
                .append(system.get_id()).build();
            r[Common::LINKS][constants::Chassis::COMPUTER_SYSTEMS].push_back(std::move(link));

            for (const auto storage_id : storage_manager.get_ids(system.get_uuid())) {
                json::Value storage_link{};
                storage_link[Common::ODATA_ID] = endpoint::PathBuilder(PathParam::BASE_URL)
                    .append(constants::Root::SYSTEMS)
                    .append(system.get_id())
                    .append(constants::System::STORAGE)
                    .append(storage_id).build();
                r[Common::LINKS][constants::Chassis::STORAGE].push_back(std::move(storage_link));
            }
        }
    }


    // switches in chassis
    auto& switch_manager = agent_framework::module::NetworkComponents::get_instance()->get_switch_manager();
    auto switch_uuids = switch_manager.get_keys();
    for (const auto& switch_uuid : switch_uuids) {
        auto switch_ = switch_manager.get_entry_reference(switch_uuid);
        if (switch_->get_chassis() == chassis.get_uuid()) {
            json::Value link{};
            link[Common::ODATA_ID] = endpoint::PathBuilder(PathParam::BASE_URL)
                .append(constants::Root::ETHERNET_SWITCHES)
                .append(switch_->get_id()).build();
            r[Common::LINKS][Common::OEM][Common::RACKSCALE][constants::Chassis::SWITCHES].push_back(std::move(link));
        }
    }


    // drives in chassis
    auto drive_ids = agent_framework::module::get_manager<agent_framework::model::Drive>().get_ids(chassis.get_uuid());
    for (const auto& drive_id : drive_ids) {
        json::Value link{};
        link[Common::ODATA_ID] = endpoint::PathBuilder(constants::PathParam::BASE_URL)
            .append(Common::CHASSIS)
            .append(chassis.get_id())
            .append(constants::Chassis::DRIVES)
            .append(drive_id).build();
        r[Common::LINKS][Chassis::DRIVES].push_back(std::move(link));
    }


    // power of the chassis
    try {
        const auto& power_zone_uuid = chassis.get_power_zone();
        if (power_zone_uuid.has_value()) {
            json::Value link_to_power{};
            link_to_power[Common::ODATA_ID] =
                endpoint::utils::get_component_url(agent_framework::model::enums::Component::PowerZone,
                                                   power_zone_uuid);
            r[Common::LINKS][Chassis::POWERED_BY].push_back(std::move(link_to_power));
        }
    }
    catch (const agent_framework::exceptions::InvalidUuid&) {}


    // thermal of the chassis
    try {
        const auto& thermal_zone_uuid = chassis.get_thermal_zone();
        if (thermal_zone_uuid.has_value()) {
            json::Value link_to_thermal{};
            link_to_thermal[Common::ODATA_ID] =
                endpoint::utils::get_component_url(agent_framework::model::enums::Component::ThermalZone,
                                                   thermal_zone_uuid);
            r[Common::LINKS][Chassis::COOLED_BY].push_back(std::move(link_to_thermal));
        }
    }
    catch (const agent_framework::exceptions::InvalidUuid&) {}
}


void fill_power_and_thermal_links(const agent_framework::model::Chassis& chassis, json::Value& r) {

    auto thermal_zones = agent_framework::module::get_manager<agent_framework::model::ThermalZone>()
        .get_keys(chassis.get_uuid());
    if(thermal_zones.size() > 0) {
        r[Chassis::THERMAL][Common::ODATA_ID] = endpoint::PathBuilder(PathParam::BASE_URL)
            .append(constants::Common::CHASSIS)
            .append(chassis.get_id())
            .append(constants::Chassis::THERMAL)
            .build();
    }

    auto power_zones = agent_framework::module::get_manager<agent_framework::model::PowerZone>()
        .get_keys(chassis.get_uuid());
    if(power_zones.size() > 0) {
        r[Chassis::POWER][Common::ODATA_ID] = endpoint::PathBuilder(PathParam::BASE_URL)
            .append(constants::Common::CHASSIS)
            .append(chassis.get_id())
            .append(constants::Chassis::POWER)
            .build();
    }
}


static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::Chassis::ASSET_TAG, constants::Common::ASSET_TAG},
    {agent_framework::model::literals::Chassis::GEO_TAG, constants::Chassis::GEO_TAG},
    {agent_framework::model::literals::Chassis::LOCATION_ID,
        endpoint::PathBuilder(constants::Common::OEM)
            .append(constants::Common::RACKSCALE)
            .append(constants::Chassis::LOCATION)
            .append(constants::Common::ID)
            .build()
    }
};

}


endpoint::Chassis::Chassis(const std::string& path) : EndpointBase(path) {}


endpoint::Chassis::~Chassis() {}


void endpoint::Chassis::get(const server::Request& request, server::Response& response) {
    auto chassis = psme::rest::model::Find<agent_framework::model::Chassis>(request.params[PathParam::CHASSIS_ID])
        .get();

    auto r = make_prototype();

    psme::rest::endpoint::status_to_json(chassis, r);
    r[Common::ODATA_ID] = PathBuilder(request).build();
    r[Common::ID] = request.params[PathParam::CHASSIS_ID];

    fill_links(chassis, r);
    fill_containing_links(chassis, r);
    fill_power_and_thermal_links(chassis, r);

    if (agent_framework::model::enums::ChassisType::Rack == chassis.get_type()) {
        r[Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.RackChassis";
        r[Common::OEM][Common::RACKSCALE][Common::UUID] = chassis.get_uuid();
        r[Common::OEM][Common::RACKSCALE][constants::Chassis::GEO_TAG] = chassis.get_geo_tag();
        r[Common::OEM][Common::RACKSCALE][constants::Chassis::RACK_SUPPORTS_DISAGGREGATED_POWER_COOLING] =
            chassis.get_disaggregated_power_cooling_support();
    }
    else if (agent_framework::model::enums::ChassisType::Drawer == chassis.get_type()) {
        json::Value reset{};
        reset[Common::TARGET] =
            endpoint::PathBuilder(request)
                .append(Common::ACTIONS)
                .append(constants::Chassis::CHASSIS_RESET)
                .build();

        reset[Common::ALLOWABLE_RESET_TYPES] = json::Value::Type::ARRAY;
        for (const auto& reset_type : chassis.get_allowed_reset_actions()) {
            reset[Common::ALLOWABLE_RESET_TYPES].push_back(reset_type.to_string());
        }
        r[Common::ACTIONS][constants::Chassis::HASH_CHASSIS_RESET] = std::move(reset);
    }

    r[constants::Chassis::CHASSIS_TYPE] = chassis.get_type().to_string();
    r[Common::MANUFACTURER] = chassis.get_fru_info().get_manufacturer();
    r[Common::MODEL] = chassis.get_fru_info().get_model_number();
    r[Common::SERIAL_NUMBER] = chassis.get_fru_info().get_serial_number();
    r[Common::PART_NUMBER] = chassis.get_fru_info().get_part_number();
    r[Common::ASSET_TAG] = chassis.get_asset_tag();
    r[Common::SKU] = chassis.get_sku();
    r[constants::Chassis::INDICATOR_LED] = chassis.get_indicator_led();

    set_response(response, r);
}


void endpoint::Chassis::patch(const server::Request& request, server::Response& response) {
    auto chassis = model::Find<agent_framework::model::Chassis>(request.params[PathParam::CHASSIS_ID]).get();
    const auto json = JsonValidator::validate_request_body<schema::ChassisPatchSchema>(request);

    agent_framework::model::attribute::Attributes attributes{};
    if (json.is_member(constants::Common::ASSET_TAG)) {
        const auto& asset_tag = json[constants::Common::ASSET_TAG].as_string();
        attributes.set_value(agent_framework::model::literals::Chassis::ASSET_TAG, asset_tag);
    }
    if (json.is_member(constants::Common::OEM)) {
        const auto& oem = json[constants::Common::OEM];
        if (oem.is_member(constants::Common::RACKSCALE)) {
            const auto& rackscale = oem[constants::Common::RACKSCALE];
            if (rackscale.is_member(constants::Chassis::LOCATION)) {
                const auto& location = rackscale[constants::Chassis::LOCATION];
                if (location.is_member(constants::Common::ID)) {
                    const auto& location_id = location[constants::Common::ID].as_string();
                    attributes.set_value(agent_framework::model::literals::Chassis::LOCATION_ID, location_id);
                }
            }
            if (rackscale.is_member(constants::Chassis::GEO_TAG)) {
                const auto& geo_tag = rackscale[constants::Chassis::GEO_TAG].as_string();
                attributes.set_value(agent_framework::model::literals::Chassis::GEO_TAG, geo_tag);
            }
        }
    }

    if (!attributes.empty()) {
        agent_framework::model::requests::SetComponentAttributes
            set_component_attributes_request{chassis.get_uuid(), attributes};

        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(chassis.get_agent_id());

        auto set_chassis_attributes = [&, gami_agent] {
            // Call set component attribute method
            const auto& set_component_attributes_response =
                gami_agent->execute<agent_framework::model::responses::SetComponentAttributes>(
                    set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, ::gami_to_rest_attributes);
                throw error::ServerException(error);
            }

            psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::Chassis)->
                load(gami_agent,
                     chassis.get_parent_uuid(),
                     agent_framework::model::enums::Component::Manager,
                     chassis.get_uuid(),
                     false);
        };

        gami_agent->execute_in_transaction(set_chassis_attributes);
    }

    get(request, response);
}
