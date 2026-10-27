# Mystic Knight

The first Legendary "class": a Hero built around the Legendary Mystic Enchant **Eldritch Knight** (81116).
Source: the owner's screenshots of the player guide "Mystic Knight" by Veritest (Archetypes window, PvE build, read
2026-10-26), with the owner's answers of the same day. Resolved against the Freepick DBCs (`Spell.dbc` of 23 Oct,
`CharacterAdvancement.dbc` and `MysticEnchant.dbc` of 26 Oct) and the core at `037025155`.

| | |
|---|---|
| Legendary | Eldritch Knight, 81116 (core: `src/server/coa/AscensionEldritchKnight.cpp`) |
| Role / position | tank / close |
| Primary stat | Intellect |
| Equipment | Plate, shield, one-handed sword |
| Stat priority | Stamina > Block Value > Intellect > Strength |
| Combat strategy | `custom::mystic-knight` |
| Non-combat strategy | `custom::mystic-knight-nc` |
| Rotation rows | `data/sql/playerbots/custom/area52_mystic_knight.sql` |

## What the enchants change

- **Eldritch Knight** grants **Eldritch Effusion** (983702): restores 20% of missing mana and gives **Eldritch
  Weapon** (983701), a main-hand imbue. Other imbues (Windfury, Flametongue) overwrite it. Melee abilities under
  Eldritch Weapon give Eldritch Knight, then Anomaly stacks, then Eldritch Horror.
  Holy Wrath (2812 and ranks) becomes **Eldritch Wrath** (983703-983707), which uses Holy Wrath's modifiers.
- **Deconstruction** (954081) turns Devastate into **Deconstruct** (954082, 954443-954450). Devastate must be learned
  for the swap, so the build keeps it; the bot only ever casts Deconstruct. Deconstruct uses Devastate's modifiers.
- **Emanating Light** (926573) turns Consecration into **Consecration (Emanate)** (926574 and ranks), which follows
  the caster.
- **Flesh Hook** (92580) teaches **Flesh Hook** (92583): damage and pulls the target to the caster (not players).

All swaps go through `SetTemporarySpellReplacement` and a granted spell, so the bot's spellbook holds the new names
and rotation lines name those.

## Build

68 ability points and 61 talent points by level 70; the server's level cap is 60 for now, so the picks above it
wait. Every pick and rank with its spell id, tree and cost:
`mystic-knight-build.tsv`. Levels 2, 3, 5-7 and 9 take nothing.

| Level | Picks |
|---|---|
| 1 | Charge, Devastate |
| 4 | Mana-forged Barrier |
| 8 | Consecration |
| 10-12 | Crusader Strike, Benediction 3/3, Seal Mastery, Seal of Wisdom |
| 13-15 | Spellblade 3/3, Invocation |
| 16-18 | Power Surge 3/3, Arcane Explosion, Demon Mastery, Summon Voidwalker |
| 19-20 | Spiritual Attunement 2/2, Judgement Mastery, Judgement of Wisdom |
| 21-23 | Arcane Fortitude 3/3 |
| 24-26 | Soul Link, Heart of the Crusader 3/3, Brilliance Aura |
| 27-29 | Shield Specialization 3/3, Evasion |
| 30-32 | Improved Consecration 3/3, Shimmer |
| 33-37 | Blessing Mastery, Blessing of Sanctuary, Adaptive Defense 2/5, Conjure Water, Redoubt 3/3, Blessing of Kings |
| 38-40 | Adaptive Defense 5/5, Angelic Feather, Mage Armor, Mage Armor Mastery |
| 41-45 | Master Demonologist 5/5, Paladin Aura Mastery, Devotion Aura, Arcane Orb |
| 46-50 | Anticipation 5/5, Cleanse, Arcane Intellect, Holy Wrath |
| 51-53 | Firm Grip 3/3, Pain Suppression |
| 54-56 | Capacitor Totem, One-Handed Weapon Specialization 1/5, Enlightened Judgements 2/2, Summon Felguard |
| 57-60 | One-Handed Weapon Specialization 5/5, Power Word: Fortitude, Arcane Barrage, Dispersion |
| 61-65 | Balance of Power 2/2, Challenging Shout, Mark of the Wild, Ardent Defender 3/3, Ritual of Summoning |
| 66-70 | Demonic Embrace 3/3, Intercept, Improved Power Word: Fortitude 2/2, Astral Recall |

## Mystic Enchants

All 17 slots from level 1, none with an investment requirement: 1 Legendary, 3 Epic (Epic + Legendary = 4, the
limit), 9 Rare, 4 Uncommon. Slot list: `mystic-knight-enchants.tsv`. Flesh Hook is Worldforged; the Worldforged limit
counts Rares only. The guide left two slots empty; the owner chose Taunting Holy Wrath (90219) and Hasty Judgement of
Wisdom (99497) for them.

## Rotation

Guide, single target: Judgement of Wisdom > Consecration > Eldritch Wrath > Invocation > Deconstruct, Arcane Barrage
as filler; the owner's Invocation rule below replaces its place in that list. AoE: the same, Arcane Explosion as
filler instead of Arcane Barrage, and Deconstruct keeps Sunder on the important target.

Owner's rules (2026-10-26):

- Devotion Aura always on.
- Brilliance Aura (91234) is a cooldown: cast it when 2 or more party members within 35 yards, the tank included,
  are below 30% mana (`coa party low mana::2,30`).
- Deconstruct, never Devastate.
- Invocation (2026-10-27) is the main aggro tool: a 3 s channel that pulses damage and threat on every enemy within
  20 yards and makes the tank immune to interrupts and pushback. It goes out on cooldown whenever the target is in
  melee range, ahead of the damage rotation, and comes first of the aggro tools when an enemy turns to someone else
  (`coa aggro lost`). The channel is no longer broken off.
- Consecration ranks above Arcane Explosion (2026-10-27): Consecration (Emanate), and plain Consecration for a bot
  whose enchants are not on yet, both only with the target in melee range.
- Crusader Strike replaces Sinister Strike in the build and as the melee filler (2026-10-27).
- Capacitor Totem lands at the caster's feet and stuns enemies within 8 yards: cast it with 3+ enemies close by.

Defaults stated to the owner and not contradicted:

- Arcane Barrage is the single-target filler (from level 60); Crusader Strike is the melee filler below it, because
  melee abilities feed Eldritch Weapon.
- Pet: Voidwalker until 56, then Felguard (Felguard first in the list; without the spell the line cannot fire). The
  summons are `cast buff` lines: a plain `cast` aims at the current target, and out of combat there is none, so the
  bot never summoned.
- Blessing of Sanctuary on the tank only. Blessing of Kings is not cast: one blessing per caster and target, so a
  party-wide Kings would overwrite the tank's Sanctuary.
- Arcane Orb on cooldown with 2+ enemies close by.
- Self-defensives: Evasion below 45% health; Pain Suppression, then Dispersion, below 25%. Pain Suppression on a
  group member below 25%.
- Flesh Hook, then Challenging Shout, on a target that is not on the tank; Intercept, then Charge, to reach a target
  out of melee range. Cleanse on group members.
- Not cast by the bot: Shimmer, Angelic Feather, Conjure Water, Ritual of Summoning, Astral Recall. Ignored: spells
  named only in the guide's notes (Solar Beam, Guardian Spirit, Divine Sacrifice, Summon Felhunter, Dispel Curse).

## Open for implementation

- Hero support itself (context, kit, Legendary class table, build application and enchant slotting): see the plan in
  `CLAUDE.md`, section 5.

Not tested in game.
