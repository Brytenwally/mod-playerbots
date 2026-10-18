/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_DUNGEONBOSSHELPERS_H
#define PLAYERBOTS_DUNGEONBOSSHELPERS_H

#include "MovementActions.h"

#include <cmath>
#include <span>

class Action;
class AiObjectContext;
class Player;
class PlayerbotAI;
class Unit;

// Shared by the CoA dungeon strategies. CoA runs the same boss kits on Normal, Heroic and Mythic; only health and
// damage change, and the Heroic/Mythic spells keep their names, so spells are matched by name.
namespace DungeonBoss
{
    constexpr float DegreesToRadians(float degrees) { return degrees * float(M_PI) / 180.0f; }

    bool HasAuraNamed(Unit const* unit, char const* name);
    bool IsCastingNamed(Unit const* unit, char const* name);

    // The unit's angle around `center`, measured from the center's facing, in (-pi, pi].
    float RelativeAngle(Unit const* center, Unit const* unit);

    // A cone a boss casts in front of itself, at its tank. Without a spell name the boss casts it so often, or
    // without a cast bar, that its front is never safe.
    struct FrontalCone
    {
        char const* boss;
        float reach;
        float halfArc;
        char const* spell;
    };

    // Margin around a cone, so that the boss turning to a new target does not catch a bot at its edge.
    constexpr float CONE_MARGIN = DegreesToRadians(15.0f);
    constexpr float CONE_SIDE_STEP = DegreesToRadians(30.0f);

    // The boss whose frontal cone the bot stands in, nullptr if none. The cone's own target is not counted.
    Unit* ConeOver(PlayerbotAI* botAI, AiObjectContext* context, Player* bot,
                   std::span<FrontalCone const> cones, FrontalCone const*& cone);

    // Within `radius` of the unit's edge, plus a margin.
    bool Within(Player* bot, Unit const* unit, float radius);
    float KeepAwayDistance(Unit const* unit, float radius);

    // Whether the action walks the bot somewhere other than out of danger: following, formation, reaching melee
    // or spell range. A strategy's own escape moves, "avoid aoe" and "coa boss dodge" are not counted.
    bool MovesIntoDanger(Action* action);
}

class DungeonBossMovementAction : public MovementAction
{
public:
    DungeonBossMovementAction(PlayerbotAI* ai, std::string const name) : MovementAction(ai, name) {}

protected:
    // To `angle` from the boss's facing, `distance` yards from it.
    bool MoveAround(Unit* boss, float angle, float distance);
    bool MoveOutOfCone(Unit* boss, DungeonBoss::FrontalCone const& cone);
    bool KeepAway(Unit* unit, float radius);
};

#endif
