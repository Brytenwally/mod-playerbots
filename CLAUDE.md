# CLAUDE.md: mod-playerbots, Area 52 Freepick (`Brytenwally/mod-playerbots`, branch `area52-freepick`)

This branch adds bot support for **Freepick Heroes** on the Area 52 server. It started from `coa-bot-strategies`
(1c08cc1). The core is `Brytenwally/azerothcore-wotlk-coa` `main`: the Area 52 server source with upstream CoA
updates applied. The Windows build is `Brytenwally/Freepick-build`.

## Context from the owner

- On Freepick there are no classes, only the **Hero** (class 10). Every level a Hero gains ability points (AE) and
  talent points (TE) and spends them freely on spells and talents from every source tree.
- A Hero's role and rotation come from its picks together with its **Mystic Enchants**, above all its
  **Legendary Mystic Enchant**.
- **Each Legendary Mystic Enchant is treated as a class.** The owner will provide, per Legendary, a build and a basic
  rotation, together with the Freepick DBCs (`MysticEnchant.dbc`, `CharacterAdvancementClassTypes.dbc`). Until then,
  do not guess builds or rotations.

## Working rules

**Pushing and pull requests**
- Push only to `Brytenwally/*` forks. Push this work to `area52-freepick`.
- Never push to `coa` (head of an upstream PR). `coa-bot-strategies` feeds the owner's builds; bring work over from
  this branch only when the owner asks.
- Open a pull request against `Zyth45/mod-playerbots` only when the owner asks. Keep this CLAUDE.md out of any
  upstream PR.

**Never modify the core.** Playerbots adapts to the core as it is: no core patches, no new core exports, no core
config changes. Use what the core already exposes (public functions, its database tables, player settings, the
client DBCs it reads). If something has no public way in, say so and find a playerbots-side way.

**Both cores must keep building.** This module also builds against upstream CoA. Where the cores differ, use
compile-time detection like `src/Ai/Class/Coa/CoaTalentCompat.h` (`if constexpr (requires { ... })`).

**Building**
- Compile-check inside a core checkout with this repo at `modules/mod-playerbots`; disable modules that are not part
  of the repo checkout (for example `-DMODULE_MOD-10-MAN-WORLD=disabled`).
- Run the core's `apps/codestyle/codestyle-cpp.py --files` on changed sources.
- Do not run full builds in the cloud unless asked.

**Testing:** Claude does not test in game. Say so when reporting.

**Naming:** keep upstream naming to avoid merge conflicts.

## Map

Taken against core `037025155` (azerothcore-wotlk-coa `main`, 2026-10-24) and this branch at 1c08cc1.

**Do not re-map from scratch.** Check whether it is still current:
1. `git diff --stat 037025155 <new> -- src/server/coa/AscensionFreePick* src/server/coa/Area52Build*
   src/server/coa/Area52ActiveBuild* src/server/coa/AscensionMysticEnchant* src/server/coa/AscensionMysticSpellGrants.h
   src/server/coa/Area52LegendaryRules.h src/server/coa/AscensionSpecialization.h`
2. Re-read only the files that changed, and update the matching section below.

### 1. The Hero in the core

- `CLASS_HERO` (10). Free Pick applies when the realm runs it and the Hero's game mode mask has no bit other than
  `0x400` (Quality mode): `AscensionFreePick::Applies(player)` (`src/server/coa/AscensionFreePick.cpp`).
- **Catalog**: client `CharacterAdvancement.dbc` (+ `CharacterAdvancementClassTypes.dbc`,
  `CharacterAdvancementEssence.dbc`), loaded by the public `AscensionFreePick::LoadCatalog(Catalog&)`
  (`AscensionFreePickRules.cpp`). Rows kept: types `Ability`, `Talent`, `TalentAbility`, plus the primary stat paths.
  Freepick's file: 7,820 rows (2,991 Ability, 4,384 Talent, 438 TalentAbility) from 43 source class types; qualities
  104 Legendary, 407 Epic, 308 Rare, 113 Uncommon.
- `AscensionFreePick::Entry`: `Id`, `Spells` (one per rank, up to 9), `Required[3]` (prerequisite entries),
  `Masteries[3]`, `Connected`, `Investment[6]`, `Level`, `AE`, `TE`, `Quality` (1 Uncommon to 4 Legendary),
  `QualityCost`, `Class`/`Tab` (source tree), `Group`, `Flags` (`0x1000` mastery), `Points`, `RequiredPoints`.
- **Budgets**: AE and TE per level from `CharacterAdvancementEssence.dbc` (class 10 rows). Quality limits per
  selection: `QualityLimits = {10, 12, 11, 6}` (Uncommon, Rare, Epic, Legendary).
- **Public functions** (`AscensionFreePick.h`): `Validate(catalog, level, wanted, previous)`,
  `Apply(player, wanted)`, `KnownEntries(player)`, `Investment(player, classType, tab, talents)`,
  `PlanSelection(player, Area52Build::Entry, wanted, retainExisting, maximumLevel)`, `LearnAvailableRanks`,
  `ActiveSpecialization`/`SwitchSpecialization`, `Purge(player, talents)`.
- **Primary stat path**: entries 1149-1152 and 18149 (spells 84864-84867, 129243). A build's `PrimaryStat` index
  selects one of `{1149, 1150, -, 1151, 1152, -, 18149}`.
- **Specializations**: up to 20 slots (`SpecializationSpells`); each keeps its picks, action bars and Mystic
  Enchant preset.

### 2. Builds (Build Creator format)

- `Area52Build::Entry` (`Area52BuildProtocol.h`): name, roles, `PrimaryStat`, `Spells` (spell id, level picked,
  role tags), `Enchants` (Mystic Enchant spell, stacks, level), equipment types, difficulty.
- Stored in the characters database (`CHAR_*_A52_BUILD*`, `Area52BuildService.cpp`).
- `Area52Build::Select` (`Area52BuildSelection.h`) turns a build into the entries due at a level.
- A followed "active build" (`Area52ActiveBuild.cpp`) re-applies **abilities and talents** as the Hero levels,
  through `PlanSelection` + `Apply`. It does **not** slot Mystic Enchants.

### 3. Mystic Enchants

- **Catalog**: client `MysticEnchant.dbc`, rows with flag bit 9 (`AscensionMysticEnchantCatalog.cpp`). Entry:
  enchant `Spell`, scroll `Item`, `Level`, `Quality` (2 Uncommon, 3 Rare, 4 Epic, 5 Legendary), `Worldforged`, and up
  to 3 requirements `Classes[i]`/`Tabs[i]` with `AbilityInvestment[i]`/`TalentInvestment[i]`.
- **Slots**: 17, one per equipment slot, stored as player setting `core.freepick.mystic.<preset>` (not on items).
  Active preset: `core.area52.mystic.active`.
- **Rules** (`ValidateSlots`): enchant level ≤ character level; quality 2-5; owned; per-enchant stack limit;
  Epic + Legendary ≤ 4; **Legendary ≤ 1**; Worldforged Rare ≤ 3; no two from an exclusive spell group.
- **Effect**: `Synchronize` (public `AscensionMysticEnchant::RefreshEquipped(player)`) adds each slotted enchant spell
  as an aura on the player, stacked by slot count, while the player is alive and **meets the investment**
  (AE and TE spent in the enchant's source tree). `SpellGrants` (`AscensionMysticSpellGrants.h`): 7 enchants that
  also grant a spell, for example 965937 → 965939.
- **Owned collection**: account-wide table `account_freepick_mystic_collection (account, spell)`.
- **Player slotting**: client packet (count, then slot + spell pairs) near a Mystic Altar, paid in gold or marks
  (item 375250). `0x617` clears a slot; `0x60C` switches the preset.
- **Bot path without core changes**: add the bot account's enchants to the collection table, write the slot
  setting, call `RefreshEquipped`. Synchronize still enforces ownership, limits and investment; the altar and the
  cost are skipped.
- **Legendaries with core scripts** (`Area52LegendaryRules.h`): Purification 84508, Exorcism 84509, Consecration
  84759, Curse 84512, Malediction 84760, Stormbringer 965500, Deathbringer 965501, Zealot 965502, Smite 965508.

### 4. How playerbots fights

- **Context per class** (`src/Bot/Factory/AiFactory.cpp`, `createAiObjectContext`): classes 1-11 hand-written;
  CoA classes 12-32 (`IsAscensionCustomClassId`) get `CoaAiObjectContext`. **Class 10 gets the plain
  `AiObjectContext`: no combat.** Class 10 is also skipped in `RandomPlayerbotMgr.cpp` (random bot class cache) and
  `PlayerbotAIConfig.cpp` (talent specs).
- **`CoaAiObjectContext`** (`src/Ai/Class/Coa/CoaAiObjectContext.cpp`): `coa attack` (rotates the class kit),
  `coa aoe`, `coa buff`, `coa dispel`, `coa purge`, `coa interrupt`, `coa taunt`, `coa defensive`, healer mana
  logic, stay near tank, threat multiplier, `coa boss dodge`, `coa pvp`. Spells are classified once into kinds
  (damage, heal, taunt, buff, dispel...) from Spell.dbc (`Classify`).
- **Class kit**: `ClassAbilities()` from the core's `GetAscensionClassAbilities(classId)`, which is empty for class
  10. `KnownAbilities(bot, filter)` keeps the kit spells the bot knows. A Hero kit must come from elsewhere: the Free
  Pick catalog (`LoadCatalog`, every entry's spells) or the bot's spellbook.
- **Authored rotations**: per specialization in `CoaSpecStrategies` (`src/Ai/Coa/CoaSpecStrategies.h`): class,
  spec, name, role, position (`close`/`ranged`), combat strategy `custom::<name>`, optional non-combat strategy,
  support flag. Rotation lines live in table `playerbots_custom_strategy`
  (`data/sql/playerbots/custom/playerbots_coa_rotations.sql`):
  `('<name>', <idx>, 0, '<trigger>::<spell>><action>::<spell>!<priority>')`, for example
  `buff missing::Inner Demon>cast buff::Inner Demon!95`, `can cast::Felbane>cast melee::Felbane!29`,
  `medium aoe>cast melee::Annihilan Strike!88`. Applied when `AiPlayerbot.CoaSpecRotations` is on
  (`AiFactory.cpp`, `GetCoaSpecStrategyFor`).
- **CoA talents**: `CoaLevelBuildData.h` + `ApplyCoaTalents`/`SpendCoaTalentPoints` (`CoaSpecialization.cpp`),
  through `CoaTalentCompat.h` on both cores.

### 5. Plan: Legendary Mystic Enchants as classes

One "class" = one Legendary enchant with:
1. a **build** (`Area52Build::Entry` shape): picks by level, primary stat, and its enchants (the Legendary plus its
   Epics/Rares) with their levels;
2. a **role and position** (tank/heal/dps, close/ranged), like a `CoaSpecStrategies` row;
3. a **rotation**: `custom::<legendary>` lines in `playerbots_custom_strategy`.

Work needed (not built yet):
- Hero → `CoaAiObjectContext`, with a kit built from the Free Pick catalog.
- A Legendary class table (enchant spell → build, role, position, rotation strategy) used for Heroes in place of
  `CoaSpecStrategies`.
- Bot setup per level: apply the build's picks (`PlanSelection` + `Apply`); add and slot the enchants (collection
  table + slot setting + `RefreshEquipped`); gear for the build's primary stat.
- Random bots: spread the Legendary classes the way classes are spread; stop skipping class 10 on a Freepick realm.

### How the owner's builds look

The owner sends builds known to work on Area 52, as screenshots of the Ascension build planner. Read them like this:

- **Level view**: per character level, the picks taken at it: spell name, `Rank n/m` for talents, and coloured
  diamonds under the name = the pick's **quality and quality cost** (orange Legendary, purple Epic, blue Rare, green
  Uncommon; the number of diamonds is `QualityCost`). The diamonds are not Mystic Enchants.
- **Summary view**: picks grouped by source tree (`<Class> - <Spec>`) with ranks; required level, required/remaining
  AE and TE; quality pick usage against the limits (Legendary 6, Epic 11, Rare 12, Uncommon 10, the same as
  `QualityLimits`); and the **Mystic Enchants** panel: enchant name + `stacks/max`, name colour = quality, 17 slots.
- Picks resolve by name: catalog entry's first spell name in Spell.dbc, matched against `CharacterAdvancement.dbc`
  rows of type Ability/Talent/TalentAbility (a proof-of-concept build matched all 66 picks). Enchant names map to
  several spell ids; `MysticEnchant.dbc` decides which is the enchant.
- Not on the screenshots, so ask per build: the primary stat, the role, and the basic rotation.
- Turn a build into an `Area52Build::Entry`-shaped record: spells with their pick level, enchants with stacks (and
  level when given), primary stat, role.

### Still to come from the owner

- Freepick `MysticEnchant.dbc` and `CharacterAdvancementClassTypes.dbc`.
- Per Legendary: the build and the basic rotation.
