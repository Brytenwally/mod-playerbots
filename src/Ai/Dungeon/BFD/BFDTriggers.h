/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_BFDTRIGGERS_H
#define PLAYERBOTS_BFDTRIGGERS_H

#include "DungeonBossHelpers.h"
#include "GenericTriggers.h"
#include "Trigger.h"

// Blackfathom Deeps (map 48).
namespace BlackfathomDeeps
{
    using DungeonBoss::DegreesToRadians;

    // Ghamoo-ra's Torrent (2102687): a 3 s cast, then 20.5 s in which the boss turns clockwise once around itself
    // (spell_coa_torrent_spin) and every 0.25 s fires a 24 degree cone of 50 yd that hits and knocks back (2102688).
    constexpr float TORRENT_REACH = 50.0f;
    constexpr float TORRENT_HALF_ARC = DegreesToRadians(12.0f);
    constexpr float TORRENT_TURN_PER_SECOND = 2.0f * float(M_PI) / 20.5f;
    // The beam turns towards negative angles: a bot that far ahead of it has two seconds left.
    constexpr float TORRENT_AHEAD_WINDOW = TORRENT_HALF_ARC + 2.0f * TORRENT_TURN_PER_SECOND;
    constexpr float TORRENT_BEHIND_WINDOW = TORRENT_HALF_ARC + DegreesToRadians(8.0f);
    // Where a bot caught in the beam goes: behind it, where the beam has just passed.
    constexpr float TORRENT_BEHIND_ANGLE = DegreesToRadians(40.0f);
    // How far a bot the beam is about to reach steps ahead of it, without crossing it.
    constexpr float TORRENT_AHEAD_STEP = DegreesToRadians(60.0f);

    // Cast at the tank. Forked Lightning (8435, Lady Sarevess, a 2 s cast every 2.5-3 s) reaches 30 yd, Gelihast's
    // Cone of Cold (2102699, instant every 8 s) 15 yd; both are 24 degrees wide.
    constexpr DungeonBoss::FrontalCone FRONTAL_CONES[] = {
        { "lady sarevess", 30.0f, DegreesToRadians(12.0f), nullptr },
        { "gelihast", 15.0f, DegreesToRadians(12.0f), nullptr },
    };

    bool IsTorrenting(Unit const* ghamoora);
    bool InTorrentDanger(Unit const* ghamoora, Player* bot);
}

class BfdGhamooraTorrentTrigger : public Trigger
{
public:
    BfdGhamooraTorrentTrigger(PlayerbotAI* ai) : Trigger(ai, "bfd ghamoo-ra torrent") {}
    bool IsActive() override;
};

class BfdFrontalConeTrigger : public Trigger
{
public:
    BfdFrontalConeTrigger(PlayerbotAI* ai) : Trigger(ai, "bfd frontal cone") {}
    bool IsActive() override;
};

#endif
