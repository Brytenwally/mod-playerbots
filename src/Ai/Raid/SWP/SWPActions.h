/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#ifndef PLAYERBOTS_SWPACTIONS_H
#define PLAYERBOTS_SWPACTIONS_H

#include "AttackAction.h"
#include "MovementActions.h"

// General

class SunwellPlateauEraseTimersAndTrackersAction : public Action
{
public:
    SunwellPlateauEraseTimersAndTrackersAction(
        PlayerbotAI* botAI) : Action(botAI, "sunwell plateau erase timers and trackers") {}
    bool Execute(Event event) override;
};

// Kalecgos

class KalecgosMisdirectBossToMainTankAction : public AttackAction
{
public:
    KalecgosMisdirectBossToMainTankAction(
        PlayerbotAI* botAI) : AttackAction(botAI, "kalecgos misdirect boss to main tank") {}
    bool Execute(Event event) override;
};

class KalecgosTanksPositionBossAction : public MovementAction
{
public:
    KalecgosTanksPositionBossAction(
        PlayerbotAI* botAI) : MovementAction(botAI, "kalecgos tanks position boss") {}
    bool Execute(Event event) override;
};

class KalecgosMeleeRepositionAwayFromTailLashAction : public MovementAction
{
public:
    KalecgosMeleeRepositionAwayFromTailLashAction(
        PlayerbotAI* botAI) : MovementAction(botAI, "kalecgos melee reposition away from tail lash") {}
    bool Execute(Event event) override;
};

class KalecgosEngageSpectralRealmFightAction : public AttackAction
{
public:
    KalecgosEngageSpectralRealmFightAction(
        PlayerbotAI* botAI) : AttackAction(botAI, "kalecgos engage spectral realm fight") {}
    bool Execute(Event event) override;
};

class KalecgosMoveAwayFromSpectralGroupAction : public MovementAction
{
public:
    KalecgosMoveAwayFromSpectralGroupAction(
        PlayerbotAI* botAI) : MovementAction(botAI, "kalecgos move away from spectral group") {}
    bool Execute(Event event) override;
};

class KalecgosManagePhaseTimerAction : public Action
{
public:
    KalecgosManagePhaseTimerAction(
        PlayerbotAI* botAI) : Action(botAI, "kalecgos manage phase timer") {}
    bool Execute(Event event) override;
};

#endif
