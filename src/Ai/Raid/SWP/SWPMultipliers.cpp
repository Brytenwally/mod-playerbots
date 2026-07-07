/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "SWPMultipliers.h"

#include "ChooseTargetActions.h"
#include "DKActions.h"
#include "DruidActions.h"
#include "HunterActions.h"
#include "MageActions.h"
#include "PaladinActions.h"
#include "PriestActions.h"
#include "SWPActions.h"
#include "SWPHelpers.h"
#include "RogueActions.h"
#include "ShamanActions.h"
#include "WarlockActions.h"
#include "WarriorActions.h"

using namespace SunwellPlateauHelpers;

static bool IsDpsCooldownAction(Action* action)
{
    return dynamic_cast<CastHeroismAction*>(action) ||
           dynamic_cast<CastBloodlustAction*>(action) ||
           dynamic_cast<CastMetamorphosisAction*>(action) ||
           dynamic_cast<CastAdrenalineRushAction*>(action) ||
           dynamic_cast<CastBladeFlurryAction*>(action) ||
           dynamic_cast<CastIcyVeinsAction*>(action) ||
           dynamic_cast<CastColdSnapAction*>(action) ||
           dynamic_cast<CastArcanePowerAction*>(action) ||
           dynamic_cast<CastPresenceOfMindAction*>(action) ||
           dynamic_cast<CastCombustionAction*>(action) ||
           dynamic_cast<CastRapidFireAction*>(action) ||
           dynamic_cast<CastReadinessAction*>(action) ||
           dynamic_cast<CastAvengingWrathAction*>(action) ||
           dynamic_cast<CastElementalMasteryAction*>(action) ||
           dynamic_cast<CastFeralSpiritAction*>(action) ||
           dynamic_cast<CastFireElementalTotemAction*>(action) ||
           dynamic_cast<CastFireElementalTotemMeleeAction*>(action) ||
           dynamic_cast<CastForceOfNatureAction*>(action) ||
           dynamic_cast<CastArmyOfTheDeadAction*>(action) ||
           dynamic_cast<CastSummonGargoyleAction*>(action) ||
           dynamic_cast<CastBerserkingAction*>(action) ||
           dynamic_cast<CastBloodFuryAction*>(action);
}

// Kalecgos

float KalecgosDelayDpsCooldownsMultiplier::GetValue(Action* action)
{
    Unit* kalecgos = AI_VALUE2(Unit*, "find target", "kalecgos");
    if (!kalecgos || kalecgos->GetHealthPct() < 95.0f)
        return 1.0f;

    if (IsDpsCooldownAction(action) ||
        (botAI->IsDps(bot) && dynamic_cast<UseTrinketAction*>(action)))
    {
        return 0.0f;
    }

    return 1.0f;
}

float KalecgosDisableNormalRealmActionsMultiplier::GetValue(Action* action)
{
    if (!BotIsInKalecgosSpectralRealm(bot))
        return 1.0f;

    // While phased into the spectral realm, Kalecgos is invisible and untargetable;
    // stop the bot from wasting priority on tank positioning or misdirects meant for it.
    if (dynamic_cast<KalecgosTanksPositionBossAction*>(action) ||
        dynamic_cast<KalecgosMisdirectBossToMainTankAction*>(action) ||
        dynamic_cast<KalecgosMeleeRepositionAwayFromTailLashAction*>(action))
    {
        return 0.0f;
    }

    return 1.0f;
}

float KalecgosCurseCarrierOnlyMoveAwayMultiplier::GetValue(Action* action)
{
    if (!bot->HasAura(static_cast<uint32>(SunwellPlateauSpells::SPELL_CURSE_OF_BOUNDLESS_AGONY_PLR)))
        return 1.0f;

    if (dynamic_cast<MovementAction*>(action) &&
        !dynamic_cast<KalecgosMoveAwayFromSpectralGroupAction*>(action))
    {
        return 0.0f;
    }

    return 1.0f;
}
