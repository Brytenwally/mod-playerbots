/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#ifndef PLAYERBOTS_SWPHELPERS_H
#define PLAYERBOTS_SWPHELPERS_H

#include <unordered_map>
#include <unordered_set>

#include "Common.h"
#include "ObjectGuid.h"
#include "Position.h"

class Player;
class PlayerbotAI;
class Unit;

namespace SunwellPlateauHelpers
{

enum class SunwellPlateauSpells : uint32
{
    // Kalecgos
    SPELL_SPECTRAL_EXHAUSTION       = 44867,
    SPELL_SPECTRAL_BLAST            = 44869,
    SPELL_SPECTRAL_BLAST_PORTAL     = 44866,
    SPELL_TELEPORT_SPECTRAL         = 46019,
    SPELL_TELEPORT_NORMAL_REALM     = 46020,
    SPELL_SPECTRAL_REALM            = 46021,
    SPELL_ARCANE_BUFFET             = 45018,
    SPELL_FROST_BREATH              = 44799,
    SPELL_TAIL_LASH                 = 45122,
    SPELL_BANISH_KALECGOS           = 44836,
    SPELL_CRAZED_RAGE               = 44807,
    SPELL_CURSE_OF_BOUNDLESS_AGONY_PLR = 45034,

    // Hunter
    SPELL_MISDIRECTION               = 35079,
};

enum class SunwellPlateauNpcs : uint32
{
    NPC_KALECGOS   = 24850,
    NPC_KALEC      = 24891,
    NPC_SATHROVARR = 24892,
};

constexpr uint32 SUNWELL_PLATEAU_MAP_ID = 580;

// Kalecgos
extern std::unordered_map<uint32, time_t> kalecgosPhaseTimer;
extern std::unordered_map<ObjectGuid, Position> kalecgosTankAnchorPosition;
bool BotIsInKalecgosSpectralRealm(Player* bot);
Unit* GetKalecTank(PlayerbotAI* botAI, Player* bot);

}

#endif
