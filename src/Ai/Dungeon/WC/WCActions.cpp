/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WCActions.h"

#include "AiObjectContext.h"
#include "Playerbots.h"

#include <cmath>

using namespace DungeonBoss;
using namespace WailingCaverns;

bool WcMoveFromOverrunAction::Execute(Event /*event*/)
{
    Unit* skum = AI_VALUE2(Unit*, "find target", "skum");
    if (!IsOverrunning(skum) || !InOverrunPath(skum, bot))
        return false;

    // Straight out to the nearer side of the path, keeping the bot's place along it.
    float const distance = skum->GetExactDist2d(bot);
    float const angle = RelativeAngle(skum, bot);
    float const ahead = distance * std::cos(angle);
    float const side = OverrunHalfWidth(skum, bot) + OVERRUN_SIDE_STEP;
    float const aside = angle >= 0.0f ? side : -side;
    return MoveAround(skum, std::atan2(aside, ahead), std::hypot(ahead, aside));
}
