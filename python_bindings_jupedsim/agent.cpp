// SPDX-License-Identifier: LGPL-3.0-or-later
#include "GeneralizedCentrifugalForceModel.hpp"
#include "GenericAgent.hpp"
#include "conversion.hpp"

#include <pybind11/cast.h>
#include <pybind11/detail/common.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // IWYU pragma: keep

#include <cstdint>
#include <tuple>

namespace py = pybind11;

void init_agent(py::module_& m)
{
    py::class_<GenericAgent>(m, "Agent")
        .def(
            py::init([](uint64_t journeyId,
                        uint64_t stageId,
                        std::tuple<double, double> position,
                        std::tuple<double, double> orientation,
                        GenericAgent::Model model,
                        double fatigue,
                        double fatigue_rate) {
                auto agent = GenericAgent(
                    GenericAgent::ID::Invalid,
                    journeyId,
                    stageId,
                    intoPoint(position),
                    intoPoint(orientation),
                    model);
                agent.fatigue = fatigue;
                agent.fatigue_rate = fatigue_rate;
                return agent;
            }),
            py::kw_only(),
            py::arg("journey_id"),
            py::arg("stage_id"),
            py::arg("position"),
            py::arg("orientation"),
            py::arg("model"),
            py::arg("fatigue") = 0.0,
            py::arg("fatigue_rate") = 0.0)
        .def_property_readonly("id", [](const GenericAgent& agent) { return agent.id.getID(); })
        .def_property_readonly(
            "journey_id", [](const GenericAgent& agent) { return agent.journeyId.getID(); })
        .def_property_readonly(
            "stage_id", [](const GenericAgent& agent) { return agent.stageId.getID(); })
        .def_property_readonly(
            "position", [](const GenericAgent& agent) { return intoTuple(agent.pos); })
        .def_property_readonly(
            "orientation", [](const GenericAgent& agent) { return intoTuple(agent.orientation); })
        .def_property(
            "target",
            [](const GenericAgent& agent) { return intoTuple(agent.target); },
            [](GenericAgent& agent, std::tuple<double, double> target) {
                agent.target = intoPoint(target);
            })
        .def_property(
            "fatigue",
            [](const GenericAgent& agent) { return agent.fatigue; },
            [](GenericAgent& agent, double fatigue) { agent.fatigue = fatigue; })
        .def_property(
            "fatigue_rate",
            [](const GenericAgent& agent) { return agent.fatigue_rate; },
            [](GenericAgent& agent, double fatigue_rate) { agent.fatigue_rate = fatigue_rate; })
        .def_property_readonly(
            "model",
            [](GenericAgent& agent) -> auto& { return agent.model; },
            py::return_value_policy::reference);
}
