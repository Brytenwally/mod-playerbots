/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#ifndef PLAYERBOTS_SWPTRIGGERS_H
#define PLAYERBOTS_SWPTRIGGERS_H

#include "Trigger.h"

// General

class SunwellPlateauBotIsNotInCombatTrigger : public Trigger
{
public:
    SunwellPlateauBotIsNotInCombatTrigger(
        PlayerbotAI* botAI) : Trigger(botAI, "sunwell plateau bot is not in combat") {}
    bool IsActive() override;
};

// Kalecgos

class KalecgosPullingBossTrigger : public Trigger
{
public:
    KalecgosPullingBossTrigger(
        PlayerbotAI* botAI) : Trigger(botAI, "kalecgos pulling boss") {}
    bool IsActive() override;
};

class KalecgosBossEngagedByTanksTrigger : public Trigger
{
public:
    KalecgosBossEngagedByTanksTrigger(
        PlayerbotAI* botAI) : Trigger(botAI, "kalecgos boss engaged by tanks") {}
    bool IsActive() override;
};

class KalecgosMeleeNeedsToAvoidTailLashTrigger : public Trigger
{
public:
    KalecgosMeleeNeedsToAvoidTailLashTrigger(
        PlayerbotAI* botAI) : Trigger(botAI, "kalecgos melee needs to avoid tail lash") {}
    bool IsActive() override;
};

class KalecgosBotTeleportedToSpectralRealmTrigger : public Trigger
{
public:
    KalecgosBotTeleportedToSpectralRealmTrigger(
        PlayerbotAI* botAI) : Trigger(botAI, "kalecgos bot teleported to spectral realm") {}
    bool IsActive() override;
};

class KalecgosBotHasCurseOfBoundlessAgonyTrigger : public Trigger
{
public:
    KalecgosBotHasCurseOfBoundlessAgonyTrigger(
        PlayerbotAI* botAI) : Trigger(botAI, "kalecgos bot has curse of boundless agony") {}
    bool IsActive() override;
};

class KalecgosNeedToManagePhaseTimerTrigger : public Trigger
{
public:
    KalecgosNeedToManagePhaseTimerTrigger(
        PlayerbotAI* botAI) : Trigger(botAI, "kalecgos need to manage phase timer") {}
    bool IsActive() override;
};

#endif
