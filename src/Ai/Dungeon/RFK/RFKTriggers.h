/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_RFKTRIGGERS_H
#define PLAYERBOTS_RFKTRIGGERS_H

#include "DungeonBossHelpers.h"
#include "GenericTriggers.h"
#include "Trigger.h"

// Razorfen Kraul (map 47).
namespace RazorfenKraul
{
    using DungeonBoss::DegreesToRadians;

    // Charlga Razorflank's Lightning Nova (2102774): a 6 s cast that hits everyone within 40 yd. Melee and the
    // tank stay to interrupt it; casters and healers have the time to leave.
    constexpr float LIGHTNING_NOVA_RADIUS = 40.0f;

    // Death Speaker Jargba's Shadow Bolt Volley (2102781): a 3 s cast at the tank, a 24 degree cone of 50 yd.
    constexpr DungeonBoss::FrontalCone FRONTAL_CONES[] = {
        { "death speaker jargba", 50.0f, DegreesToRadians(12.0f), "Shadow Bolt Volley" },
    };

    Unit* CharlgaCastingNova(PlayerbotAI* botAI, AiObjectContext* context);
    bool LeavesNova(PlayerbotAI* botAI, Player* bot);
}

class RfkCharlgaLightningNovaTrigger : public Trigger
{
public:
    RfkCharlgaLightningNovaTrigger(PlayerbotAI* ai) : Trigger(ai, "rfk charlga lightning nova") {}
    bool IsActive() override;
};

class RfkFrontalConeTrigger : public Trigger
{
public:
    RfkFrontalConeTrigger(PlayerbotAI* ai) : Trigger(ai, "rfk frontal cone") {}
    bool IsActive() override;
};

#endif
