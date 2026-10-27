# CLAUDE.md: mod-playerbots, Area 52 Freepick (`Brytenwally/mod-playerbots`, branch `area52-freepick`)

This branch adds bot support for **Freepick Heroes** on the Area 52 server. It started from `coa-bot-strategies`
(1c08cc1). The core is `Brytenwally/azerothcore-wotlk-coa` `main`: the Area 52 server source with upstream CoA
updates applied. The Windows build is `Brytenwally/Freepick-build`.

## Context from the owner

- On Freepick there are no classes, only the **Hero** (class 10). Every level a Hero gains ability points (AE) and
  talent points (TE) and spends them freely on spells and talents from every source tree.
- A Hero's role and rotation come from its picks together with its **Mystic Enchants**, above all its
  **Legendary Mystic Enchant**.
- **Random bots start with every Mystic Enchant learned at creation** (the owner's rule): add all of them to the bot
  account's collection, then slot the ones its Legendary class uses.
- **The level cap is 60 for now.** Builds go to 70 or 80; their later picks wait for a higher cap.
- **Each Legendary Mystic Enchant is treated as a class.** The owner will provide, per Legendary, a build and a basic
  rotation. Until then, do not guess builds or rotations.

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

**Class changes:** whenever a Legendary class is changed or tweaked (build, enchants, weights, rotation), commit
and push the repo change, and also hand the owner a ready-to-run SQL file with the changed `area52_*.sql` content,
so they can apply it to the live playerbots database at once and restart instead of waiting for a build.

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
  Freepick's current file (2026-10-26): 10,255 rows; a Hero picks from class types 2-13 only (vanilla classes,
  General, Hero): 5,306 rows (2,486 Ability, 2,529 Talent, 291 TalentAbility). Class types, tabs (one spec list
  shared by all classes), categories (spell tags, field 160) and the field layout:
  `docs/area52/advancement-reference.md`.
  Entry ids change between client versions: match picks by spell id.
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
  Freepick's file (WDBC, 31 fields, 124-byte rows; fields: 0 id, 1 spell, 3 quality string, 5 level, 6 item,
  7 worldforged, 8 must be non-zero, 13 flags, 19-21 classes, 22-24 tabs, 25-27 AE, 28-30 TE): 3,822 usable rows,
  170 Legendary, 575 Epic, 1,498 Rare, 1,579 Uncommon. The 170 Legendaries are listed in
  `docs/area52/legendary-mystic-enchants.tsv`: 26 Worldforged, 5 with an investment requirement (15 TE in one of
  three trees), almost all level 0.
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
  CoA classes 12-32 (`IsAscensionCustomClassId`) and Freepick Heroes (`IsCoaHero`) get `CoaAiObjectContext`.
  Class 10 is also skipped in `RandomPlayerbotMgr.cpp` (add-class bot cache) and
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

Built (`src/Ai/Coa/CoaLegendaryClasses.{h,cpp}`):
- **Classes are data** in the playerbots database, read once at startup (`CoaLegendaryClasses()`, called from
  `OnBeforeWorldInitialized`): `playerbots_legendary_class` (settings: role, position, primary stat, rotation names,
  random-bot chance, armor, shield, weapons, pet), `playerbots_legendary_pick` (spell, level),
  `playerbots_legendary_enchant` (slot 1-17, spell) and `playerbots_legendary_weight` (stat name, weight). The owner's
  rule (27/10): **every class and build goes in these tables**; a tweak needs a restart, no new build. Source of the
  shipped rows: `docs/area52/classes/classes.tsv` plus each class's `-build.tsv` and `-enchants.tsv`;
  `apps/area52/generate_legendary_class_data.py` writes `data/sql/playerbots/custom/area52_legendary_classes.sql`
  (rerun it after changing a TSV). Bad names in a row are logged and the row skipped. Code changes are needed only
  for new triggers, actions or mechanics.
- **Hero detection**: `IsCoaHero` = class 10 and `AscensionFreePick::Applies`. A Hero's class is the Legendary slotted
  in its active enchant preset (`GetCoaLegendaryClass`); no other storage.
- **Applying a class** (`ApplyCoaLegendaryClass`): `PlanSelection` + `Apply` with the class's picks (those in the
  catalog) and primary stat. The collection: `CoaFillMysticCollection` writes every enchant the server supports
  (`AscensionMysticEnchant::IsConnected`, added to core `main` 27/10 with the owner's OK) into
  `account_freepick_mystic_collection` for the bot account before the bot logs in (`HandlePlayerBotLoginCallback`),
  and the core loads it at login. An alt bot fills its owner's account: filled with the whole catalog, that altar
  offered 3,822 enchants and the core silently refused all but the 84 connected ones. A bot session never runs
  `WorldSession::Update`, so `PlayerbotHolder::UpdateSessions`
  drains its query processor (`GetQueryProcessor().ProcessReadyCallbacks()`): without it the core's collection load
  never finished and a bot wore no enchant at all (no Hoplite aura, no shield beside the polearm). The core's
  `mystic grant` is not used for bots: its completion goes through the session's private transaction callbacks,
  which never run for a bot, and it left the collection busy, refusing every later `mystic equip`. The core puts on
  no enchant while any slotted one is invalid (not connected on that server, or over its stack limit), so only the
  Legendary is written straight to `core.freepick.mystic.<preset>` (the other slots emptied) and the rest go through
  `mystic equip <slot> <spell> <name>`, which refuses an invalid enchant alone; slots still differing once the
  Legendary is on (equips refused before the collection loaded) are asked for again, up to 5 times a minute
  apart. (The server's Spell.dbc, 239,065 rows, has every enchant of the
  three classes; older copies lacked some.)
- **Changing class** (`talents spec`, or a tweaked build after a restart): `Apply` swaps the picks;
  `RemoveUnpickedSpells` then removes Free Pick spells of entries the bot has not picked (trainer, quest or old class
  leftovers), keeping enchant grants and, for a class with a pet, the pet commands; `LeaveClass` removes the old
  class's pet and re-gears a random bot. The core accepts every switch between the three classes (checked offline).
- **Random bots**: `EnsureCoaLegendaryClass` gives a classless Hero random bot a class by a stable roll: first a role
  (`AiPlayerbot.Area52HeroTankChance` 20, `Area52HeroHealerChance` 20, the rest damage), then a class of that role
  weighed by its `chance` column (equal values in a role: equal shares; 0 keeps a class off random bots;
  the shipped 20/30/30/20 also read right as percents for a server still on the older roll). It keeps its picks at its
  level. Called from `InitTalentsTree`, the combat
  strategy setup and the non-combat strategy `coa legendary` (trigger `coa legendary unready`, action
  `coa legendary setup`). `AiPlayerbot.Area52HeroBots` (default 1) makes the random bot factory create Heroes only.
- **Master command**: `talents spec <name>` applies a class (spec or name, no category needed). `talents spec list`
  explains the categories; `talents spec list tank|healer|mdps|rdps` lists that category, from the class's `role`
  and `position` (dps + close = mdps, dps + ranged = rdps).
- **`.playerbot bot addclass <class>`** takes a Legendary class by its `spec` or `name` from
  `playerbots_legendary_class` (case, spaces and punctuation ignored: `mknight`, `mysticknight`). It logs in a Hero
  from the addclass accounts (the addclass cache holds class 10 when `CoaHeroBotsWanted()`) and queues the class
  with `CoaRequestLegendaryClass`; `EnsureCoaLegendaryClass` applies it after login, over any class the Hero had.
  A new class row works with addclass after a restart, no rebuild.
- **Play**: Heroes get `CoaAiObjectContext`; their role (`GetCoaRole`, `PlayerbotAI::IsTank/IsHeal/IsDps/IsRanged`)
  and style come from the class; combat strategies as the CoA classes ("coa tank" + "tank assist", "coa heal",
  "coa"/"coa ranged" + "dps assist") plus `custom::<class>` and its position; non-combat adds `custom::<class>-nc`.
- **Gear**: armor up to the class's heaviest type, a shield only if it carries one, only its weapons; stat weights
  from the class. The core lets a Hoplite hold a shield next to its polearm only while the Legendary aura is on
  (`Player::CanUseTwoHandWithShield`), and that aura comes after the first gear pass: once it is on,
  `EnsureCoaLegendaryClass` equips the best shield from the bags, or for a random bot runs the gear pass for the
  off-hand. `ItemUsageValue` counts such a shield as an upgrade (`CoaHeroHoldsShieldWith`). Before the aura is on,
  `InitEquipment` (random gear, `autogear`) puts the best shield in the bags instead (`CoaShieldWaitsForLegendary`,
  only one better than every shield carried), and `ItemUsageValue` keeps such a shield. A class with a shield looks
  at shields only for its off-hand (`GetPossibleInventoryTypeListBySlot`): the 25-candidate list, with a quarter
  skipped at random, otherwise filled with unusable off-hand items and some Hoplite bots got none. The off-hand step
  (`OffHandMissing`, `EquipLegendaryOffHand`) runs whenever such a class has an empty off-hand, and for a class without
  a shield once Titan's Grip is on (a Crusader's second two-hander from level 60); an off-hand-only gear pass also runs
  below level 5.
- **Points**: a Hero's spells come only from its picks, so its AE and TE are spent: class trainers
  (`InitAvailableSpells`) and class quest reward spells are skipped for Heroes. `EnsureCoaLegendaryClass` logs a
  refused apply at info level ("could not take its <class> picks"). The three class builds pass the core's `Select` and
  `Validate` at every level 1-60 (checked offline against the 10,255-row `CharacterAdvancement.dbc`).
- **Dungeon Finder role**: `LfgJoinAction::GetRoles` takes the role from the bot's combat strategies (tank or heal type,
  else damage), for every class; the talent-tab guess only remains for a bot without combat strategies.
- **Buffs**: `CoaEquivalentBuffWorn` (checked in `PlayerbotAI::CanCastSpell` for CoA classes and Heroes) leaves a
  buff of a minute or more that does not stack while the target wears it in any rank or greater form, one the core keeps
  exclusive with it (spell specific, spell group) or one with the same stat effects.
- **Proficiencies**: the core teaches a Hero every weapon and armor proficiency at login. `ClearSpells` leaves a
  Hero's spells alone (its picks are kept by `EnsureCoaLegendaryClass`), and `InitSkills` gives a Hero every weapon
  and armor skill again after `ClearSkills`.

Classes written:
- **Mystic Knight** (Eldritch Knight 81116, tank), **Hoplite** (978217, melee damage), **Naturalist** (Seed of Life
  965937, healer), **Crusader** (Consecrated Strikes 965295, melee damage, two two-handers from 60 with Titan's
  Grip): `docs/area52/classes/<class>.md`, its `-build.tsv` and `-enchants.tsv`, rotation rows in
  `data/sql/playerbots/custom/area52_<class>.sql`.

A class file holds: the Legendary and what its enchants swap in (rotation lines name the swapped-in spells), the
build by level, the 17 enchant slots, role/position/stat/gear, the rotation with the owner's rules and the defaults
the owner accepted, and what still needs code. When the guide leaves enchant slots empty, propose Uncommons that
improve the build's core spells and let the owner choose.

Triggers added for Legendary classes (`src/Ai/Coa/CoaGenericContext.h`): `coa all::<a>&<b>` (all of them active),
`coa steady aggro` (every fighting enemy on the bot), `coa aggro lost` (a fighting enemy on someone else),
`coa channeling::<spell>`, `coa party low mana::<members>,<percent>`, `coa none::<a>&<b>` (none active),
`coa party hurt::<members>,<percent>`, `coa party aura count::<aura>,<members>`,
`coa party aura spread::<aura>,<members>`, `coa tank aura below::<aura>,<stacks>`, `coa tank health::<min>,<max>`,
`target stacks below max::<aura>`, `target aura expiring::<aura>,<seconds>`, `last cast not::<spell>`,
`coa no imbue`, `coa enemy casting::<kick>[,alone|,<ms>]`. Actions: `cast on tank::<spell>`,
`coa spread::<spell>[,others][,tank][,heal][,dps]`, `cast on aura::<spell>,<aura>`,
`coa kick::<kick>[,alone|,<ms>]` (shares the claims of the interrupt coordination), `cast imbue::<spell>`;
break off a channel with the base action `cancel channel`.

Legendary pets (`src/Ai/Coa/CoaLegendaryPets.h`): a Legendary that plays with a pet gives the bot a fixed creature and
the pet commands (Call, Dismiss, Revive Pet, learnt inside `AscensionFreePick::SpellLearningScope`) as soon as its
aura is on; no taming. `PlayerbotFactory::InitPet` and the action `coa legendary pet` use it. Hoplite → Winter Wolf.

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

- DBCs: `MysticEnchant.dbc`, `CharacterAdvancement.dbc`, `CharacterAdvancementClassTypes.dbc` and
  `CharacterAdvancementCategories.dbc` arrived 2026-10-26 (summarised in `docs/area52/`).
- Per Legendary: the build and the basic rotation (done: Mystic Knight, Hoplite, Naturalist, Crusader).
