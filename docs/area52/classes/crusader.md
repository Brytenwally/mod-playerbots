# Crusader

A Legendary "class": a Hero built around the Legendary Mystic Enchant **Consecrated Strikes** (965295).
Source: the owner's screenshots of the player guide "Crusader" by Literally Elbowttv (Archetypes window, build only,
overview "Placeholder for Crusader", no rotation page, read 2026-10-27), with the owner's answers of the same day.
Resolved against the Freepick DBCs and the core's Free Pick catalog.

| | |
|---|---|
| Legendary | Consecrated Strikes, 965295 |
| Role / position | damage / close (`mdps`) |
| Primary stat | Strength |
| Equipment | Plate; a two-handed sword, axe or mace (the guide names swords, the owner allows any two-hander that Titan's Grip takes: no polearm, no staff); a second one in the off-hand from level 60 (Titan's Grip) |
| Pet | none |
| Combat strategy | `custom::crusader` |
| Non-combat strategy | `custom::crusader-nc` |
| Rotation rows | `data/sql/playerbots/custom/area52_crusader.sql` |
| Stat weights | Strength 2.5, Hit 1.4, Crit 1.2, Expertise 1.2, Attack Power 1, Haste 1, weapon damage 5; no Armor Penetration (mostly Holy damage, which ignores armor) |

## What the enchants change

- **Consecrated Strikes** teaches **Templar's Slash** (278742 and ranks): a cone swing with 3 charges, 8 s recharge.
  Seal of Command deals Physical weapon damage, and Crusader Strike, Divine Storm and Hammer of the Righteous may
  create **Consecrated Ground** under the bot (Strength for nearby allies, a Holy-imbued weapon).
- **Exorcist's Slash** (Epic, Worldforged) adds **Exorcist's Slash** (986281, 6 s cooldown) beside Heroic Strike,
  which it needs.
- **Ward of Light** (Epic): Divine Storm damage; spell damage taken may reset Divine Storm.
- **Faithful Flex** (Epic): Physical and Holy damage raise each other.
- Rares: Vanquishing Storm 3, Seals of the Pure 3, Sanctity of Battle 3, Divine Strength, Crusade, Strength in Faith,
  Dual Wield Specialization.

## Build

80 levels of picks; the server's level cap is 60 for now, so the picks above it wait. Every pick and rank:
`crusader-build.tsv`. It passes the core's `Select` at every level 1-60 (59 of 60 ability points and 51 of 51
talent points spent at 60). Two different "Two-Handed Weapon Specialization" talents are taken: Paladin Retribution
at 38-40 and Warrior Arms at 68-70. Vengeance is the Paladin talent (3 ranks), Dual Wield Specialization the Rogue
one (5 ranks).

| Level | Picks |
|---|---|
| 1-10 | Holy Strike, Judgement of Wisdom, Judgement Mastery, Judgement of Light, Crusader Strike, Power Word: Shield, Divine Strength 1/3 |
| 11-16 | Divine Strength 3/3, Heroic Strike, Seals of the Pure 3/3, Divine Protection, Divine Storm, Twin Disciplines 1/5 |
| 17-22 | Improved Judgements 3/3, Flash of Light, Exorcism, Vindication 3/3, Seal of Command, Seal Mastery |
| 23-30 | Heart of the Crusader 2/3, Portal Mastery, Sanctity of Battle 3/3, Judgements of the Wise 1/3, Angelic Feather, Judgement of Justice, The Art of War 1/3, Blessing Mastery, Blessing of Wisdom |
| 31-37 | The Art of War 3/3, Judgements of the Wise 3/3, Heart of the Crusader 3/3, Divine Shield, Levitate, Vengeance 3/3, Rebuke |
| 38-46 | Two-Handed Weapon Specialization (Paladin) 3/3, Execution Sentence, Judgements of the Pure 3/3, Blinding Light, Hammer of Justice, Blessing of Kings, Strength of Arms 1/2, Body and Soul 2/2, Light of Dawn |
| 47-54 | Fanaticism 3/3, Purify, Swift Retribution 2/3, Rampage, Redemption, Power of Light 3/3, Banish |
| 55-60 | Righteous Vengeance 3/3, Tracking Mastery, Trueshot Aura, Swift Retribution 3/3, Strength of Arms 2/2, Titan's Grip, Dual Wield Specialization 1/5 |
| 61-70 | Dual Wield Specialization 5/5, Mage Armor Mastery, Molten Armor, Blessing of Might, Paladin Aura Mastery, Crusader Aura, Holy Power 3/3, Blessing of Sanctuary, Arcane Intellect, Two-Handed Weapon Specialization (Warrior) 3/3, Conjure Food, Avenging Wrath |
| 71-80 | Twin Disciplines 5/5, Consecration, Retribution Aura, Mental Quickness 5/5, Penance, Halo, Divine Spirit, Crusade 1/3, Concentration Aura |

## Mystic Enchants

All 17 slots from level 1, none with an investment requirement: 1 Legendary, 3 Epic (Epic + Legendary = 4, the
limit), 13 Rare. Slot list: `crusader-enchants.tsv`. Exorcist's Slash needs Heroic Strike (level 12) and Ward of
Light names Divine Storm (level 16); a slot the core refuses before then is asked for again.

## Rotation

The guide has no rotation. The owner took the proposed one for now (2026-10-27), to be updated later.

Combat, by priority:

1. Seal of Command, if missing.
2. Divine Shield at critical health; Divine Protection, then Power Word: Shield, at low health; Flash of Light on
   itself at critical health.
3. Rebuke on a cast, Hammer of Justice as the backup interrupt.
4. Purify on group members; Light of Dawn when 3 group members are below 60%.
5. With 2+ enemies close by: Divine Storm, then Templar's Slash.
6. Execution Sentence, Judgement of Light, Crusader Strike, Divine Storm, Exorcist's Slash, Templar's Slash,
   Exorcism, then Holy Strike and Heroic Strike as fillers.

Out of combat: Seal of Command; blessings by the owner's rule (below); Trueshot Aura; Purify.

Blessings (the owner's rule, 2026-10-27): **Might on itself, Kings on tanks and damage dealers, Wisdom on healers**.
One caster's blessings replace each other on a target, so the others get theirs through
`coa spread::<blessing>,others,<roles>`. Might comes at level 62, beyond the cap: until then the Crusader wears its
own Kings. Taking Might over its own Kings at 62 is not handled yet: the buff check sees the two as exclusive.

Not cast by the bot: Blinding Light (breaks on damage), Portal Mastery, Angelic Feather, Levitate, Tracking Mastery,
Banish, Redemption, and the picks above level 60.

## Titan's Grip

From level 60 the build holds a two-hander in each hand. Free Pick Heroes get Dual Wield from the core, and learning
Titan's Grip turns it on. The bots' gear pass already offers two-handers for the off-hand and lets the core decide;
`EnsureCoaLegendaryClass` fills an empty off-hand once the bot can grip (the best fitting two-hander from the bags,
or an off-hand gear pass for a random bot), as it does for the Hoplite's shield.
