/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BFDMultipliers.h"

#include "AiObjectContext.h"
#include "BFDActions.h"
#include "BFDTriggers.h"
#include "MovementActions.h"
#include "Playerbots.h"
#include "ReachTargetActions.h"

using namespace BlackfathomDeeps;

float GhamooraMultiplier::GetValue(Action* action)
{
    if (!dynamic_cast<MovementAction*>(action) || dynamic_cast<BfdMoveAroundBossAction*>(action) ||
        dynamic_cast<ReachTargetAction*>(action) || dynamic_cast<AvoidAoeAction*>(action) ||
        action->getName() == "coa boss dodge")
        return 1.0f;

    // While the beam turns, following, formation and getting behind the boss would walk a bot across it. Moving
    // straight in to the boss keeps the bot's angle and stays allowed.
    Unit* ghamoora = AI_VALUE2(Unit*, "find target", "ghamoo-ra");
    if (!IsTorrenting(ghamoora) || bot->GetExactDist2d(ghamoora) > TORRENT_REACH + ghamoora->GetCombatReach())
        return 1.0f;

    return 0.0f;
}
