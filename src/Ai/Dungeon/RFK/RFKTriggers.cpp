/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "RFKTriggers.h"

#include "AiObjectContext.h"
#include "Playerbots.h"

using namespace DungeonBoss;

namespace RazorfenKraul
{
    Unit* CharlgaCastingNova(PlayerbotAI* botAI, AiObjectContext* context)
    {
        Unit* charlga = AI_VALUE2(Unit*, "find target", "charlga razorflank");
        return charlga && charlga->IsAlive() && IsCastingNamed(charlga, "Lightning Nova") ? charlga : nullptr;
    }

    bool LeavesNova(PlayerbotAI* botAI, Player* bot)
    {
        return !botAI->IsTank(bot) && botAI->IsRanged(bot);
    }
}

using namespace RazorfenKraul;

bool RfkCharlgaLightningNovaTrigger::IsActive()
{
    if (!LeavesNova(botAI, bot))
        return false;

    Unit* charlga = CharlgaCastingNova(botAI, context);
    return charlga && Within(bot, charlga, LIGHTNING_NOVA_RADIUS);
}

bool RfkFrontalConeTrigger::IsActive()
{
    if (botAI->IsTank(bot))
        return false;

    FrontalCone const* cone = nullptr;
    return ConeOver(botAI, context, bot, FRONTAL_CONES, cone) != nullptr;
}
