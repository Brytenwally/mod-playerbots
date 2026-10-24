/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef _PLAYERBOT_COATALENTCOMPAT_H
#define _PLAYERBOT_COATALENTCOMPAT_H

#include "AscensionCoATalentData.h"
#include "AscensionCoATalentState.h"
#include "AscensionSpecialization.h"

#include <array>
#include <cstdint>
#include <vector>

// Talent data that differs between CoA cores. Upstream CoA marks talent-window entries, lists prerequisites on each
// entry and returns a player's learned entries; cores without them (Area 52) get the same answers from the spells the
// player knows, and entries without a talent flag or prerequisites.
template <class PlayerType>
std::vector<AscensionCoATalentState::KnownEntry> CoaKnownTalentEntries(PlayerType const* player)
{
    if constexpr (requires { GetAscensionKnownTalentEntries(player); })
        return GetAscensionKnownTalentEntries(player);
    else
    {
        if (!player)
            return {};
        return AscensionCoATalentState::KnownEntries(player->getClass(),
            [player](std::uint32_t spell) { return player->HasSpell(spell); });
    }
}

template <class Entry>
bool CoaEntryIsTalent(Entry const& entry)
{
    if constexpr (requires { entry.Talent; })
        return entry.Talent;
    else
        return false;
}

template <class Entry>
std::array<std::uint32_t, 3> CoaRequiredEntries(Entry const& entry)
{
    std::array<std::uint32_t, 3> required{};
    if constexpr (requires { entry.RequiredEntryIds; })
        for (std::size_t index = 0; index < required.size() && index < entry.RequiredEntryIds.size(); ++index)
            required[index] = entry.RequiredEntryIds[index];
    return required;
}

#endif
