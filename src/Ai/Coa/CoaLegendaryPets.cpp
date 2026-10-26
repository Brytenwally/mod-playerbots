#include "CoaLegendaryPets.h"

#include "DBCStores.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Pet.h"
#include "Player.h"
#include "SpellMgr.h"
#include <array>
#include <set>
#if __has_include("AscensionFreePick.h")
#include "AscensionFreePick.h"
#endif

namespace
{
constexpr uint8 HeroClass = 10;
constexpr int32 FerocityTalents = 0;

struct LegendaryPet
{
    uint32 legendary;  // the Legendary Mystic Enchant, also its aura
    uint32 creature;
};

// Hoplite: Winter Wolf, a wolf (Ferocity, Furious Howl) that any Hero may have.
constexpr std::array<LegendaryPet, 1> LegendaryPets = {{
    { 978217, 1131 },
}};

// Call Pet, Dismiss Pet, Revive Pet.
std::set<uint32> const PetCommands = { 883, 2641, 982 };

bool FerocityPet(CreatureTemplate const* creature)
{
    if (!creature || !creature->IsTameable(false))
        return false;
    CreatureFamilyEntry const* family = sCreatureFamilyStore.LookupEntry(creature->family);
    return family && family->petTalentType == FerocityTalents;
}

// Free Pick refuses spells that no pick grants; the scope lets these through, as the core does for its own grants.
void TeachPetCommands(Player* bot)
{
#if __has_include("AscensionFreePick.h")
    AscensionFreePick::SpellLearningScope learning(bot, PetCommands);
#endif
    for (uint32 spell : PetCommands)
        if (!bot->HasSpell(spell) && sSpellMgr->GetSpellInfo(spell))
            bot->learnSpell(spell);
}
}  // namespace

uint32 CoaLegendaryPetEntry(Player* bot)
{
    if (!bot || bot->getClass() != HeroClass)
        return 0;
    for (LegendaryPet const& pet : LegendaryPets)
        if (bot->HasAura(pet.legendary))
            return pet.creature;
    return 0;
}

Pet* CoaCreateLegendaryPet(Player* bot)
{
    uint32 const entry = CoaLegendaryPetEntry(bot);
    if (!entry)
        return nullptr;

    TeachPetCommands(bot);
    CreatureTemplate const* creature = sObjectMgr->GetCreatureTemplate(entry);
    if (!FerocityPet(creature))
    {
        LOG_ERROR("playerbots", "Legendary pet {} of bot {} is not a tameable Ferocity creature", entry,
                  bot->GetName());
        return nullptr;
    }

    if (bot->GetPetStable() && bot->GetPetStable()->CurrentPet)
        return nullptr;

    Pet* pet = bot->CreateTamedPetFrom(entry, 0);
    if (!pet)
        return nullptr;

    pet->SetUInt32Value(UNIT_FIELD_LEVEL, bot->GetLevel());
    pet->GetMap()->AddToMap(pet->ToCreature());
    bot->SetMinion(pet, true);
    pet->InitTalentForLevel();
    pet->InitStatsForLevel(bot->GetLevel());
    pet->SetPower(POWER_HAPPINESS, pet->GetMaxPower(POWER_HAPPINESS));
    pet->SetHealth(pet->GetMaxHealth());
    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    bot->PetSpellInitialize();

    for (auto const& [spellId, spell] : pet->m_spells)
        if (spell.state != PETSPELL_REMOVED)
            if (SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId); info && !info->IsPassive())
                pet->ToggleAutocast(info, true);
    return pet;
}
