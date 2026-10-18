/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BFDTriggers.h"

#include "AiObjectContext.h"
#include "Playerbots.h"

using namespace DungeonBoss;

namespace BlackfathomDeeps
{
    bool IsTorrenting(Unit const* ghamoora)
    {
        return ghamoora && ghamoora->IsAlive() &&
               (HasAuraNamed(ghamoora, "Torrent") || IsCastingNamed(ghamoora, "Torrent"));
    }

    bool InTorrentDanger(Unit const* ghamoora, Player* bot)
    {
        float const distance = bot->GetExactDist2d(ghamoora);
        if (distance > TORRENT_REACH + ghamoora->GetCombatReach())
            return false;

        // Inside the boss's own boundary a cone hits at any angle.
        if (distance < ghamoora->GetBoundaryRadius() + 1.0f)
            return true;

        float const angle = RelativeAngle(ghamoora, bot);
        return angle > -TORRENT_AHEAD_WINDOW && angle < TORRENT_BEHIND_WINDOW;
    }
}

using namespace BlackfathomDeeps;

bool BfdGhamooraTorrentTrigger::IsActive()
{
    Unit* ghamoora = AI_VALUE2(Unit*, "find target", "ghamoo-ra");
    return IsTorrenting(ghamoora) && InTorrentDanger(ghamoora, bot);
}

bool BfdFrontalConeTrigger::IsActive()
{
    if (botAI->IsTank(bot))
        return false;

    FrontalCone const* cone = nullptr;
    return ConeOver(botAI, context, bot, FRONTAL_CONES, cone) != nullptr;
}
