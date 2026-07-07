/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#ifndef PLAYERBOTS_SWPMULTIPLIERS_H
#define PLAYERBOTS_SWPMULTIPLIERS_H

#include "Multiplier.h"

// Kalecgos

class KalecgosDelayDpsCooldownsMultiplier : public Multiplier
{
public:
    KalecgosDelayDpsCooldownsMultiplier(
        PlayerbotAI* botAI) : Multiplier(botAI, "kalecgos delay dps cooldowns multiplier") {}
    virtual float GetValue(Action* action);
};

class KalecgosDisableNormalRealmActionsMultiplier : public Multiplier
{
public:
    KalecgosDisableNormalRealmActionsMultiplier(
        PlayerbotAI* botAI) : Multiplier(botAI, "kalecgos disable normal realm actions multiplier") {}
    virtual float GetValue(Action* action);
};

class KalecgosCurseCarrierOnlyMoveAwayMultiplier : public Multiplier
{
public:
    KalecgosCurseCarrierOnlyMoveAwayMultiplier(
        PlayerbotAI* botAI) : Multiplier(botAI, "kalecgos curse carrier only move away multiplier") {}
    virtual float GetValue(Action* action);
};

#endif
