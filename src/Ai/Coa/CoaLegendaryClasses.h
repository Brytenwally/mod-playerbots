/*
 * Area 52 Freepick: Legendary Mystic Enchants played as classes (docs/area52/classes/).
 *
 * A Freepick Hero (class 10 under Free Pick) has no class of its own: its picks and its Mystic Enchants make it
 * what it is. Each Legendary here is a class with a build, 17 enchant slots, a role and a rotation. The bot's
 * class is read from its slotted Legendary, so a class chosen once (randomly for a random bot, or with "talents
 * spec <spec>") stays with the character.
 *
 * Everything goes through what the core exposes: AscensionFreePick::PlanSelection and Apply for the picks, the
 * player setting the core keeps the slots in, and the console command "mystic grant" for the collection. On a
 * core without Free Pick the module compiles to nothing and no player is a Hero here.
 */
#ifndef PLAYERBOTS_COALEGENDARYCLASSES_H
#define PLAYERBOTS_COALEGENDARYCLASSES_H

#include "CoaLegendaryClassData.h"
#include "CoaSpecStrategies.h"
#include "Define.h"
#include <array>
#include <span>
#include <string>
#include <utility>

class Player;

struct CoaLegendaryClass
{
    char const* spec;                     // "talents spec <spec>"
    char const* name;
    uint32 legendary;                     // the Legendary Mystic Enchant, also its aura
    CoaSpecRole role;
    char const* position;                 // "close" or "ranged"
    uint32 primaryStat;                   // Area52Build::Entry::PrimaryStat: 0 Strength, 3 Intellect, 4 Spirit
    char const* combat;                   // custom::<class>
    char const* nonCombat;                // custom::<class>-nc
    uint32 chance;                        // percent of the random bots that play it
    std::span<CoaLegendaryPick const> picks;
    std::array<uint32, 17> const* enchants;
    uint32 heaviestArmor;                 // ITEM_SUBCLASS_ARMOR_CLOTH to ITEM_SUBCLASS_ARMOR_PLATE
    bool shield;
    uint32 weapons;                       // bits of ITEM_SUBCLASS_WEAPON_*
    std::array<std::pair<uint8, float>, 6> weights;  // StatsCollector STATS_TYPE_*, weight
};

inline constexpr uint8 CoaHeroClass = 10;

std::span<CoaLegendaryClass const> CoaLegendaryClasses();

// Whether random bots are created as Heroes: AiPlayerbot.Area52HeroBots on a core with Free Pick enabled.
bool CoaHeroBotsWanted();

// A Freepick Hero, whose class comes from its Legendary.
bool IsCoaHero(Player const* player);

// The class of the Legendary slotted in the bot's active preset, or null.
CoaLegendaryClass const* GetCoaLegendaryClass(Player const* player);

CoaLegendaryClass const* FindCoaLegendaryClass(std::string const& spec);

// Takes the picks of the class for the bot's level, slots its enchants and asks the core for the whole
// collection (all Mystic Enchants, the owner's rule for bots). False with the reason when the core refused.
bool ApplyCoaLegendaryClass(Player* bot, CoaLegendaryClass const& legendaryClass, std::string& error);

// A Hero bot without a class gets one: random bots by chance (20% tank, 20% healer, 60% damage), the same one
// for a bot every time. A Hero with a class gets its picks for its level and, until its Legendary is on, its
// enchants again. Does nothing for anyone else.
void EnsureCoaLegendaryClass(Player* bot);

// Whether a Hero bot's class is not fully set up: no class, picks behind its level, or the Legendary not on yet.
bool CoaLegendaryClassUnready(Player* bot);

std::string CoaLegendaryClassList(Player const* bot);

#endif
