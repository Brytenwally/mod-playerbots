/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#ifndef PLAYERBOTS_SWPACTIONCONTEXT_H
#define PLAYERBOTS_SWPACTIONCONTEXT_H

#include "NamedObjectContext.h"
#include "SWPActions.h"

class RaidSunwellPlateauActionContext : public NamedObjectContext<Action>
{
public:
    RaidSunwellPlateauActionContext()
    {
        // General
        creators["sunwell plateau erase timers and trackers"] =
            &RaidSunwellPlateauActionContext::sunwell_plateau_erase_timers_and_trackers;

        // Kalecgos
        creators["kalecgos misdirect boss to main tank"] =
            &RaidSunwellPlateauActionContext::kalecgos_misdirect_boss_to_main_tank;

        creators["kalecgos tanks position boss"] =
            &RaidSunwellPlateauActionContext::kalecgos_tanks_position_boss;

        creators["kalecgos melee reposition away from tail lash"] =
            &RaidSunwellPlateauActionContext::kalecgos_melee_reposition_away_from_tail_lash;

        creators["kalecgos engage spectral realm fight"] =
            &RaidSunwellPlateauActionContext::kalecgos_engage_spectral_realm_fight;

        creators["kalecgos move away from spectral group"] =
            &RaidSunwellPlateauActionContext::kalecgos_move_away_from_spectral_group;

        creators["kalecgos manage phase timer"] =
            &RaidSunwellPlateauActionContext::kalecgos_manage_phase_timer;
    }

private:
    // General
    static Action* sunwell_plateau_erase_timers_and_trackers(
        PlayerbotAI* botAI) { return new SunwellPlateauEraseTimersAndTrackersAction(botAI); }

    // Kalecgos
    static Action* kalecgos_misdirect_boss_to_main_tank(
        PlayerbotAI* botAI) { return new KalecgosMisdirectBossToMainTankAction(botAI); }

    static Action* kalecgos_tanks_position_boss(
        PlayerbotAI* botAI) { return new KalecgosTanksPositionBossAction(botAI); }

    static Action* kalecgos_melee_reposition_away_from_tail_lash(
        PlayerbotAI* botAI) { return new KalecgosMeleeRepositionAwayFromTailLashAction(botAI); }

    static Action* kalecgos_engage_spectral_realm_fight(
        PlayerbotAI* botAI) { return new KalecgosEngageSpectralRealmFightAction(botAI); }

    static Action* kalecgos_move_away_from_spectral_group(
        PlayerbotAI* botAI) { return new KalecgosMoveAwayFromSpectralGroupAction(botAI); }

    static Action* kalecgos_manage_phase_timer(
        PlayerbotAI* botAI) { return new KalecgosManagePhaseTimerAction(botAI); }
};

#endif
