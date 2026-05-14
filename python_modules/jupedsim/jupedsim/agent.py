# SPDX-License-Identifier: LGPL-3.0-or-later

import jupedsim.native as py_jps
from jupedsim.models.anticipation_velocity_model import (
    AnticipationVelocityModelState,
)
from jupedsim.models.collision_free_speed import CollisionFreeSpeedModelState
from jupedsim.models.collision_free_speed_v2 import (
    CollisionFreeSpeedModelV2State,
)
from jupedsim.models.generalized_centrifugal_force import (
    GeneralizedCentrifugalForceModelState,
)
from jupedsim.models.social_force import SocialForceModelState


class Agent:
    """Represents an Agent in the simulation.

    Agent objects are always retrieved from the simulation and never created directly.

    Agents can be accessed with:

    .. code:: python

        # a specific agent
        sim.agent(id)

        # all agents as iterator
        sim.agents()

        # agents in a specific distance to a point as iterator
        sim.agents_in_range(position, distance)

        # agents in a polygon as iterator
        sim.agents_in_polygon(polygon)

    .. note ::

        You need to be aware that currently there are no checks done when setting
        properties on an Agent instance. For example it is possible to set an Agent position
        outside the walkable area of the Simulation resulting in a crash.
    """

    def __init__(self, backing) -> None:
        """Do not use.

        Retrieve agents from the simulation.
        """
        self._obj = backing

    @property
    def id(self) -> int:
        """Numeric id of the agent in this simulation."""
        return self._obj.id

    @property
    def journey_id(self) -> int:
        """Id of the :class:`~jupedsim.journey.JourneyDescription` the agent is currently following."""
        return self._obj.journey_id

    @property
    def stage_id(self) -> int:
        """Id of the :class:`Stage` the Agent is currently targeting."""
        return self._obj.stage_id

    @property
    def position(self) -> tuple[float, float]:
        """Position of the agent."""
        return self._obj.position

    @property
    def orientation(self) -> tuple[float, float]:
        """Orientation of the agent."""
        return self._obj.orientation

    @property
    def target(self) -> tuple[float, float]:
        """Current target of the agent.

        Can be used to directly steer an agent towards the given coordinate.
        This will bypass the strategical and tactical level, but the operational level
        will still be active.

        .. important::

            If the agent is not in a Journey with a DirectSteering stage, any change will be
            ignored.

        .. important::

            When setting the target, the given coordinates must lie within the walkable area.
            Otherwise, an error will be thrown at the next iteration call.

        Returns:
            Current target of the agent.
        """
        return self._obj.target

    @target.setter
    def target(self, target: tuple[float, float]):
        self._obj.target = target

    @property
    def type(self) -> str:
        """Type of the agent (e.g., 'male', 'female', 'child')."""
        return self._obj.type

    @type.setter
    def type(self, type: str):
        self._obj.type = type

    @property
    def heartbeat(self) -> float:
        """Heartbeat of the agent in bpm."""
        return self._obj.heartbeat

    @heartbeat.setter
    def heartbeat(self, val: float):
        self._obj.heartbeat = val

    @property
    def stress(self) -> float:
        """Stress level of the agent (0.0 to 1.0)."""
        return self._obj.stress

    @stress.setter
    def stress(self, val: float):
        self._obj.stress = val

    @property
    def panic(self) -> float:
        """Panic level of the agent (0.0 to 1.0)."""
        return self._obj.panic

    @panic.setter
    def panic(self, val: float):
        self._obj.panic = val

    @property
    def openness(self) -> float:
        """Openness trait of the agent (0.0 to 1.0)."""
        return self._obj.openness

    @openness.setter
    def openness(self, val: float):
        self._obj.openness = val

    @property
    def conscientiousness(self) -> float:
        """Conscientiousness trait of the agent (0.0 to 1.0)."""
        return self._obj.conscientiousness

    @conscientiousness.setter
    def conscientiousness(self, val: float):
        self._obj.conscientiousness = val

    @property
    def extraversion(self) -> float:
        """Extraversion trait of the agent (0.0 to 1.0)."""
        return self._obj.extraversion

    @extraversion.setter
    def extraversion(self, val: float):
        self._obj.extraversion = val

    @property
    def agreeableness(self) -> float:
        """Agreeableness trait of the agent (0.0 to 1.0)."""
        return self._obj.agreeableness

    @agreeableness.setter
    def agreeableness(self, val: float):
        self._obj.agreeableness = val

    @property
    def neuroticism(self) -> float:
        """Neuroticism trait of the agent (0.0 to 1.0)."""
        return self._obj.neuroticism

    @neuroticism.setter
    def neuroticism(self, val: float):
        self._obj.neuroticism = val

    @property
    def model(
        self,
    ) -> (
        GeneralizedCentrifugalForceModelState
        | CollisionFreeSpeedModelState
        | CollisionFreeSpeedModelV2State
        | AnticipationVelocityModelState
        | SocialForceModelState
    ):
        """Access model specific state of this agent."""
        model = self._obj.model
        if isinstance(model, py_jps.GeneralizedCentrifugalForceModelState):
            return GeneralizedCentrifugalForceModelState(model)
        elif isinstance(model, py_jps.CollisionFreeSpeedModelState):
            return CollisionFreeSpeedModelState(model)
        elif isinstance(model, py_jps.CollisionFreeSpeedModelV2State):
            return CollisionFreeSpeedModelV2State(model)
        elif isinstance(model, py_jps.AnticipationVelocityModelState):
            return AnticipationVelocityModelState(model)
        elif isinstance(model, py_jps.SocialForceModelState):
            return SocialForceModelState(model)
        else:
            raise Exception("Internal error")
