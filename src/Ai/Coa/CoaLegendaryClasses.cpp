#include "CoaLegendaryClasses.h"

#include "Config.h"
#include "IWorld.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Log.h"
#include "Player.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotFactory.h"
#include "RandomPlayerbotMgr.h"
#include "StatsCollector.h"
#include "StatsWeightCalculator.h"
#include "Timer.h"
#include "World.h"
#include <mutex>
#include <set>
#include <sstream>
#include <unordered_map>

#if __has_include("AscensionFreePick.h") && __has_include("Area52BuildProtocol.h") && \
    __has_include("AscensionMysticEnchantCatalog.h")
#define COA_LEGENDARY_CLASSES 1
#include "Area52BuildProtocol.h"
#include "AscensionFreePick.h"
#include "AscensionMysticEnchantCatalog.h"
#else
#define COA_LEGENDARY_CLASSES 0
#endif

namespace
{
constexpr uint32 RetryMs = 10 * IN_MILLISECONDS;
constexpr uint32 CollectionRetryMs = 60 * IN_MILLISECONDS;

constexpr uint32 Weapon(uint32 subclass) { return 1u << subclass; }

std::array<CoaLegendaryClass, 3> const LegendaryClasses = {{
    { "mknight", "Mystic Knight", 81116, CoaSpecRole::Tank, "close", 3, "custom::mystic-knight",
      "custom::mystic-knight-nc", 20, CoaLegendaryPicksMknight, &CoaLegendaryEnchantsMknight,
      ITEM_SUBCLASS_ARMOR_PLATE, true, Weapon(ITEM_SUBCLASS_WEAPON_SWORD),
      {{ { STATS_TYPE_STAMINA, 3.0f }, { STATS_TYPE_BLOCK_VALUE, 2.5f }, { STATS_TYPE_INTELLECT, 2.0f },
         { STATS_TYPE_STRENGTH, 1.0f }, { STATS_TYPE_DEFENSE, 1.0f }, { STATS_TYPE_ARMOR, 0.02f } }} },
    { "hoplite", "Hoplite", 978217, CoaSpecRole::Dps, "close", 0, "custom::hoplite", "custom::hoplite-nc", 60,
      CoaLegendaryPicksHoplite, &CoaLegendaryEnchantsHoplite, ITEM_SUBCLASS_ARMOR_MAIL, true,
      Weapon(ITEM_SUBCLASS_WEAPON_POLEARM) | Weapon(ITEM_SUBCLASS_WEAPON_THROWN),
      {{ { STATS_TYPE_STRENGTH, 2.5f }, { STATS_TYPE_ATTACK_POWER, 1.0f }, { STATS_TYPE_HIT, 1.4f },
         { STATS_TYPE_CRIT, 1.2f }, { STATS_TYPE_ARMOR_PENETRATION, 1.0f }, { STATS_TYPE_MELEE_DPS, 5.0f } }} },
    { "naturalist", "Naturalist", 965937, CoaSpecRole::Heal, "ranged", 4, "custom::naturalist",
      "custom::naturalist-nc", 20, CoaLegendaryPicksNaturalist, &CoaLegendaryEnchantsNaturalist,
      ITEM_SUBCLASS_ARMOR_LEATHER, false, Weapon(ITEM_SUBCLASS_WEAPON_STAFF),
      {{ { STATS_TYPE_SPIRIT, 2.5f }, { STATS_TYPE_INTELLECT, 2.0f }, { STATS_TYPE_HEAL_POWER, 1.5f },
         { STATS_TYPE_SPELL_POWER, 1.0f }, { STATS_TYPE_MANA_REGENERATION, 1.5f }, { STATS_TYPE_HASTE, 1.0f } }} },
}};

struct LegendaryState
{
    uint32 appliedLevel = 0;
    uint32 nextTry = 0;
    uint32 nextCollection = 0;
    uint32 nextShield = 0;
};

std::mutex StatesLock;
std::unordered_map<uint64, LegendaryState> States;

bool Due(uint32 now, uint32 when) { return !when || int32(now - when) >= 0; }

std::string SlotSetting(Player const* player)
{
    PlayerSettingVector const* active = player->FindPlayerSettings("core.area52.mystic.active");
    uint32 const preset = active && !active->empty() ? (*active)[0].value : player->GetActiveSpec();
    return "core.freepick.mystic." + std::to_string(preset);
}

CoaLegendaryClass const* RandomClassFor(Player const* bot)
{
    uint32 const roll = (bot->GetGUID().GetCounter() * 2654435761u >> 16) % 100;
    uint32 reached = 0;
    for (CoaLegendaryClass const& legendaryClass : LegendaryClasses)
    {
        reached += legendaryClass.chance;
        if (roll < reached)
            return &legendaryClass;
    }
    return &LegendaryClasses.back();
}

template <typename Hero>
bool HoldsShieldWith(Hero const* bot, ItemTemplate const* main, ItemTemplate const* off)
{
    if constexpr (requires { bot->CanUseTwoHandWithShield(main, off); })
        return bot->CanUseTwoHandWithShield(main, off);
    else
        return false;
}

bool ShieldMissing(Player* bot, CoaLegendaryClass const& legendaryClass)
{
    if (!legendaryClass.shield || !bot->HasAura(legendaryClass.legendary) ||
        bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
        return false;
    Item* main = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    return main && main->GetTemplate()->InventoryType == INVTYPE_2HWEAPON;
}

Item* BestBagShield(Player* bot)
{
    Item* main = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    StatsWeightCalculator calculator(bot);
    Item* best = nullptr;
    float bestScore = -1.0f;
    auto consider = [&](Item* item)
    {
        if (!item || item->GetTemplate()->InventoryType != INVTYPE_SHIELD ||
            !HoldsShieldWith(bot, main->GetTemplate(), item->GetTemplate()))
            return;
        float const score = calculator.CalculateItem(item->GetEntry(), item->GetItemRandomPropertyId());
        if (score > bestScore)
        {
            best = item;
            bestScore = score;
        }
    };
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
        consider(bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot));
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
        if (Bag* container = bot->GetBagByPos(bag))
            for (uint32 slot = 0; slot < container->GetBagSize(); ++slot)
                consider(container->GetItemByPos(slot));
    return best;
}

void EquipLegendaryShield(Player* bot)
{
    if (Item* shield = BestBagShield(bot))
    {
        uint16 dest;
        if (bot->CanEquipItem(EQUIPMENT_SLOT_OFFHAND, dest, shield, false) == EQUIP_ERR_OK)
        {
            bot->RemoveItem(shield->GetBagSlot(), shield->GetSlot(), true);
            bot->EquipItem(dest, shield, true);
            return;
        }
    }
    if (sRandomPlayerbotMgr.IsRandomBot(bot))
        PlayerbotFactory(bot, bot->GetLevel()).InitEquipment(false, false, EQUIPMENT_SLOT_OFFHAND);
}

#if COA_LEGENDARY_CLASSES
std::set<uint32> const& CatalogSpells()
{
    static std::set<uint32> const spells = []
    {
        std::set<uint32> known;
        AscensionFreePick::Catalog catalog;
        if (AscensionFreePick::LoadCatalog(catalog))
            for (auto const& [id, entry] : catalog.Entries)
                known.insert(entry.Spells.begin(), entry.Spells.end());
        return known;
    }();
    return spells;
}

void WriteSlots(Player* bot, std::array<uint32, 17> const& enchants)
{
    std::string const setting = SlotSetting(bot);
    PlayerSettingVector const* stored = bot->FindPlayerSettings(setting);
    for (uint32 slot = 0; slot < enchants.size(); ++slot)
        if (!stored || slot >= stored->size() || (*stored)[slot].value != enchants[slot])
            bot->UpdatePlayerSetting(setting, slot, enchants[slot]);
}

void Quiet(void*, std::string_view) { }
void Finished(void*, bool) { }

// The core grants an account every Mystic Enchant through its console command, then reloads the collection of
// the character and puts the slotted ones on. It refuses while the character's collection is still loading or
// the character fights, so this is asked again until the Legendary is on.
void RequestCollection(Player* bot)
{
    std::string const command = "mystic grant 0 " + bot->GetName();
    sWorld->QueueCliCommand(new CliCommandHolder(nullptr, command.c_str(), &Quiet, &Finished));
}
#endif
}  // namespace

std::span<CoaLegendaryClass const> CoaLegendaryClasses() { return LegendaryClasses; }

// Random bots are created in OnBeforeWorldInitialized, before the core turns Free Pick on in OnStartup, so
// AscensionFreePick::Enabled() is still false then: the bots came out in CoA classes. This reads the settings the
// core decides by (AscensionCompat.cpp, OnStartup), with the core's defaults.
bool CoaHeroBotsWanted()
{
#if COA_LEGENDARY_CLASSES
    return sPlayerbotAIConfig.area52HeroBots && sConfigMgr->GetOption<bool>("CoA.Enable", true) &&
        sConfigMgr->GetOption<std::string>("CoA.ClassModel", "coa") == "hero" &&
        sConfigMgr->GetOption<std::string>("CoA.RealmType", "live") == "live" &&
        !(sConfigMgr->GetOption<uint32>("CoA.GameModeMask", 0) & ~AscensionFreePick::QualityMode);
#else
    return false;
#endif
}

bool CoaHeroHoldsShieldWith(Player const* bot, ItemTemplate const* main, ItemTemplate const* off)
{
    return main && off && HoldsShieldWith(bot, main, off);
}

bool IsCoaHero(Player const* player)
{
#if COA_LEGENDARY_CLASSES
    return player && player->getClass() == CoaHeroClass && AscensionFreePick::Applies(player);
#else
    (void)player;
    return false;
#endif
}

CoaLegendaryClass const* GetCoaLegendaryClass(Player const* player)
{
    if (!IsCoaHero(player))
        return nullptr;

    PlayerSettingVector const* slots = player->FindPlayerSettings(SlotSetting(player));
    if (!slots)
        return nullptr;
    for (auto const& slot : *slots)
        for (CoaLegendaryClass const& legendaryClass : LegendaryClasses)
            if (slot.value == legendaryClass.legendary)
                return &legendaryClass;
    return nullptr;
}

CoaLegendaryClass const* FindCoaLegendaryClass(std::string const& spec)
{
    for (CoaLegendaryClass const& legendaryClass : LegendaryClasses)
        if (spec == legendaryClass.spec)
            return &legendaryClass;
    return nullptr;
}

bool ApplyCoaLegendaryClass(Player* bot, CoaLegendaryClass const& legendaryClass, std::string& error)
{
#if COA_LEGENDARY_CLASSES
    if (!IsCoaHero(bot))
    {
        error = "I am not a Freepick Hero.";
        return false;
    }

    Area52Build::Entry plan;
    plan.PrimaryStat = legendaryClass.primaryStat;
    for (CoaLegendaryPick const& pick : legendaryClass.picks)
        if (CatalogSpells().contains(pick.spell))
        {
            Area52Build::Spell spell;
            spell.Id = pick.spell;
            spell.Level = pick.level;
            plan.Spells.push_back(spell);
        }

    std::vector<AscensionFreePick::KnownEntry> wanted;
    AscensionFreePick::Result const planned = AscensionFreePick::PlanSelection(bot, plan, wanted);
    if (!planned)
    {
        error = std::string(planned.Status) + " " + planned.Learn;
        return false;
    }
    AscensionFreePick::Result const applied = AscensionFreePick::Apply(bot, wanted, false);
    if (!applied)
    {
        error = std::string(applied.Status) + " " + applied.Learn;
        return false;
    }

    WriteSlots(bot, *legendaryClass.enchants);
    AscensionMysticEnchant::RefreshEquipped(bot);

    uint32 const now = getMSTime();
    bool requestCollection = false;
    {
        std::lock_guard<std::mutex> guard(StatesLock);
        LegendaryState& state = States[bot->GetGUID().GetRawValue()];
        state.appliedLevel = bot->GetLevel();
        if (!bot->HasAura(legendaryClass.legendary) && Due(now, state.nextCollection))
        {
            state.nextCollection = now + CollectionRetryMs;
            requestCollection = true;
        }
    }
    if (requestCollection)
        RequestCollection(bot);
    return true;
#else
    (void)bot;
    (void)legendaryClass;
    error = "This server has no Free Pick.";
    return false;
#endif
}

void EnsureCoaLegendaryClass(Player* bot)
{
    if (!IsCoaHero(bot) || !bot->IsInWorld())
        return;

    CoaLegendaryClass const* legendaryClass = GetCoaLegendaryClass(bot);
    if (!legendaryClass)
    {
        if (!sRandomPlayerbotMgr.IsRandomBot(bot))
            return;
        legendaryClass = RandomClassFor(bot);
    }

    uint32 const now = getMSTime();
    bool equipShield = false;
    {
        std::lock_guard<std::mutex> guard(StatesLock);
        LegendaryState& state = States[bot->GetGUID().GetRawValue()];
        if (state.appliedLevel == bot->GetLevel() && bot->HasAura(legendaryClass->legendary))
        {
            if (!ShieldMissing(bot, *legendaryClass) || !Due(now, state.nextShield))
                return;
            state.nextShield = now + CollectionRetryMs;
            equipShield = true;
        }
        else if (!Due(now, state.nextTry))
            return;
        else
            state.nextTry = now + RetryMs;
    }
    if (equipShield)
    {
        EquipLegendaryShield(bot);
        return;
    }

    std::string error;
    if (!ApplyCoaLegendaryClass(bot, *legendaryClass, error))
        LOG_DEBUG("playerbots", "{} could not take {}: {}", bot->GetName(), legendaryClass->name, error);
}

bool CoaLegendaryClassUnready(Player* bot)
{
    if (!IsCoaHero(bot))
        return false;

    CoaLegendaryClass const* legendaryClass = GetCoaLegendaryClass(bot);
    if (!legendaryClass)
        return sRandomPlayerbotMgr.IsRandomBot(bot);

    std::lock_guard<std::mutex> guard(StatesLock);
    auto const found = States.find(bot->GetGUID().GetRawValue());
    if (found == States.end())
        return true;
    uint32 const now = getMSTime();
    if (found->second.appliedLevel == bot->GetLevel() && bot->HasAura(legendaryClass->legendary))
        return ShieldMissing(bot, *legendaryClass) && Due(now, found->second.nextShield);
    return Due(now, found->second.nextTry);
}

std::string CoaLegendaryClassList(Player const* bot)
{
    CoaLegendaryClass const* current = GetCoaLegendaryClass(bot);
    std::ostringstream out;
    out << "Legendary classes (talents spec <name>):";
    for (CoaLegendaryClass const& legendaryClass : LegendaryClasses)
    {
        char const* role = legendaryClass.role == CoaSpecRole::Tank ? "tank" :
            legendaryClass.role == CoaSpecRole::Heal ? "healer" : "damage";
        out << (&legendaryClass == current ? "\n> " : "\n  ") << legendaryClass.spec << " - "
            << legendaryClass.name << ", " << role << ", " << legendaryClass.position;
    }
    return out.str();
}
