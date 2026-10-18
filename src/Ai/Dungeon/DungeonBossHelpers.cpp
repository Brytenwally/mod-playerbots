/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "DungeonBossHelpers.h"

#include "AiObjectContext.h"
#include "Map.h"
#include "Playerbots.h"
#include "ReachTargetActions.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellInfo.h"

#include <algorithm>
#include <cstring>

namespace DungeonBoss
{
    namespace
    {
        bool NameIs(SpellInfo const* info, char const* name)
        {
            return info && info->SpellName[LOCALE_enUS] && std::strcmp(info->SpellName[LOCALE_enUS], name) == 0;
        }

        constexpr float RANGE_MARGIN = 2.0f;
    }

    bool HasAuraNamed(Unit const* unit, char const* name)
    {
        for (auto const& [spellId, application] : unit->GetAppliedAuras())
            if (NameIs(application->GetBase()->GetSpellInfo(), name))
                return true;
        return false;
    }

    bool IsCastingNamed(Unit const* unit, char const* name)
    {
        for (uint32 type : { CURRENT_GENERIC_SPELL, CURRENT_CHANNELED_SPELL })
            if (Spell* spell = unit->GetCurrentSpell(type))
                if (NameIs(spell->GetSpellInfo(), name))
                    return true;
        return false;
    }

    float RelativeAngle(Unit const* center, Unit const* unit)
    {
        float const angle = Position::NormalizeOrientation(center->GetAngle(unit) - center->GetOrientation());
        return angle > float(M_PI) ? angle - 2.0f * float(M_PI) : angle;
    }

    bool InFront(Unit const* unit, Player* bot, float reach, float halfArc)
    {
        return Within(bot, unit, reach) && std::fabs(RelativeAngle(unit, bot)) < halfArc + CONE_MARGIN;
    }

    Unit* ConeOver(PlayerbotAI* botAI, AiObjectContext* context, Player* bot,
                   std::span<FrontalCone const> cones, FrontalCone const*& cone)
    {
        for (FrontalCone const& candidate : cones)
        {
            Unit* boss = AI_VALUE2(Unit*, "find target", candidate.boss);
            if (!boss || !boss->IsAlive() || boss->GetVictim() == bot)
                continue;

            if (candidate.spell && !IsCastingNamed(boss, candidate.spell))
                continue;

            if (InFront(boss, bot, candidate.reach, candidate.halfArc))
            {
                cone = &candidate;
                return boss;
            }
        }
        return nullptr;
    }

    bool Within(Player* bot, Unit const* unit, float radius)
    {
        return bot->GetExactDist2d(unit) < KeepAwayDistance(unit, radius);
    }

    float KeepAwayDistance(Unit const* unit, float radius)
    {
        return radius + unit->GetCombatReach() + RANGE_MARGIN;
    }

    bool MovesIntoDanger(Action* action)
    {
        if (dynamic_cast<DungeonBossMovementAction*>(action) || dynamic_cast<AvoidAoeAction*>(action) ||
            action->getName() == "coa boss dodge")
            return false;

        return dynamic_cast<MovementAction*>(action) || dynamic_cast<CastReachTargetSpellAction*>(action);
    }
}

using namespace DungeonBoss;

bool DungeonBossMovementAction::MoveAround(Unit* boss, float angle, float distance)
{
    float const absolute = Position::NormalizeOrientation(boss->GetOrientation() + angle);
    float x = boss->GetPositionX() + std::cos(absolute) * distance;
    float y = boss->GetPositionY() + std::sin(absolute) * distance;
    float z = bot->GetPositionZ();
    if (!bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(),
                                                        bot->GetPositionZ(), x, y, z))
        return false;

    return MoveTo(bot->GetMapId(), x, y, z, false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
}

bool DungeonBossMovementAction::MoveOutOfCone(Unit* boss, FrontalCone const& cone)
{
    float const side = cone.halfArc + CONE_SIDE_STEP;
    float const angle = RelativeAngle(boss, bot) >= 0.0f ? side : -side;
    float const distance = std::max(bot->GetExactDist2d(boss), boss->GetCombatReach() + 2.0f);
    return MoveAround(boss, angle, distance);
}

bool DungeonBossMovementAction::KeepAway(Unit* unit, float radius)
{
    float const wanted = KeepAwayDistance(unit, radius);
    float const distance = bot->GetExactDist2d(unit);
    if (distance >= wanted)
        return false;

    return MoveAway(unit, wanted - distance);
}
