// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once
#include "AnticipationVelocityModelData.hpp"
#include "CollisionFreeSpeedModelData.hpp"
#include "CollisionFreeSpeedModelV2Data.hpp"
#include "GeneralizedCentrifugalForceModelData.hpp"
#include "OperationalModel.hpp"
#include "Point.hpp"
#include "SocialForceModelData.hpp"
#include "UniqueID.hpp"
#include "Visitor.hpp"

#include <fmt/core.h>

#include <utility>
#include <variant>
class Journey;
class BaseStage;

struct GenericAgent {
    using ID = jps::UniqueID<GenericAgent>;
    ID id{};

    jps::UniqueID<Journey> journeyId{jps::UniqueID<Journey>::Invalid};
    jps::UniqueID<BaseStage> stageId{jps::UniqueID<BaseStage>::Invalid};

    // This is evaluated by the "operational level"
    Point destination{};
    Point target{};

    // Agent fields common for all models
    Point pos{};
    Point orientation{};
    double fatigue{0.0};
    double fatigue_rate{0.0};
    std::string type{"male"};
    
    // Physiological & Psychological parameters
    double heartbeat{70.0};
    double stress{0.0};
    double panic{0.0};
    
    // OCEAN Traits (0.0 to 1.0)
    double openness{0.5};
    double conscientiousness{0.5};
    double extraversion{0.5};
    double agreeableness{0.5};
    double neuroticism{0.5};

    using Model = std::variant<
        GeneralizedCentrifugalForceModelData,
        CollisionFreeSpeedModelData,
        CollisionFreeSpeedModelV2Data,
        AnticipationVelocityModelData,
        SocialForceModelData>;
    Model model{};

    GenericAgent(
        ID id_,
        jps::UniqueID<Journey> journeyId_,
        jps::UniqueID<BaseStage> stageId_,
        Point pos_,
        Point orientation_,
        Model model_)
        : id(id_ != ID::Invalid ? id_ : ID{})
        , journeyId(journeyId_)
        , stageId(stageId_)
        , target(pos_)
        , pos(pos_)
        , orientation(orientation_)
        , model(std::move(model_))
    {
    }
};
template <>
struct fmt::formatter<GenericAgent> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const GenericAgent& agent, FormatContext& ctx) const
    {
        return std::visit(
            [&ctx, &agent](const auto& m) {
                return fmt::format_to(
                    ctx.out(),
                    "Agent[id={}, journey={}, stage={}, pos={}, type={}, "
                    "heartbeat={:.1f}, stress={:.2f}, panic={:.2f}, "
                    "O={:.2f}, C={:.2f}, E={:.2f}, A={:.2f}, N={:.2f}, model={})",
                    agent.id,
                    agent.journeyId,
                    agent.stageId,
                    agent.pos,
                    agent.type,
                    agent.heartbeat,
                    agent.stress,
                    agent.panic,
                    agent.openness,
                    agent.conscientiousness,
                    agent.extraversion,
                    agent.agreeableness,
                    agent.neuroticism,
                    m);
            },
            agent.model);
    }
};
