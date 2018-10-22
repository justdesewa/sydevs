#pragma once
#ifndef SYDEVS_EXAMPLES_ADVANCED_BUILDING_DYNAMICS_NODE_H_
#define SYDEVS_EXAMPLES_ADVANCED_BUILDING_DYNAMICS_NODE_H_

#include <examples/demo/building7m_advanced/weather_node.h>
#include <examples/demo/building7m_advanced/thermodynamics_node.h>
#include <examples/demo/building7m_advanced/comfort_node.h>
#include <examples/demo/building7m_advanced/occupant_node.h>
#include <sydevs/systems/composite_node.h>

namespace sydevs_examples {

using namespace sydevs;
using namespace sydevs::systems;


class building_dynamics_node : public composite_node
{
public:
    // Constructor/Destructor:
    building_dynamics_node(const std::string& node_name, const node_context& external_context);
    virtual ~building_dynamics_node() = default;

    // Ports:
    port<flow, input, thermodynamic_temperature> initial_temperature_input;
    port<flow, input, std::pair<array2d<int64>, distance>> building_layout_input;
    port<flow, input, float64> wall_resistance_input;
    port<flow, input, array1d<int64>> initial_position_input;
    port<flow, input, quantity<decltype(_m/_s)>> walking_speed_input;
    port<message, output, array2d<thermodynamic_temperature>> temperature_field_output;
    port<message, output, array1d<int64>> occupant_position_output;
    port<flow, output, thermodynamic_temperature> average_temperature_output;

    // Components:
    weather_node weather;
    thermodynamics_node thermodynamics;
    comfort_node comfort;
    occupant_node occupant;
};


building_dynamics_node::building_dynamics_node(const std::string& node_name, const node_context& external_context)
    : composite_node(node_name, external_context)
    , initial_temperature_input("initial_temperature_input", external_interface())
    , building_layout_input("building_layout_input", external_interface())
    , wall_resistance_input("wall_resistance_input", external_interface())
    , initial_position_input("initial_position_input", external_interface())
    , walking_speed_input("walking_speed_input", external_interface())
    , temperature_field_output("temperature_field_output", external_interface())
    , occupant_position_output("occupant_position_output", external_interface())
    , average_temperature_output("average_temperature_output", external_interface())
    , weather("weather", internal_context())
    , thermodynamics("thermodynamics", internal_context())
    , comfort("comfort", internal_context())
    , occupant("occupant", internal_context())
{
    // Flow Input Links
    inward_link(initial_temperature_input, weather.initial_temperature_input);
    inward_link(initial_temperature_input, thermodynamics.initial_temperature_input);
    inward_link(initial_temperature_input, comfort.initial_temperature_input);
    inward_link(initial_temperature_input, occupant.initial_temperature_input);
    inward_link(building_layout_input, thermodynamics.building_layout_input);
    inward_link(building_layout_input, occupant.building_layout_input);
    inward_link(wall_resistance_input, thermodynamics.wall_resistance_input);
    inward_link(initial_position_input, occupant.initial_position_input);
    inward_link(walking_speed_input, occupant.walking_speed_input);

    // Message Links
    inner_link(weather.outdoor_temperature_output, thermodynamics.outdoor_temperature_input);
    inner_link(thermodynamics.temperature_field_output, comfort.temperature_field_input);
    inner_link(comfort.occupant_temperature_output, occupant.occupant_temperature_input);
    inner_link(occupant.occupant_position_output, comfort.occupant_position_input);
    outward_link(thermodynamics.temperature_field_output, temperature_field_output);
    outward_link(occupant.occupant_position_output, occupant_position_output);

    // Flow Output Links
    outward_link(occupant.average_temperature_output, average_temperature_output);
}


}  // namespace

#endif