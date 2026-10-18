/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BFDActions.h"

#include "AiObjectContext.h"
#include "Map.h"
#include "Playerbots.h"

#include <algorithm>

using namespace BlackfathomDeeps;

bool BfdMoveAroundBossAction::MoveAround(Unit* boss, float angle, float distance)
{
    float const absolute = Position::NormalizeOrientation(boss->GetOrientation() + angle);
    float x = boss->GetPositionX() + std::cos(absolute) * distance;
    float y = boss->GetPositionY() + std::sin(absolute) * distance;
    float z = bot->GetPositionZ();
    if (!bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(),
                                                        bot->GetPositionZ(), x, y, z))
        return false;

    return MoveTo(bot->GetMapId(), x, y, z, false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
}

bool BfdMoveFromTorrentAction::Execute(Event /*event*/)
{
    Unit* ghamoora = AI_VALUE2(Unit*, "find target", "ghamoo-ra");
    if (!IsTorrenting(ghamoora) || !InTorrentDanger(ghamoora, bot))
        return false;

    float const angle = RelativeAngle(ghamoora, bot);
    float const distance = std::max(bot->GetExactDist2d(ghamoora), ghamoora->GetCombatReach() + 2.0f);

    // In the beam, or just behind it: out on the side it has passed. Ahead of it: further ahead, so that the
    // way out never crosses it.
    if (angle > -TORRENT_HALF_ARC)
        return MoveAround(ghamoora, TORRENT_BEHIND_ANGLE, distance);

    return MoveAround(ghamoora, angle - TORRENT_AHEAD_STEP, distance);
}

bool BfdMoveFromFrontalConeAction::Execute(Event /*event*/)
{
    FrontalCone const* cone = nullptr;
    Unit* boss = ConeOver(botAI, context, bot, cone);
    if (!boss)
        return false;

    float const side = cone->halfArc + CONE_SIDE_STEP;
    float const angle = RelativeAngle(boss, bot) >= 0.0f ? side : -side;
    float const distance = std::max(bot->GetExactDist2d(boss), boss->GetCombatReach() + 2.0f);
    return MoveAround(boss, angle, distance);
}
