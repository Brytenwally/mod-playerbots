# CLAUDE.md: mod-playerbots, CoA fork (`Brytenwally/mod-playerbots`)

This is the owner's fork of the CoA mod-playerbots (upstream fork: `Zyth45/mod-playerbots`, branch `coa`). The
module runs inside the CoA AzerothCore core (`Brytenwally/azerothcore-wotlk-coa`).

## Working rules

**Pushing and pull requests**
- Push only to `Brytenwally/*` forks.
- Open a pull request against `Zyth45/mod-playerbots` only when the owner asks for one, and then from a clean branch
  based on `Zyth45:coa`.
- Never push new work to `coa`: it is the head of an upstream PR.

**Branches**
- `coa-bot-strategies`: dungeon and raid strategies. The owner's build (`Brytenwally/coa-build`) uses this branch.
- `coa-dungeon-strategies`: the dungeon strategies only, for upstream.
- `wildcard-freepick`: Wildcard, Freepick and Mystic Enchant support for bots, started from `Zyth45:coa` (3b5a29b).
- Keep this CLAUDE.md out of any upstream PR.

**Building**
- Compile-check inside a core checkout, with this repo at `modules/mod-playerbots`.
- Run the core's `apps/codestyle/codestyle-cpp.py --files` on changed sources.
- Do not run full builds in the cloud unless asked.

**Testing:** Claude does not test in game. Say so when reporting.

**Naming:** keep upstream naming (`vanilla-<dungeon>` strategies) to avoid merge conflicts.

## Wildcard / Freepick / Mystic Enchant map

This map was taken against core `7e6a5b5a` (Brytenwally/azerothcore-wotlk-coa `main`, 2026-10-20) and this branch at
3b5a29b.

**Do not re-map from scratch.** Check whether it is still current:
1. Run `git diff --stat 7e6a5b5a <new> -- src/server/coa/AscensionWildcard* src/server/coa/AscensionFreepick* src/server/coa/AscensionMysticEnchant* src/server/coa/AscensionBuildCreator.cpp src/server/coa/AscensionCompatOpcodes* src/server/game/Entities/Creature/Trainer.* modules/mod-coa-challenges`.
2. Re-map only the parts that changed in a meaningful way.
3. Update the commit above.

File shorthands: `WC` = `src/server/coa/AscensionWildcard.cpp`, `WH` = `AscensionWildcard.h`, `FP` =
`AscensionFreepick.cpp`, `ME` = `AscensionMysticEnchant.cpp`, `MR` = `AscensionMysticEnchantRules.cpp`, `MRH` =
`AscensionMysticEnchantRules.h`. All are in `src/server/coa/` of the core.

### 1. Hero modes in one view

- **Class.** Wildcard and Freepick characters are class 10, `CLASS_HERO` (`SharedDefines.h:136`). CoA classes are
  12-32 (`IsAscensionCustomClassId`, `AscensionCompat.cpp:10060`). Stock classes are 1-9 and 11.
- **Freepick** is the classless "pick anything" mode, Ascension's Character Advancement.
  - Active when realm config `CoA.ClassModel = "hero"` (`FP:476`).
  - `AscensionFreepick::IsFreepickHero` = classless realm + Hero + not Wildcard (`FP:250-253`).
  - `HasFreepickBuild` also covers Warcraft Reborn stock classes (`CoA.ClassModel="wcr"`, `FP:255-264`).
- **Wildcard** is a Hero with game-mode bit `0x40` (`GAME_MODE_WILDCARD`, `WH:23`).
  - Check it with `AscensionWildcard::IsWildcardHero(player)` (`WC:3291`); `Trainer.cpp:36-42` holds a core
    duplicate.
  - The mask comes from `PlayerScript::OnPlayerGetGameModeMask`, implemented only by mod-coa-challenges
    (`CoA.Challenges.Scripts.cpp:1302`).
  - That mask mirrors table `coa_character_gamemode(guid, gameMode)` (`CoA.Challenges.GameModes.cpp:154-200`).
  - `RecomputeRequiredGameModes` (`CoA.Challenges.Lifecycle.cpp:148-173`) ORs in the realm mask from
    `CoAChallenges.GameModes.Realm` ("WildCard" maps to 0x40).
  - It runs before `OnPlayerLogin`.
- **Wildcard realm config** (`src/server/coa/conf/wildcard.conf.dist`): `CoAChallenges.GameModes.Realm = WildCard`,
  `CoA.GameModeMask = 64`, `CoA.ClassModel = "hero"`, `CoA.MapClass10ToWarrior = 0`.
  - Elsewhere, class 10 can be remapped to Warrior at creation (`CharacterHandler.cpp:283-290`).
- **Rulesets are something else.** `AscensionRulesets.*` (War Mode / High Risk / PvE auras) is not Wildcard. This fork
  already applies rulesets in `RandomPlayerbotMgr::CoaApplyRuleset` (`RandomPlayerbotMgr.cpp:3289-3340`).

### 2. Wildcard spells (random rolls, not offers)

- **Catalog.** `LoadTables` (`WC:508-597`) reads entries from `CharacterAdvancement.dbc` of type Ability,
  TalentAbility or Talent.
  - Glyphs, `DRAFT_ONLY` and mode≠1 are excluded.
  - Each becomes an `Entry{EntryId, Talent, MinLevel, Group, RankSpells, AbilityCost}` (`WH:89`).
  - The budget is cumulative AE (Ability Essence) and TE (Talent Essence) per level, from
    `CharacterAdvancementEssence.dbc` (Hero + Wildcard rows).
- **Costs and pool levels.**
  - Ability = DBC AE (field 22, default 2); AE 0 is never rolled.
  - Talent = 1 TE (`WH:28-29`).
  - `PoolLevel` (`WC:2476`) decides whether another roll fits. Talents start at level 10 (`WH:30`).
- **Starter abilities.** `RerollStartingAbilities` (`WC:1449`, client 0x064B) rolls 4 starters.
  - Starter weights come from `AscensionWildcardStarterData.h` (`FirstAbility`, `OtherAbilities`).
  - Allowed only at level ≤ 9 or while no slots exist.
  - **Not automatic at login.**
- **Level rolls.** `RollAbilities` (`WC:1615`, client 0x0643) → `RollLevelEntry` (`WC:2611`).
  - It rolls a talent when the talent pool is behind, otherwise an ability.
  - The pick is 65% synergy-weighted, otherwise uniform.
  - Abilities come at rank 1; talents at a random rank 1..N.
  - It is stored by `Place` (`WC:2752`) and taught only by `RevealLearnedEntry` (`WC:1058`), which calls
    `player->learnSpell(RankSpells[rank-1])`.
  - **Nothing rolls automatically on level-up.** The client only gets `WILDCARD_ROLL_READY`.
- **Rerolls (Scrolls of Fortune).** `UnlearnWithScroll` (`WC:1658`, client 0x061D) spends one token and frees the
  slot; the next roll refills it.
  - It is refused while the entry is locked or AE/TE is unspent (`CheckUnlearn`, `WC:2762`).
  - Tokens are per spec, stored in player settings.
  - Leveling rewards (`LEVELING_REWARDS`, `WH:71`) at 10/20/30/40/50/60: 2/4/6/8/10/12 generic + 6/8/10/12/14/16 talent
    scrolls.
- **Locking.** 0x0658 / 0x065A → `SetLocked` (`WC:1729`).
- **Stat path.** Entries 1149–1152 or 18149 (`WH:56`) are chosen via the 0x0727 upload → `ApplyBuildUpload`
  (`WC:3422`).
  - The upload must echo every rolled slot plus one new path entry.
  - Not allowed in combat.
- **Ability ranks.** These come from the class trainer's synthetic rows: `RankTrainerRows` (`WC:1952`),
  `Trainer::SetWildcardRankRows` (`Trainer.cpp:451-470`).
- **Specializations.** There are 20 (`SPECIALIZATION_SWAP_SPELLS`, `WH:38`). Cast the swap spell →
  `SwitchSpecialization` (`WC:1818`). Prestige: `PrestigeSpecialization` (`WC:3309`).
- **First login (`AT_LOGIN_FIRST` only)** (`WC:2255-2264`):
  - starting kit spells and items (`GiveStartingKit`, `WC:2001`)
  - entry companion spells (`GrantEntrySpells`, `WC:1990`)
  - Dice of Destiny
  - first specialization
  - `GiveSpecializationCache` skips bots.
- **Skill cards and Darkmoon packs** only bias rolls. They are optional for bots.
- **Per-spell scripts.** `AscensionWildcard{Arcane,Bloodbath,CorruptedBear,CorruptedBlade,Cremation,DarkIntent,ElementalBlast,Physical,Recuperate,SealOfFervor,SolarStrike}.cpp`
  are spell-effect fixes. Bots need nothing special for them.

### 3. Wildcard talents

- Wildcard does **not** use the CoA class talent system (`AscensionCoATalentState`, `SetAscensionTalentRank`), which
  is for classes 12-32 only.
- Wildcard "talents" are Talent-type entries, rolled like abilities for 1 TE each, at a random rank, with no point
  spending.
- They are removed only by a scroll unlearn or by prestige.

### 4. Freepick spells and talents (deterministic)

- **Rules.** `AscensionFreepickRules.cpp` holds the catalog plus `BuildRuleOk` (`:377`).
  - It checks level, expansion, class, realm visibility, connected and required nodes, exclusive groups, and AE/TE
    thresholds.
  - `UnitRules` (`FP:140`): not in combat, battleground or arena; alive.
  - `AutoLearn` (`:548`) grants the free rows; `CheckApply` (`:645`) prices removals in gold or Marks (375250).
- **Entry points.**
  - `AscensionFreepick::ApplyGuide(player, {GuideSpell{spell, level}…}, primaryStat)` (`FP:315`).
  - `ApplyUpload(player, entries)` (`FP:306`).
  - `SwitchSpecialization(player, index, err)` (`FP:338`).
  - `Synchronize` (`FP:419`, at login).
  - `SyncSpells` (`FP:217`) does the `learnSpell` / `removeSpell`.
- **Template.** Hero Architect's `AutoLearn` (`AscensionBuildCreator.cpp:417-447`) applies a Freepick guide plus a
  Mystic Enchant guide. Copy this pattern for bots.

### 5. Mystic Enchants (random enchants)

- **Slots, not items.** There are 17 character slots (`MRH:15`; 1-15 under class fusion) and up to 100 presets.
  - An applied enchant is a passive aura on the player, plus an optional taught spell.
  - There is no item enchantment and no equip hook.
- **Catalog.** `MysticEnchant.dbc`, loaded by `LoadCatalog` (`MR:283-302`).
  - Fields: Id, Spell, Weight, Quality, RebornQuality, RequiredLevel, Item (the scroll), Worldforged, Realms[5],
    ClassMask, RequiredAE/TE.
  - An enchant is identified by its **aura spell id** everywhere.
  - Hero uses `Quality`.
- **Per-character state.** `State` (`ME:94-104`): Progress/Level (the altar level), ActivePreset, Presets
  (`array<uint32,17>` of spell ids), Known, Auras.
- **DB tables.** `coa_mystic_enchant`, `coa_mystic_enchant_known` and `coa_mystic_enchant_slot`.
  - They are written immediately and loaded at `OnPlayerLogin` (`ME:760-768`).
  - **If `OnPlayerLogin` didn't run, every call is a no-op.**
- **Learning.**
  - Only `Learn()` (`ME:339-349`, internal) adds to Known.
  - It is reached only by extracting a scroll (0x0607) or a slotted enchant (0x0608).
  - An extract needs an altar GameObject within 10 yd (`ME:87-90`) and Mystic Extract 98463, about one per enchant.
  - Scroll sources:
    - Unidentified Mystic Scroll 97866 drops: 4% from non-boss creatures above level 10; bosses drop level/10.
    - The reveal roll `RollReveal` (`MR:971-1010`) favours enchants that affect a spell the Hero knows.
    - Reforge (0x0603).
    - Vendors 9781002-9781011, for Freepick heroes only.
- **Applying.** The final effect is `SyncAuras` (`ME:214-238`): `AddAura` + `SetStackAmount` + `learnSpell(Taught)`.
  - Scroll apply (0x0610) consumes the scrolls and needs no altar.
  - Collection apply (0x060F) needs an altar and Runes or gold.
  - **Public API: `AscensionMysticEnchant::ApplyGuide(player, std::vector<GuideEnchant{Spell, Stacks, Level}>)`**
    (`AscensionMysticEnchant.h:13`, `ME:1014-1033`; `GuideEnchant` at `MRH:393`).
    - No altar, no cost, no packet; all caps enforced.
    - **The enchants must already be in Known.**
- **Caps** (`QualityCap`, `MR:361-410`):
  - uncommon 17, rare 17, epic 3, legendary 1, artifact 1;
  - with fusion: 6 uncommon / 5 rare / 4 epic;
  - rare worldforged ≤ 3;
  - unlock levels come from `CONFIG_*_UNLOCK_LEVEL_n`.
- **Wildcard gets no Mystic Enchants. This is deliberate.**
  - `MysticAltars = (Classless || Reborn) && !PlaysWildcard(realm)` (`FP:477-478`).
  - The rules reject `Character::Wildcard()` (realm `CoA.GameModeMask` & 0x40) at `MR:204, 445, 595, 677, 831, 884`.
  - Enabling them for Wildcard means changing those core gates. That is a design decision for the owner.
- **Classes 12-32** get BAD_CLASS from most enchant operations.

### 6. Pitfalls for bots

1. **Do not inject client packets.** Wildcard and Mystic opcodes go through `AscensionCompatOpcodes::Claim` →
   `QueueRequest`, which queues **per account**.
   - Wildcard drains only on non-bot sessions (`WC:2287-2314`), so a bot's packet never runs, or runs on the human's
     character.
   - The opcode hook is socket-only (`AscensionCompat.cpp:7949`).
   - **Call the C++ APIs.**
2. **No automatic rolls.** Roll after creation and after every level-up.
3. **The starting kit needs `AT_LOGIN_FIRST`**, which only `HandleCharCreateOpcode` sets (`CharacterHandler.cpp:595`).
   Bot factories creating a Player directly get no kit.
4. **The Wildcard bit comes from the realm or a `coa_character_gamemode` row.** Otherwise a Hero is Freepick, or a
   Warrior if `MapClass10ToWarrior=1`.
5. **The reroll economy is per spec.** `CheckUnlearn` refuses while essence is unspent: roll first, reroll after.
6. **The stat path upload and Freepick changes refuse during combat**, death, battlegrounds and arenas.

### 7. Core API for bots (the integration surface)

**Wildcard** (`AscensionWildcard::`):

| Call | What it does |
|---|---|
| `IsWildcardHero` | Mode check. |
| `DraftBuild(player, StarterPick = {}, starterDraws = 1)` (`WC:3439`) | Rolls the starters if there are none, then `RollAbilities` until the budget is spent (≤200). Call at creation and on each level-up. |
| `ClaimLevelingScrolls(player)` (`WC:3449`) | Credits the leveling reward tokens. |
| `UnlearnForReroll(player, entryId)` (`WC:3469`), then `DraftBuild` | Rerolls an unwanted entry. |
| `Slots`, `KnownEntries`, `LoadedTables().Entries`, `PrimaryStat`, `ActiveSpec` | Inspect the hero's state. |
| `ApplyBuildUpload(player, KnownEntries(player) + {pathEntry, 1})` | Chooses the stat path. |
| `PrestigeSpecialization` | Prestige. |

**Freepick** (`AscensionFreepick::`):

| Call | What it does |
|---|---|
| `IsFreepickHero`, `HasFreepickBuild`, `RealmIsClassless`, `RealmOffersMysticAltars` | Mode and realm checks. |
| `ApplyGuide(player, spells, primaryStat)` | Applies a build. |
| `ApplyUpload` | Applies a full known-entries upload. |
| `SwitchSpecialization` | Switches specialization. |
| `InvestedEssence` | Essence invested. |

**Mystic Enchants** (`AscensionMysticEnchant::`):

| Call | What it does |
|---|---|
| `ApplyGuide(player, guide)` | Slots known enchants. |

**Learning an enchant without the altar flow has no public API.** It needs a new core export such as
`LearnForBot(player, spell)` exposing `Learn()` (`ME:339`), shipped as a core change. Alternatively, insert into
`coa_mystic_enchant_known` before the bot logs in.

### 8. This fork today (nothing handles Hero, Wildcard, Freepick or Mystic Enchants)

- **Creation.**
  - Class 10 is never created (`RandomPlayerbotFactory.cpp:883-899`; `CoaClassesOnly` keeps 12-32).
  - The Mystic tables are only purged (`RandomPlayerbotFactory.cpp:677-679`).
- **Spec and talents for CoA classes.**
  - `EnsureCoaSpecialization` (`src/Ai/Class/Coa/CoaSpecialization.cpp:304-383`, random bots only).
  - `ApplyCoaTalents` (`:495-578`, random bots) and `SpendCoaTalentPoints` / `SpendAltBotTalentPoints` (`:580-649`,
    alts, add-only).
  - Builds come from `CoaLevelBuildData.h`.
  - Role, style and primary stats come from `GetCoaRole`, `GetCoaStyle` and `GetCoaPrimaryStats` (`:261-294`).
- **Spells.** `PlayerbotFactory::Randomize` (`src/Bot/Factory/PlayerbotFactory.cpp:829-1091`) runs `ClearSpells`
  (`:1939`), which for CoA classes only calls `DropCoaRanksAbove`.
  - `InitClassSpells` is vanilla-only.
  - `InitAvailableSpells` uses trainers.
- **Gear.** `InitEquipment` (`:2689-3098`) and `CoaUpdateGear` (`RandomPlayerbotMgr.cpp:3342-3456`).
  - `ApplyEnchantAndGemsNew` (`:5629-5815`) is ordinary WotLK enchanting.
  - Stat weights: `StatsWeightCalculator.cpp:280-317` (CoA classes only; Hero falls through to the WotLK tabs).
- **Combat AI.** `AiFactory.cpp:44-77` gives `CoaAiObjectContext` only to classes 12-32.
  - `ClassAbilities()` / `KnownAbilities()` (`src/Ai/Class/Coa/CoaAiObjectContext.cpp:858-984`) use the class
    catalogue ∩ spellbook.
  - Spells are classified data-driven (`Classify`, `:695-857`, `KIND_*`), so this suits Wildcard if the candidate
    list comes from `bot->GetSpellMap()`.
  - Example of that pattern: `CoaTankThreatBuffs`, `:4955+`.
- **Lifecycle hooks.**
  - `RandomizeFirst` (`RandomPlayerbotMgr.cpp:2460`)
  - `OnBotLoginInternal` (`:3458+`)
  - `AutoMaintenanceOnLevelupAction::Execute` (`:17-46`)
  - alt `init=` (`PlayerbotMgr.cpp:854-900`)

### 9. Plan sketch (not started)

1. **Hero bot creation.** Add a config share for class 10 bots, gated on `RealmIsClassless()`.
   - On a Wildcard realm the bit comes from the realm; otherwise write a `coa_character_gamemode` row through
     mod-coa-challenges.
   - Give the starting kit explicitly.
2. **Wildcard.** In `Randomize`, `AutoMaintenanceOnLevelupAction` and login repair, call `ClaimLevelingScrolls` +
   `DraftBuild` (with a role-biased `StarterPick`).
   - Reroll off-role entries with `UnlearnForReroll`.
   - Pick the stat path.
3. **Freepick.** Use `ApplyGuide` with bot-side guides per role (like `CoaLevelBuildData`).
4. **Role.** Add a Hero path in `GetCoaRole` / `GetCoaStyle` / `GetCoaPrimaryStats`, derived from the `KIND_*` counts
   of known spells and from `PrimaryStat`. This feeds strategies, stat weights and armor proficiency.
5. **Combat.** Give Heroes `CoaAiObjectContext`.
   - Build `ClassAbilities` from the bot's spellbook (cached `Classify`).
   - Skip the authored `CoaSpecRotations`.
6. **Mystic Enchants.** Only for Freepick or Reborn on altar realms.
   - Make enchants known (needs a core export), then call `ApplyGuide` next to `ApplyEnchantAndGemsNew` and in
     `CoaUpdateGear`.
   - Wildcard stays excluded unless the owner changes the core gates.
7. **Alts.** A player's own Wildcard or Freepick alt keeps its draft. Add only, never remove.
