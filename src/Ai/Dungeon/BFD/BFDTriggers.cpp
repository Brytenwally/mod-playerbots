/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "BFDTriggers.h"

#include "AiObjectContext.h"
#include "Playerbots.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellInfo.h"

#include <cstring>

namespace BlackfathomDeeps
{
    namespace
    {
        bool NameIs(SpellInfo const* info, char const* name)
        {
            return info && info->SpellName[LOCALE_enUS] && std::strcmp(info->SpellName[LOCALE_enUS], name) == 0;
        }
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

    float RelativeAngle(Unit const* unit, Unit const* bot)
    {
        float const angle = Position::NormalizeOrientation(unit->GetAngle(bot) - unit->GetOrientation());
        return angle > float(M_PI) ? angle - 2.0f * float(M_PI) : angle;
    }

    bool IsTorrenting(Unit const* ghamoora)
    {
        return ghamoora && ghamoora->IsAlive() &&
               (HasAuraNamed(ghamoora, "Torrent") || IsCastingNamed(ghamoora, "Torrent"));
    }

    bool InTorrentDanger(Unit const* ghamoora, Player* bot)
    {
        float const distance = bot->GetExactDist2d(ghamoora);
        if (distance > TORRENT_REACH + ghamoora->GetCombatReach())
            return false;

        // Inside the boss's own boundary a cone hits at any angle.
        if (distance < ghamoora->GetBoundaryRadius() + 1.0f)
            return true;

        float const angle = RelativeAngle(ghamoora, bot);
        return angle > -TORRENT_AHEAD_WINDOW && angle < TORRENT_BEHIND_WINDOW;
    }

    Unit* ConeOver(PlayerbotAI* botAI, AiObjectContext* context, Player* bot, FrontalCone const*& cone)
    {
        for (FrontalCone const& candidate : FRONTAL_CONES)
        {
            Unit* boss = AI_VALUE2(Unit*, "find target", candidate.boss);
            if (!boss || !boss->IsAlive() || boss->GetVictim() == bot)
                continue;

            if (bot->GetExactDist2d(boss) > candidate.reach + boss->GetCombatReach() + 2.0f)
                continue;

            if (std::fabs(RelativeAngle(boss, bot)) < candidate.halfArc + CONE_MARGIN)
            {
                cone = &candidate;
                return boss;
            }
        }
        return nullptr;
    }
}

using namespace BlackfathomDeeps;

bool BfdGhamooraTorrentTrigger::IsActive()
{
    Unit* ghamoora = AI_VALUE2(Unit*, "find target", "ghamoo-ra");
    return IsTorrenting(ghamoora) && InTorrentDanger(ghamoora, bot);
}

bool BfdFrontalConeTrigger::IsActive()
{
    if (botAI->IsTank(bot))
        return false;

    FrontalCone const* cone = nullptr;
    return ConeOver(botAI, context, bot, cone) != nullptr;
}
