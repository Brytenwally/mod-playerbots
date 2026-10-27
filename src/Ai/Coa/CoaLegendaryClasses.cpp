#include "CoaLegendaryClasses.h"

#include "CoaLegendaryPets.h"
#include "Config.h"
#include "DBCStores.h"
#include "DatabaseEnv.h"
#include "IWorld.h"
#include "Item.h"
#include "Pet.h"
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
#include <algorithm>
#include <cctype>
#include <map>
#include <mutex>
#include <set>
#include <sstream>
#include <string_view>
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
constexpr uint32 SlotRetryMs = 60 * IN_MILLISECONDS;
constexpr uint32 SlotTries = 5;

std::map<std::string_view, uint32> const WeaponSubclasses = {
    { "axe", ITEM_SUBCLASS_WEAPON_AXE }, { "axe2", ITEM_SUBCLASS_WEAPON_AXE2 }, { "bow", ITEM_SUBCLASS_WEAPON_BOW },
    { "gun", ITEM_SUBCLASS_WEAPON_GUN }, { "mace", ITEM_SUBCLASS_WEAPON_MACE }, { "mace2", ITEM_SUBCLASS_WEAPON_MACE2 },
    { "polearm", ITEM_SUBCLASS_WEAPON_POLEARM }, { "sword", ITEM_SUBCLASS_WEAPON_SWORD },
    { "sword2", ITEM_SUBCLASS_WEAPON_SWORD2 }, { "staff", ITEM_SUBCLASS_WEAPON_STAFF },
    { "fist", ITEM_SUBCLASS_WEAPON_FIST }, { "dagger", ITEM_SUBCLASS_WEAPON_DAGGER },
    { "thrown", ITEM_SUBCLASS_WEAPON_THROWN }, { "crossbow", ITEM_SUBCLASS_WEAPON_CROSSBOW },
    { "wand", ITEM_SUBCLASS_WEAPON_WAND },
};

std::map<std::string_view, uint32> const ArmorSubclasses = {
    { "cloth", ITEM_SUBCLASS_ARMOR_CLOTH }, { "leather", ITEM_SUBCLASS_ARMOR_LEATHER },
    { "mail", ITEM_SUBCLASS_ARMOR_MAIL }, { "plate", ITEM_SUBCLASS_ARMOR_PLATE },
};

std::map<std::string_view, CoaSpecRole> const Roles = {
    { "tank", CoaSpecRole::Tank }, { "heal", CoaSpecRole::Heal }, { "dps", CoaSpecRole::Dps },
};

std::map<std::string_view, uint8> const Stats = {
    { "agility", STATS_TYPE_AGILITY }, { "strength", STATS_TYPE_STRENGTH }, { "intellect", STATS_TYPE_INTELLECT },
    { "spirit", STATS_TYPE_SPIRIT }, { "stamina", STATS_TYPE_STAMINA }, { "hit", STATS_TYPE_HIT },
    { "crit", STATS_TYPE_CRIT }, { "haste", STATS_TYPE_HASTE }, { "armor", STATS_TYPE_ARMOR },
    { "defense", STATS_TYPE_DEFENSE }, { "dodge", STATS_TYPE_DODGE }, { "parry", STATS_TYPE_PARRY },
    { "block_value", STATS_TYPE_BLOCK_VALUE }, { "block_rating", STATS_TYPE_BLOCK_RATING },
    { "resilience", STATS_TYPE_RESILIENCE }, { "health_regeneration", STATS_TYPE_HEALTH_REGENERATION },
    { "spell_power", STATS_TYPE_SPELL_POWER }, { "spell_penetration", STATS_TYPE_SPELL_PENETRATION },
    { "heal_power", STATS_TYPE_HEAL_POWER }, { "mana_regeneration", STATS_TYPE_MANA_REGENERATION },
    { "attack_power", STATS_TYPE_ATTACK_POWER }, { "armor_penetration", STATS_TYPE_ARMOR_PENETRATION },
    { "expertise", STATS_TYPE_EXPERTISE }, { "melee_dps", STATS_TYPE_MELEE_DPS },
    { "ranged_dps", STATS_TYPE_RANGED_DPS },
};

template <typename Value>
bool Lookup(std::map<std::string_view, Value> const& names, std::string const& name, Value& value,
    std::string const& spec, char const* column)
{
    auto const found = names.find(name);
    if (found == names.end())
    {
        LOG_ERROR("playerbots", "Legendary class '{}': {} '{}' is unknown", spec, column, name);
        return false;
    }
    value = found->second;
    return true;
}

CoaLegendaryClass* FindLoaded(std::vector<CoaLegendaryClass>& classes, std::string const& spec)
{
    auto const found = std::find_if(classes.begin(), classes.end(),
        [&spec](CoaLegendaryClass const& legendaryClass) { return legendaryClass.spec == spec; });
    if (found == classes.end())
    {
        LOG_ERROR("playerbots", "Legendary class '{}' has rows but no playerbots_legendary_class row", spec);
        return nullptr;
    }
    return &*found;
}

std::vector<CoaLegendaryClass> LoadClasses()
{
    std::vector<CoaLegendaryClass> classes;
    QueryResult rows = PlayerbotsDatabase.Query("SELECT `spec`, `name`, `legendary`, `role`, `position`, "
        "`primary_stat`, `combat`, `noncombat`, `chance`, `armor`, `shield`, `weapons`, `pet` "
        "FROM `playerbots_legendary_class` ORDER BY `spec`");
    if (!rows)
    {
        LOG_ERROR("playerbots", "No Area 52 Legendary classes in playerbots_legendary_class: Hero bots get no class");
        return classes;
    }
    do
    {
        Field* field = rows->Fetch();
        CoaLegendaryClass legendaryClass;
        legendaryClass.spec = field[0].Get<std::string>();
        legendaryClass.name = field[1].Get<std::string>();
        legendaryClass.legendary = field[2].Get<uint32>();
        legendaryClass.position = field[4].Get<std::string>();
        legendaryClass.primaryStat = field[5].Get<uint8>();
        legendaryClass.combat = field[6].Get<std::string>();
        legendaryClass.nonCombat = field[7].Get<std::string>();
        legendaryClass.chance = field[8].Get<uint8>();
        legendaryClass.shield = field[10].Get<uint8>() != 0;
        legendaryClass.pet = field[12].Get<uint32>();
        std::string const& spec = legendaryClass.spec;
        bool valid = Lookup(Roles, field[3].Get<std::string>(), legendaryClass.role, spec, "role") &&
            Lookup(ArmorSubclasses, field[9].Get<std::string>(), legendaryClass.heaviestArmor, spec, "armor");
        std::stringstream weapons(field[11].Get<std::string>());
        for (std::string weapon; valid && std::getline(weapons, weapon, ',');)
        {
            uint32 subclass = 0;
            valid = Lookup(WeaponSubclasses, weapon, subclass, spec, "weapon");
            legendaryClass.weapons |= 1u << subclass;
        }
        if (legendaryClass.position != "close" && legendaryClass.position != "ranged")
        {
            LOG_ERROR("playerbots", "Legendary class '{}': position '{}' is neither close nor ranged", spec,
                legendaryClass.position);
            valid = false;
        }
        if (valid)
            classes.push_back(std::move(legendaryClass));
    } while (rows->NextRow());

    if (QueryResult picks = PlayerbotsDatabase.Query(
            "SELECT `spec`, `spell`, `level` FROM `playerbots_legendary_pick` ORDER BY `spec`, `level`, `spell`"))
        do
        {
            Field* field = picks->Fetch();
            if (CoaLegendaryClass* legendaryClass = FindLoaded(classes, field[0].Get<std::string>()))
                legendaryClass->picks.push_back({ field[1].Get<uint32>(), field[2].Get<uint8>() });
        } while (picks->NextRow());

    if (QueryResult enchants = PlayerbotsDatabase.Query(
            "SELECT `spec`, `slot`, `spell` FROM `playerbots_legendary_enchant`"))
        do
        {
            Field* field = enchants->Fetch();
            uint8 const slot = field[1].Get<uint8>();
            if (slot < 1 || slot > 17)
                LOG_ERROR("playerbots", "Legendary class '{}': enchant slot {} is not 1 to 17",
                    field[0].Get<std::string>(), slot);
            else if (CoaLegendaryClass* legendaryClass = FindLoaded(classes, field[0].Get<std::string>()))
                legendaryClass->enchants[slot - 1] = field[2].Get<uint32>();
        } while (enchants->NextRow());

    if (QueryResult weights = PlayerbotsDatabase.Query(
            "SELECT `spec`, `stat`, `weight` FROM `playerbots_legendary_weight`"))
        do
        {
            Field* field = weights->Fetch();
            std::string const spec = field[0].Get<std::string>();
            uint8 stat = 0;
            if (CoaLegendaryClass* legendaryClass = FindLoaded(classes, spec))
                if (Lookup(Stats, field[1].Get<std::string>(), stat, spec, "stat"))
                    legendaryClass->weights.emplace_back(stat, field[2].Get<float>());
        } while (weights->NextRow());

    for (CoaLegendaryClass const& legendaryClass : classes)
        if (std::find(legendaryClass.enchants.begin(), legendaryClass.enchants.end(), legendaryClass.legendary) ==
            legendaryClass.enchants.end())
            LOG_ERROR("playerbots", "Legendary class '{}': its Legendary {} is in none of its enchant slots",
                legendaryClass.spec, legendaryClass.legendary);
    LOG_INFO("server.loading", ">> Loaded {} Area 52 Legendary classes for the bots", classes.size());
    return classes;
}

std::vector<CoaLegendaryClass> const& LoadedClasses()
{
    static std::vector<CoaLegendaryClass> const classes = LoadClasses();
    return classes;
}

struct LegendaryState
{
    uint32 appliedLevel = 0;
    uint32 nextTry = 0;
    uint32 nextSlots = 0;
    uint32 slotTries = 0;
    uint32 nextShield = 0;
    std::string failure;
};

std::mutex StatesLock;
std::unordered_map<uint64, LegendaryState> States;
std::unordered_map<uint64, std::string> RequestedSpecs;

CoaLegendaryClass const* RequestedClass(Player const* bot)
{
    std::string spec;
    {
        std::lock_guard<std::mutex> guard(StatesLock);
        auto const found = RequestedSpecs.find(bot->GetGUID().GetRawValue());
        if (found == RequestedSpecs.end())
            return nullptr;
        spec = found->second;
    }
    return FindCoaLegendaryClass(spec);
}

void ForgetRequestedClass(Player const* bot)
{
    std::lock_guard<std::mutex> guard(StatesLock);
    RequestedSpecs.erase(bot->GetGUID().GetRawValue());
}

std::string ClassKey(std::string_view text)
{
    std::string key;
    for (char c : text)
        if (std::isalnum(static_cast<unsigned char>(c)))
            key += char(std::tolower(static_cast<unsigned char>(c)));
    return key;
}

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
    for (CoaLegendaryClass const& legendaryClass : LoadedClasses())
    {
        reached += legendaryClass.chance;
        if (roll < reached)
            return &legendaryClass;
    }
    return LoadedClasses().empty() ? nullptr : &LoadedClasses().back();
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
    return legendaryClass.shield && bot->HasAura(legendaryClass.legendary) &&
        !bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
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
            (main && main->GetTemplate()->InventoryType == INVTYPE_2HWEAPON &&
             !HoldsShieldWith(bot, main->GetTemplate(), item->GetTemplate())))
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

bool SlotsMatch(Player const* bot, CoaLegendaryClass const& legendaryClass)
{
    PlayerSettingVector const* stored = bot->FindPlayerSettings(SlotSetting(bot));
    for (uint32 slot = 0; slot < legendaryClass.enchants.size(); ++slot)
        if ((stored && slot < stored->size() ? (*stored)[slot].value : 0) != legendaryClass.enchants[slot])
            return false;
    return true;
}

#if COA_LEGENDARY_CLASSES
// The Free Pick entries each catalog spell belongs to, any rank.
std::unordered_map<uint32, std::vector<uint32>> const& CatalogEntries()
{
    static std::unordered_map<uint32, std::vector<uint32>> const entries = []
    {
        std::unordered_map<uint32, std::vector<uint32>> bySpell;
        AscensionFreePick::Catalog catalog;
        if (AscensionFreePick::LoadCatalog(catalog))
            for (auto const& [id, entry] : catalog.Entries)
                for (uint32 spell : entry.Spells)
                    bySpell[spell].push_back(id);
        return bySpell;
    }();
    return entries;
}

bool GrantedByEnchant(Player const* bot, uint32 spell)
{
    PlayerSettingVector const* owners = bot->FindPlayerSettings("core.freepick.mystic.grant." + std::to_string(spell));
    return owners && owners->size() > 1 &&
        std::any_of(owners->begin() + 1, owners->end(), [](PlayerSetting const& owner) { return owner.value != 0; });
}

// Free Pick spells of entries the bot has not picked: those a trainer or a quest handed it, and those of a class it
// played before. They stayed when it changed class, beside the new picks.
void RemoveUnpickedSpells(Player* bot, CoaLegendaryClass const& legendaryClass)
{
    std::set<uint32> picked;
    for (AscensionFreePick::KnownEntry const& known : AscensionFreePick::KnownEntries(bot))
        picked.insert(known.EntryId);
    std::vector<uint32> unpicked;
    for (auto const& [spell, playerSpell] : bot->GetSpellMap())
    {
        auto const entries = CatalogEntries().find(spell);
        if (playerSpell->State == PLAYERSPELL_REMOVED || entries == CatalogEntries().end() ||
            std::any_of(entries->second.begin(), entries->second.end(),
                [&picked](uint32 entry) { return picked.contains(entry); }) ||
            (legendaryClass.pet && CoaLegendaryPetCommand(spell)) || GrantedByEnchant(bot, spell))
            continue;
        unpicked.push_back(spell);
    }
    for (uint32 spell : unpicked)
        bot->removeSpell(spell, SPEC_MASK_ALL, false);
}

// A bot changing class leaves the pet of the old one, and a random bot takes the gear of the new one.
void LeaveClass(Player* bot, CoaLegendaryClass const& previous, CoaLegendaryClass const& next)
{
    if (Pet* pet = bot->GetPet(); pet && previous.pet && pet->GetEntry() == previous.pet && next.pet != previous.pet)
        bot->RemovePet(pet, PET_SAVE_AS_DELETED);
    if (sRandomPlayerbotMgr.IsRandomBot(bot))
        PlayerbotFactory(bot, bot->GetLevel()).InitEquipment(false);
}

void Quiet(void*, std::string_view) { }
void Finished(void*, bool) { }

void QueueCommand(std::string const& command)
{
    sWorld->QueueCliCommand(new CliCommandHolder(nullptr, command.c_str(), &Quiet, &Finished));
}

// The core puts on no enchant at all while one slotted enchant is invalid (one this server does not connect, or
// more stacks than it allows), and the Legendary with them. The Legendary alone is valid, so it is written
// straight to its slot, which also keeps the class, and every other slot emptied. The rest go through the core's
// "mystic equip" command, which checks each slot as the Mystic Enchant window does: an invalid enchant stays out
// alone.
void EquipSlots(Player* bot, CoaLegendaryClass const& legendaryClass)
{
    std::array<uint32, 17> const& enchants = legendaryClass.enchants;
    std::string const setting = SlotSetting(bot);
    PlayerSettingVector const* stored = bot->FindPlayerSettings(setting);
    auto storedAt = [stored](uint32 slot) { return stored && slot < stored->size() ? (*stored)[slot].value : 0; };

    if (!bot->HasAura(legendaryClass.legendary))
    {
        for (uint32 slot = 0; slot < enchants.size(); ++slot)
        {
            uint32 const wanted = enchants[slot] == legendaryClass.legendary ? enchants[slot] : 0;
            if (storedAt(slot) != wanted || !stored || slot >= stored->size())
                bot->UpdatePlayerSetting(setting, slot, wanted);
        }
        AscensionMysticEnchant::RefreshEquipped(bot);
        stored = bot->FindPlayerSettings(setting);
    }

    std::string const name = bot->GetName();
    for (uint32 slot = 0; slot < enchants.size(); ++slot)
        if (storedAt(slot) != enchants[slot])
            QueueCommand("mystic equip " + std::to_string(slot + 1) + " " + std::to_string(enchants[slot]) + " " +
                name);
}
#endif
}  // namespace

std::span<CoaLegendaryClass const> CoaLegendaryClasses() { return LoadedClasses(); }

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

void CoaFillMysticCollection(uint32 accountId)
{
#if COA_LEGENDARY_CLASSES
    static std::mutex lock;
    static std::set<uint32> filled;
    static std::vector<uint32> const enchants = []
    {
        std::vector<uint32> spells;
        AscensionMysticEnchant::Catalog catalog;
        if (AscensionMysticEnchant::LoadCatalog(catalog, GetClientDBCPath("MysticEnchant.dbc")))
            for (auto const& [spell, entry] : catalog.BySpell)
                spells.push_back(spell);
        return spells;
    }();
    if (enchants.empty() || !AscensionFreePick::Enabled())
        return;
    {
        std::lock_guard<std::mutex> guard(lock);
        if (!filled.insert(accountId).second)
            return;
    }
    if (QueryResult owned = CharacterDatabase.Query(
            "SELECT COUNT(*) FROM `account_freepick_mystic_collection` WHERE `account` = {}", accountId))
        if (owned->Fetch()[0].Get<uint64>() >= enchants.size())
            return;
    std::ostringstream insert;
    insert << "INSERT IGNORE INTO `account_freepick_mystic_collection` (`account`, `spell`) VALUES ";
    for (std::size_t i = 0; i < enchants.size(); ++i)
        insert << (i ? "," : "") << '(' << accountId << ',' << enchants[i] << ')';
    CharacterDatabase.DirectExecute(insert.str());
#else
    (void)accountId;
#endif
}

bool CoaShieldWaitsForLegendary(Player* bot, ItemTemplate const* proto)
{
    CoaLegendaryClass const* legendaryClass = GetCoaLegendaryClass(bot);
    if (!legendaryClass || !legendaryClass->shield || !proto || proto->InventoryType != INVTYPE_SHIELD ||
        bot->CanUseItem(proto) != EQUIP_ERR_OK)
        return false;
    Item* main = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    return main && main->GetTemplate()->InventoryType == INVTYPE_2HWEAPON &&
        !HoldsShieldWith(bot, main->GetTemplate(), proto);
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
        for (CoaLegendaryClass const& legendaryClass : LoadedClasses())
            if (slot.value == legendaryClass.legendary)
                return &legendaryClass;
    return nullptr;
}

CoaLegendaryClass const* FindCoaLegendaryClass(std::string const& spec)
{
    std::string const key = ClassKey(spec);
    if (key.empty())
        return nullptr;
    for (CoaLegendaryClass const& legendaryClass : LoadedClasses())
        if (key == ClassKey(legendaryClass.spec) || key == ClassKey(legendaryClass.name))
            return &legendaryClass;
    return nullptr;
}

void CoaRequestLegendaryClass(ObjectGuid guid, CoaLegendaryClass const& legendaryClass)
{
    std::lock_guard<std::mutex> guard(StatesLock);
    RequestedSpecs[guid.GetRawValue()] = legendaryClass.spec;
}

bool ApplyCoaLegendaryClass(Player* bot, CoaLegendaryClass const& legendaryClass, std::string& error)
{
#if COA_LEGENDARY_CLASSES
    if (!IsCoaHero(bot))
    {
        error = "I am not a Freepick Hero.";
        return false;
    }

    CoaLegendaryClass const* previous = GetCoaLegendaryClass(bot);
    Area52Build::Entry plan;
    plan.PrimaryStat = legendaryClass.primaryStat;
    for (CoaLegendaryPick const& pick : legendaryClass.picks)
        if (CatalogEntries().contains(pick.spell))
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

    EquipSlots(bot, legendaryClass);
    RemoveUnpickedSpells(bot, legendaryClass);
    if (previous && previous != &legendaryClass)
        LeaveClass(bot, *previous, legendaryClass);

    {
        std::lock_guard<std::mutex> guard(StatesLock);
        LegendaryState& state = States[bot->GetGUID().GetRawValue()];
        state.appliedLevel = bot->GetLevel();
        state.nextSlots = getMSTime() + SlotRetryMs;
        state.slotTries = 0;
    }
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
    CoaLegendaryClass const* const requested = RequestedClass(bot);
    if (requested && requested == legendaryClass)
        ForgetRequestedClass(bot);
    else if (requested)
        legendaryClass = requested;
    if (!legendaryClass)
    {
        if (!sRandomPlayerbotMgr.IsRandomBot(bot))
            return;
        legendaryClass = RandomClassFor(bot);
        if (!legendaryClass)
            return;
    }

    uint32 const now = getMSTime();
    bool equipShield = false;
    bool equipSlots = false;
    {
        std::lock_guard<std::mutex> guard(StatesLock);
        LegendaryState& state = States[bot->GetGUID().GetRawValue()];
        if (state.appliedLevel == bot->GetLevel() && bot->HasAura(legendaryClass->legendary))
        {
            // The "mystic equip" commands of an apply made before the core had loaded the collection were refused:
            // the slots are asked for again a few times.
            if (!SlotsMatch(bot, *legendaryClass) && state.slotTries < SlotTries && Due(now, state.nextSlots))
            {
                state.nextSlots = now + SlotRetryMs;
                ++state.slotTries;
                equipSlots = true;
            }
            if (ShieldMissing(bot, *legendaryClass) && Due(now, state.nextShield))
            {
                state.nextShield = now + SlotRetryMs;
                equipShield = true;
            }
            if (!equipSlots && !equipShield)
                return;
        }
        else if (!Due(now, state.nextTry))
            return;
        else
            state.nextTry = now + RetryMs;
    }
    if (equipSlots || equipShield)
    {
#if COA_LEGENDARY_CLASSES
        if (equipSlots)
            EquipSlots(bot, *legendaryClass);
#endif
        if (equipShield)
            EquipLegendaryShield(bot);
        return;
    }

    std::string error;
    bool const applied = ApplyCoaLegendaryClass(bot, *legendaryClass, error);
    if (applied && legendaryClass == requested)
        ForgetRequestedClass(bot);
    bool report = false;
    {
        std::lock_guard<std::mutex> guard(StatesLock);
        std::string& failure = States[bot->GetGUID().GetRawValue()].failure;
        report = !applied && failure != error;
        failure = applied ? std::string() : error;
    }
    if (report)
        LOG_INFO("playerbots", "{} (level {}) could not take its {} picks: {}", bot->GetName(), bot->GetLevel(),
            legendaryClass->name, error);
}

bool CoaLegendaryClassUnready(Player* bot)
{
    if (!IsCoaHero(bot))
        return false;

    CoaLegendaryClass const* legendaryClass = GetCoaLegendaryClass(bot);
    CoaLegendaryClass const* const requested = RequestedClass(bot);
    if (!legendaryClass && !requested)
        return sRandomPlayerbotMgr.IsRandomBot(bot);

    std::lock_guard<std::mutex> guard(StatesLock);
    auto const found = States.find(bot->GetGUID().GetRawValue());
    if (found == States.end())
        return true;
    uint32 const now = getMSTime();
    LegendaryState const& state = found->second;
    if (requested && requested != legendaryClass)
        return Due(now, state.nextTry);
    if (state.appliedLevel == bot->GetLevel() && bot->HasAura(legendaryClass->legendary))
        return (ShieldMissing(bot, *legendaryClass) && Due(now, state.nextShield)) ||
            (state.slotTries < SlotTries && Due(now, state.nextSlots) && !SlotsMatch(bot, *legendaryClass));
    return Due(now, state.nextTry);
}

std::string CoaLegendaryClassCategory(CoaLegendaryClass const& legendaryClass)
{
    if (legendaryClass.role == CoaSpecRole::Tank)
        return "tank";
    if (legendaryClass.role == CoaSpecRole::Heal)
        return "healer";
    return legendaryClass.position == "ranged" ? "rdps" : "mdps";
}

std::string CoaLegendaryClassList(Player const* bot, std::string const& category)
{
    static char const* const Categories[] = {"tank", "healer", "mdps", "rdps"};
    std::string wanted = ClassKey(category);
    if (wanted == "heal")
        wanted = "healer";
    if (std::none_of(std::begin(Categories), std::end(Categories), [&](char const* known) { return wanted == known; }))
        return "Type 'talents spec list tank', 'talents spec list healer', 'talents spec list mdps' (melee damage) or "
               "'talents spec list rdps' (ranged damage), then 'talents spec <name>'.";

    CoaLegendaryClass const* current = GetCoaLegendaryClass(bot);
    std::ostringstream out;
    out << "Legendary classes, " << wanted << " (talents spec <name>):";
    bool any = false;
    for (CoaLegendaryClass const& legendaryClass : LoadedClasses())
    {
        if (CoaLegendaryClassCategory(legendaryClass) != wanted)
            continue;
        any = true;
        out << (&legendaryClass == current ? "\n> " : "\n  ") << legendaryClass.spec << " - " << legendaryClass.name;
    }
    if (!any)
        out << "\n  none yet";
    return out.str();
}
