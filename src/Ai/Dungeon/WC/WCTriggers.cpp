/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "WCTriggers.h"

#include "AiObjectContext.h"
#include "Playerbots.h"

#include <cmath>

using namespace DungeonBoss;

namespace WailingCaverns
{
    bool IsOverrunning(Unit const* skum)
    {
        return skum && skum->IsAlive() && IsCastingNamed(skum, "Overrun");
    }

    float OverrunHalfWidth(Unit const* skum, Unit const* bot)
    {
        // The trample's range check counts both bodies.
        return OVERRUN_RADIUS + skum->GetBoundaryRadius() + bot->GetBoundaryRadius() + OVERRUN_MARGIN;
    }

    bool InOverrunPath(Unit const* skum, Unit const* bot)
    {
        float const distance = skum->GetExactDist2d(bot);
        float const angle = RelativeAngle(skum, bot);
        float const ahead = distance * std::cos(angle);
        float const aside = distance * std::sin(angle);
        float const halfWidth = OverrunHalfWidth(skum, bot);
        return ahead > -halfWidth && ahead < OVERRUN_LENGTH + halfWidth && std::fabs(aside) < halfWidth;
    }
}

using namespace WailingCaverns;

bool WcSkumOverrunTrigger::IsActive()
{
    Unit* skum = AI_VALUE2(Unit*, "find target", "skum");
    return IsOverrunning(skum) && InOverrunPath(skum, bot);
}
