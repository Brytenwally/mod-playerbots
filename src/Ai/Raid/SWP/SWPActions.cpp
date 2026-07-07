/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "SWPActions.h"

#include <cmath>

#include "Playerbots.h"
#include "SWPHelpers.h"
#include "RaidBossHelpers.h"

using namespace SunwellPlateauHelpers;

// General

bool SunwellPlateauEraseTimersAndTrackersAction::Execute(Event /*event*/)
{
    if (AI_VALUE2(Unit*, "find target", "kalecgos"))
        return false;

    const uint32 instanceId = bot->GetMap()->GetInstanceId();
    const ObjectGuid guid = bot->GetGUID();

    bool erased = false;
    if (kalecgosPhaseTimer.erase(instanceId) > 0)
        erased = true;
    if (kalecgosTankAnchorPosition.erase(guid) > 0)
        erased = true;

    return erased;
}

// Kalecgos

bool KalecgosMisdirectBossToMainTankAction::Execute(Event /*event*/)
{
    Unit* kalecgos = AI_VALUE2(Unit*, "find target", "kalecgos");
    if (!kalecgos)
        return false;

    Player* mainTank = GetGroupMainTank(botAI, bot);
    if (!mainTank)
        return false;

    if (botAI->CanCastSpell("misdirection", mainTank))
        return botAI->CastSpell("misdirection", mainTank);

    if (bot->HasAura(static_cast<uint32>(SunwellPlateauSpells::SPELL_MISDIRECTION)) &&
        botAI->CanCastSpell("steady shot", kalecgos))
    {
        return botAI->CastSpell("steady shot", kalecgos);
    }

    return false;
}

bool KalecgosTanksPositionBossAction::Execute(Event /*event*/)
{
    Unit* kalecgos = AI_VALUE2(Unit*, "find target", "kalecgos");
    if (!kalecgos)
        return false;

    if (AI_VALUE(Unit*, "current target") != kalecgos)
        return Attack(kalecgos);

    if (kalecgos->GetVictim() != bot || !bot->IsWithinMeleeRange(kalecgos))
        return false;

    const ObjectGuid guid = bot->GetGUID();

    // Remember the spot where the tank first locked the boss down, so Kalecgos doesn't
    // get walked/kited away from a stable anchor point over the course of the fight.
    auto it = kalecgosTankAnchorPosition.find(guid);
    if (it == kalecgosTankAnchorPosition.end())
    {
        kalecgosTankAnchorPosition[guid] = bot->GetPosition();
        return false;
    }

    const Position& anchor = it->second;
    const float distToAnchor = bot->GetExactDist2d(anchor.GetPositionX(), anchor.GetPositionY());
    if (distToAnchor <= 3.0f)
        return false;

    return MoveTo(bot->GetMapId(), anchor.GetPositionX(), anchor.GetPositionY(),
                  anchor.GetPositionZ(), false, false, false, false,
                  MovementPriority::MOVEMENT_FORCED, true, false);
}

bool KalecgosMeleeRepositionAwayFromTailLashAction::Execute(Event /*event*/)
{
    Unit* kalecgos = AI_VALUE2(Unit*, "find target", "kalecgos");
    if (!kalecgos)
        return false;

    // Sidestep to the dragon's flank, just inside its forward arc, so melee keeps hitting
    // it without eating Tail Lash from directly behind.
    constexpr float meleeRange = 4.0f;
    const float sideAngle = kalecgos->GetOrientation() + ANGLE_90_DEG;
    const float destX = kalecgos->GetPositionX() + meleeRange * std::cos(sideAngle);
    const float destY = kalecgos->GetPositionY() + meleeRange * std::sin(sideAngle);

    return MoveTo(bot->GetMapId(), destX, destY, kalecgos->GetPositionZ(), false, false,
                  false, false, MovementPriority::MOVEMENT_FORCED, true, false);
}

bool KalecgosEngageSpectralRealmFightAction::Execute(Event /*event*/)
{
    Unit* sathrovarr = AI_VALUE2(Unit*, "find target", "sathrovarr");
    if (!sathrovarr)
        return false;

    if (AI_VALUE(Unit*, "current target") != sathrovarr)
        return Attack(sathrovarr);

    Unit* kalec = GetKalecTank(botAI, bot);
    if (kalec && bot->GetDistance(kalec) > 20.0f)
        return MoveTo(kalec, sPlayerbotAIConfig.followDistance);

    return false;
}

bool KalecgosMoveAwayFromSpectralGroupAction::Execute(Event /*event*/)
{
    constexpr float safeDistance = 10.0f;
    constexpr uint32 minInterval = 1000;
    if (Unit* nearestPlayer = GetNearestPlayerInRadius(bot, safeDistance))
        return FleePosition(nearestPlayer->GetPosition(), safeDistance, minInterval);

    return false;
}

bool KalecgosManagePhaseTimerAction::Execute(Event /*event*/)
{
    Unit* kalecgos = AI_VALUE2(Unit*, "find target", "kalecgos");
    if (!kalecgos)
        return false;

    kalecgosPhaseTimer.try_emplace(
        kalecgos->GetMap()->GetInstanceId(), std::time(nullptr));

    return false;
}
