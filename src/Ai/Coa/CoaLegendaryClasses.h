/*
 * Area 52 Freepick: Legendary Mystic Enchants played as classes (docs/area52/classes/).
 *
 * A Freepick Hero (class 10 under Free Pick) has no class of its own: its picks and its Mystic Enchants make it
 * what it is. Each Legendary here is a class with a build, 17 enchant slots, a role and a rotation. The bot's
 * class is read from its slotted Legendary, so a class chosen once (randomly for a random bot, or with "talents
 * spec <spec>") stays with the character. The classes are data in the playerbots database (playerbots_legendary_*,
 * data/sql/playerbots/custom/area52_legendary_classes.sql), read once at startup: a build is changed there and the
 * server restarted, without a new build of the module.
 *
 * Everything goes through what the core exposes: AscensionFreePick::PlanSelection and Apply for the picks, the
 * player setting the core keeps the slots in, and the console command "mystic grant" for the collection. On a
 * core without Free Pick the module compiles to nothing and no player is a Hero here.
 */
#ifndef PLAYERBOTS_COALEGENDARYCLASSES_H
#define PLAYERBOTS_COALEGENDARYCLASSES_H

#include "CoaSpecStrategies.h"
#include "Define.h"
#include <array>
#include <span>
#include <string>
#include <utility>
#include <vector>

class Player;
struct ItemTemplate;

struct CoaLegendaryPick
{
    uint32 spell;  // a spell of the Free Pick entry, the rank picked
    uint32 level;  // the character level it is picked at
};

// One row of playerbots_legendary_class with its picks, enchants and stat weights (data/sql/playerbots/custom/
// area52_legendary_classes.sql), read once at startup.
struct CoaLegendaryClass
{
    std::string spec;                     // "talents spec <spec>"
    std::string name;
    uint32 legendary = 0;                 // the Legendary Mystic Enchant, also its aura
    CoaSpecRole role = CoaSpecRole::Dps;
    std::string position;                 // "close" or "ranged"
    uint32 primaryStat = 0;               // Area52Build::Entry::PrimaryStat: 0 Strength, 3 Intellect, 4 Spirit
    std::string combat;                   // custom::<class>
    std::string nonCombat;                // custom::<class>-nc
    uint32 chance = 0;                    // percent of the random bots that play it
    std::vector<CoaLegendaryPick> picks;
    std::array<uint32, 17> enchants{};
    uint32 heaviestArmor = 0;             // ITEM_SUBCLASS_ARMOR_CLOTH to ITEM_SUBCLASS_ARMOR_PLATE
    bool shield = false;
    uint32 weapons = 0;                   // bits of ITEM_SUBCLASS_WEAPON_*
    std::vector<std::pair<uint8, float>> weights;  // StatsCollector STATS_TYPE_*, weight
    uint32 pet = 0;                       // creature entry of the pet the Legendary gives, 0 for none
};

inline constexpr uint8 CoaHeroClass = 10;

// The classes from the playerbots database, read on the first call (the playerbots script calls it at startup).
std::span<CoaLegendaryClass const> CoaLegendaryClasses();

// Whether random bots are created as Heroes: AiPlayerbot.Area52HeroBots on a core with Free Pick enabled.
bool CoaHeroBotsWanted();

// Whether the core lets the bot hold this shield next to this two-handed weapon (a Hoplite's polearm).
bool CoaHeroHoldsShieldWith(Player const* bot, ItemTemplate const* main, ItemTemplate const* off);

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
// enchants again. Once its Legendary is on, a class that carries a shield next to a two-handed weapon equips one:
// the core refuses it before the Legendary is on, when the bot's gear is first chosen. Does nothing for anyone else.
void EnsureCoaLegendaryClass(Player* bot);

// Whether a Hero bot's class is not fully set up: no class, picks behind its level, the Legendary not on yet, or
// the shield of its class missing.
bool CoaLegendaryClassUnready(Player* bot);

std::string CoaLegendaryClassList(Player const* bot);

#endif
