/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "SWPHelpers.h"

#include "Playerbots.h"

namespace SunwellPlateauHelpers
{

// Kalecgos
std::unordered_map<uint32, time_t> kalecgosPhaseTimer;
std::unordered_map<ObjectGuid, Position> kalecgosTankAnchorPosition;

// Returns true if the bot currently has the Spectral Realm aura, meaning it has been pulled
// into the alternate layer where Kalec and Sathrovarr fight (invisible/untargetable to bots
// still standing in the normal realm, and vice versa).
bool BotIsInKalecgosSpectralRealm(Player* bot)
{
    return bot->HasAura(static_cast<uint32>(SunwellPlateauSpells::SPELL_SPECTRAL_REALM));
}

// Returns Kalec, the friendly dragon that anchors the fight against Sathrovarr inside the
// spectral realm. Used as a rally/reference point for bots who get teleported there.
Unit* GetKalecTank(PlayerbotAI* botAI, Player* /*bot*/)
{
    return botAI->GetAiObjectContext()->GetValue<Unit*>("find target", "kalec")->Get();
}

}
