/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RFKMultipliers.h"

#include "AiObjectContext.h"
#include "Playerbots.h"
#include "RFKTriggers.h"

using namespace DungeonBoss;
using namespace RazorfenKraul;

float CharlgaRazorflankMultiplier::GetValue(Action* action)
{
    if (!MovesIntoDanger(action) || !LeavesNova(botAI, bot))
        return 1.0f;

    // Back into spell or healing range only once the Nova is over.
    return CharlgaCastingNova(botAI, context) ? 0.0f : 1.0f;
}
