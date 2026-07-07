/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "SWPTriggers.h"

#include "AiFactory.h"
#include "Playerbots.h"
#include "SWPActions.h"
#include "SWPHelpers.h"
#include "RaidBossHelpers.h"
#include "SharedDefines.h"

using namespace SunwellPlateauHelpers;

// General

bool SunwellPlateauBotIsNotInCombatTrigger::IsActive()
{
    return !bot->IsInCombat() && bot->GetMapId() == SUNWELL_PLATEAU_MAP_ID;
}

// Kalecgos

bool KalecgosPullingBossTrigger::IsActive()
{
    if (bot->getClass() != CLASS_HUNTER)
        return false;

    Unit* kalecgos = AI_VALUE2(Unit*, "find target", "kalecgos");
    return kalecgos && kalecgos->GetHealthPct() > 95.0f;
}

bool KalecgosBossEngagedByTanksTrigger::IsActive()
{
    return botAI->IsTank(bot) &&
           AI_VALUE2(Unit*, "find target", "kalecgos");
}

bool KalecgosMeleeNeedsToAvoidTailLashTrigger::IsActive()
{
    if (!botAI->IsMelee(bot) || botAI->IsTank(bot))
        return false;

    Unit* kalecgos = AI_VALUE2(Unit*, "find target", "kalecgos");
    if (!kalecgos)
        return false;

    constexpr float tailLashRange = 10.0f;
    if (bot->GetDistance2d(kalecgos) > tailLashRange)
        return false;

    // Tail Lash hits everything roughly behind the dragon; treat the rear 180 degree
    // half as the danger zone.
    return !kalecgos->HasInArc(static_cast<float>(M_PI), bot);
}

bool KalecgosBotTeleportedToSpectralRealmTrigger::IsActive()
{
    return BotIsInKalecgosSpectralRealm(bot);
}

bool KalecgosBotHasCurseOfBoundlessAgonyTrigger::IsActive()
{
    return bot->HasAura(static_cast<uint32>(SunwellPlateauSpells::SPELL_CURSE_OF_BOUNDLESS_AGONY_PLR));
}

bool KalecgosNeedToManagePhaseTimerTrigger::IsActive()
{
    if (!botAI->IsDps(bot))
        return false;

    if (!AI_VALUE2(Unit*, "find target", "kalecgos"))
        return false;

    return IsMechanicTrackerBot(botAI, bot, SUNWELL_PLATEAU_MAP_ID);
}
