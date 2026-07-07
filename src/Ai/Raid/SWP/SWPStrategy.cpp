/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "SWPStrategy.h"

#include "SWPMultipliers.h"

void RaidSunwellPlateauStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // General
    triggers.push_back(new TriggerNode("sunwell plateau bot is not in combat", {
        NextAction("sunwell plateau erase timers and trackers", ACTION_EMERGENCY + 11) }));

    // Kalecgos
    triggers.push_back(new TriggerNode("kalecgos pulling boss", {
        NextAction("kalecgos misdirect boss to main tank", ACTION_RAID + 2) }));

    triggers.push_back(new TriggerNode("kalecgos boss engaged by tanks", {
        NextAction("kalecgos tanks position boss", ACTION_RAID + 1) }));

    triggers.push_back(new TriggerNode("kalecgos melee needs to avoid tail lash", {
        NextAction("kalecgos melee reposition away from tail lash", ACTION_EMERGENCY + 1) }));

    triggers.push_back(new TriggerNode("kalecgos bot teleported to spectral realm", {
        NextAction("kalecgos engage spectral realm fight", ACTION_RAID + 3) }));

    triggers.push_back(new TriggerNode("kalecgos bot has curse of boundless agony", {
        NextAction("kalecgos move away from spectral group", ACTION_EMERGENCY + 6) }));

    triggers.push_back(new TriggerNode("kalecgos need to manage phase timer", {
        NextAction("kalecgos manage phase timer", ACTION_EMERGENCY + 10) }));
}

void RaidSunwellPlateauStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    // Kalecgos
    multipliers.push_back(new KalecgosDelayDpsCooldownsMultiplier(botAI));
    multipliers.push_back(new KalecgosDisableNormalRealmActionsMultiplier(botAI));
    multipliers.push_back(new KalecgosCurseCarrierOnlyMoveAwayMultiplier(botAI));
}
