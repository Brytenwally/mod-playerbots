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
- `wildcard-freepick`: bot support for **Wildcard heroes only**, started from `Zyth45:coa` (3b5a29b). Freepick and
  Mystic Enchants are out of scope for now.
- Keep this CLAUDE.md out of any upstream PR.

**Never modify the core.** Playerbots adapts to the CoA core as it is. That means no core patches, no new core exports
and no core config changes for bot work. Use only what the core already exposes. If something the bots need has no
public core API, say so and find a playerbots-side way (for example the data the core already reads) instead.

**Building**
- Compile-check inside a core checkout, with this repo at `modules/mod-playerbots`.
- Run the core's `apps/codestyle/codestyle-cpp.py --files` on changed sources.
- Do not run full builds in the cloud unless asked.

**Testing:** Claude does not test in game. Say so when reporting.

**Naming:** keep upstream naming (`vanilla-<dungeon>` strategies) to avoid merge conflicts.

## Wildcard map

This map was taken against core `7e6a5b5a` (Brytenwally/azerothcore-wotlk-coa `main`, 2026-10-20) and this branch at
3b5a29b.

**Do not re-map from scratch.** Check whether it is still current:
1. Run `git diff --stat 7e6a5b5a <new> -- src/server/coa/AscensionWildcard* src/server/coa/AscensionCompatOpcodes* src/server/game/Entities/Creature/Trainer.* modules/mod-coa-challenges`.
2. Re-map only the parts that changed in a meaningful way.
3. Update the commit above.

File shorthands: `WC` = `src/server/coa/AscensionWildcard.cpp`, `WH` = `AscensionWildcard.h`, `FP` =
`AscensionFreepick.cpp`. All are in `src/server/coa/` of the core.

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

### 4. Out of scope for now: Freepick and Mystic Enchants

- **Freepick** heroes are class 10 without the 0x40 bit (`AscensionFreepick::IsFreepickHero`, `FP:250-253`). Bots
  must not treat them as Wildcard. Their deterministic build API (`AscensionFreepick::ApplyGuide`, `FP:315`) is not
  used on this branch.
- **Mystic Enchants** (random enchants) are switched off for Wildcard by the core itself. Wildcard realms have no
  altars (`FP:477-478`), and the enchant rules reject Wildcard characters. Wildcard bots therefore get none, and this
  branch does not touch them.

### 5. Pitfalls for bots

1. **Do not inject client packets.** Wildcard opcodes go through `AscensionCompatOpcodes::Claim` →
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
6. **The stat path upload refuses during combat.**

### 6. Core API for bots (the integration surface)

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

### 7. This fork today (nothing handles Hero or Wildcard)

- **Creation.**
  - Class 10 is never created (`RandomPlayerbotFactory.cpp:883-899`; `CoaClassesOnly` keeps 12-32).
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

### 8. Plan sketch (not started; playerbots changes only)

1. **Wildcard bot creation.** Add a config share for class 10 bots, used only when the realm plays Wildcard
   (`AscensionWildcard::PlaysWildcard` on `CoAChallenges.GameModes.Realm`, or a bot's own `IsWildcardHero`). There
   the 0x40 bit comes from the realm. Give the starting kit from playerbots, since a factory-made bot has no
   `AT_LOGIN_FIRST`.
2. **Draft.** In `Randomize`, `AutoMaintenanceOnLevelupAction` and login repair, call `ClaimLevelingScrolls` +
   `DraftBuild` (with a role-biased `StarterPick`).
   - Reroll off-role entries with `UnlearnForReroll`.
   - Pick the stat path with `ApplyBuildUpload`.
   - `ClearSpells` and `DropCoaRanksAbove` must leave a Wildcard hero's rolled spells alone.
3. **Role.** Add a Hero path in `GetCoaRole` / `GetCoaStyle` / `GetCoaPrimaryStats`, derived from the `KIND_*` counts
   of known spells and from `PrimaryStat`. This feeds strategies, stat weights and armor proficiency.
4. **Combat.** Give Wildcard heroes `CoaAiObjectContext`.
   - Build `ClassAbilities` from the bot's spellbook (cached `Classify`).
   - Skip the authored `CoaSpecRotations`.
5. **Ranks.** Teach higher ranks of rolled abilities as the bot levels, from the same rows the core's trainer offers
   (`RankTrainerRows`), with `learnSpell`.
6. **Alts.** A player's own Wildcard alt keeps its draft. Add only, never remove.
