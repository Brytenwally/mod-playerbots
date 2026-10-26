# Freepick Character Advancement reference

From the Freepick client DBCs the owner sent on 2026-10-26: `CharacterAdvancement.dbc` (10,255 rows),
`CharacterAdvancementClassTypes.dbc` (46 rows), `CharacterAdvancementCategories.dbc` (51 rows).

## Source trees a Hero can pick from

The core keeps class types whose column 2 is 10 (`AscensionFreePick::LoadCatalog`): the ten vanilla classes, General
and Hero. The CoA class trees (Barbarian to Runemaster) and the Reborn trees are not part of Free Pick.

| Class type | Name | Class type | Name |
|---|---|---|---|
| 2 | Hunter | 8 | Druid |
| 3 | Warrior | 9 | Shaman |
| 4 | Rogue | 10 | Paladin |
| 5 | Mage | 11 | Death Knight |
| 6 | Priest | 12 | General |
| 7 | Warlock | 13 | Hero |

In this file: 5,306 Hero-pickable rows (2,486 Ability, 2,529 Talent, 291 TalentAbility). General holds 1,803 of them.

## Tabs (specializations)

`Tab` (field 33) is one list shared by every class. It is also the `Tabs[i]` of Mystic Enchant requirements.

| Tab | Spec | Tab | Spec | Tab | Spec |
|---|---|---|---|---|---|
| 2 | Survival | 11 | Arcane | 20 | Restoration |
| 3 | Marksmanship | 12 | Frost | 21 | Feral |
| 4 | Beast Mastery | 13 | Fire | 22 | Balance |
| 5 | Fury | 14 | Holy | 23 | Elemental |
| 6 | Arms | 15 | Shadow | 24 | Enhancement |
| 7 | Protection | 16 | Discipline | 25 | Retribution |
| 8 | Combat | 17 | Affliction | 26 | Blood |
| 9 | Subtlety | 18 | Demonology | 27 | Unholy |
| 10 | Assassination | 19 | Destruction | 28 | General |

Example: Crazy Cultist needs 15 TE in class type 7, tab 17, 18 or 19 = Warlock Affliction, Demonology or
Destruction.

## Categories (spell tags)

Field 160 of `CharacterAdvancement.dbc` is the pick's category; 0 = untagged (most talents).

| Id | Category | Id | Category |
|---|---|---|---|
| 1 | Core Damage | 16 | Resurrections |
| 2 | Power Ability | 17 | Passives |
| 3 | Mobility | 18 | Resource Replenishment |
| 4 | Buffs | 19 | Damage Over Time |
| 5 | Debuffs | 20 | Shapeshifts, Stances, and Transformations |
| 6 | Everything Else | 21 | Combo Point Spenders |
| 7 | Crowd Control | 22 | Mastery |
| 8 | Area of Effect | 23 | Pets |
| 9 | Utility | 24 | Threat |
| 10 | Core Healing | 25 | Weapon Enhancements |
| 11 | Dispels | 27 | Temp Category |
| 12 | Offensive Cooldowns | 28 | Group Buffs |
| 13 | Healing Cooldowns | 29 | Damage |
| 14 | Defensive Cooldowns | 30 | Healing |
| 15 | Support Cooldown | | |

Categories 31-52 are planner groupings ("Recently Unlocked", "Soon to Unlock"), not tags. Ascension's own tagging can
differ from the obvious one: Healing Wave is Power Ability (2), not Core Healing (10).

## CharacterAdvancement.dbc fields used

WDBC, 179 fields, 692-byte rows, packed after field 120 or so.

| Field | Meaning |
|---|---|
| 0 | entry id |
| 1 | type string: Ability, Talent, TalentAbility, None |
| 2-4 | required entries |
| 5-13 | spell per rank |
| 14 / 15 | AE / TE cost |
| 16 | quality string (Normal, Poor, Uncommon, Rare, Epic, Legendary) |
| 17 | quality cost |
| 26 | required level |
| 29 | group |
| 32 / 33 | class type / tab |
| 34-39 | investment |
| 40 / 41 | points / required points |
| 154-156 | masteries |
| 160 | category |
| 163 | probably the order within the category (unverified) |

Entry ids differ between client versions (the earlier copy had 7,820 rows): match picks by spell id, not entry id.
