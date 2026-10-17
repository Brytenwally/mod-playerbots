/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_SMTRIGGERS_H
#define PLAYERBOTS_SMTRIGGERS_H

#include "DungeonStrategyUtils.h"
#include "GenericTriggers.h"
#include "Trigger.h"

// Scarlet Monastery (map 189, all four wings). CoA runs the same boss kits on Normal, Heroic and Mythic; only
// health and damage change, and the Heroic/Mythic spells keep their names, so the spells are matched by name.
namespace ScarletMonastery
{
    // Arcanist Doan: at half health he shields himself in Arcane Bubble and starts Detonation, a large arcane
    // blast around him.
    constexpr float DETONATION_SAFE_DISTANCE = 24.0f;

    // Herod's Whirlwind hits everyone next to him for its whole duration.
    constexpr float WHIRLWIND_SAFE_DISTANCE = 12.0f;

    bool HasAuraNamed(Unit const* unit, char const* name);
    bool IsCastingNamed(Unit const* unit, char const* name);

    bool IsDetonating(Unit const* doan);
    bool IsWhirlwinding(Unit const* herod);

    // Whitemane while Mograine stands again beside her: she is the one to kill first.
    Unit* WhitemaneToFocus(PlayerbotAI* botAI, AiObjectContext* context);
}

class SmDoanDetonationTrigger : public Trigger
{
public:
    SmDoanDetonationTrigger(PlayerbotAI* ai) : Trigger(ai, "sm doan detonation") {}
    bool IsActive() override;
};

class SmHerodWhirlwindTrigger : public Trigger
{
public:
    SmHerodWhirlwindTrigger(PlayerbotAI* ai) : Trigger(ai, "sm herod whirlwind") {}
    bool IsActive() override;
};

class SmWhitemaneFocusTrigger : public Trigger
{
public:
    SmWhitemaneFocusTrigger(PlayerbotAI* ai) : Trigger(ai, "sm whitemane focus") {}
    bool IsActive() override;
};

#endif
