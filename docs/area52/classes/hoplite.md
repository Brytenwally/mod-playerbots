# Hoplite

A Legendary "class": a Hero built around the Legendary Mystic Enchant **Hoplite** (978217).
Source: the owner's screenshots of the player guide "Hoplite" by Veritest (Archetypes window, build only, no rotation
page, read 2026-10-27), with the owner's answers of the same day. Resolved against the Freepick DBCs and the core at
`037025155` (`src/server/coa/AscensionHoplite.cpp`).

| | |
|---|---|
| Legendary | Hoplite, 978217 |
| Role / position | damage / close |
| Primary stat | Strength |
| Equipment | Mail, polearm, **shield** (the core refuses Hoplite abilities without one), thrown weapon |
| Pet | Winter Wolf (1131), a Ferocity wolf, given with the Legendary: no taming |
| Combat strategy | `custom::hoplite` |
| Non-combat strategy | `custom::hoplite-nc` |
| Rotation rows | `data/sql/playerbots/custom/area52_hoplite.sql` |

## What the enchants change

- **Hoplite** works only in **Battle Stance** (which grants Phalanx Stance, 997743) with a polearm in the main hand
  and a shield in the off hand. It swaps in, all paid in rage:
  - Sinister Strike → **Thrust** (978569): combo point builder. Under Bloodrage each Thrust adds a Phalanx Strike
    stack; recasting Bloodrage resets the stacks.
  - Shield Slam → **Shield Strike** (978533): finisher; each combo point takes 2 s off Bloodrage's cooldown.
  - Heroic Strike → **Javelin** (982250): ranged hit that leaves **Impaled**, a bleed. Colossus Smash resets
    Javelin's cooldown on an Impaled target.
  - Siegebreaker → **Flurry of Spears** (901200): finisher in a cone in front.
- **Sweeping Swings**: Sweeping Strikes also hits the main target; melee hits shorten its cooldown.
- **Wind Slam**: more Windfury procs, which empower the next Shield Slam. Needs Windfury Weapon on the polearm.
- **Spartan Kick**: Kick knocks back and has its own 20 s cooldown.
- Draconic Sinister Strike and Aggression name Sinister Strike, which Thrust replaces; whether they follow Thrust is
  not checked.

## Build

70 ability points and 61 talent points by level 70; the server's level cap is 60 for now, so the picks above it
wait. Every pick and rank: `hoplite-build.tsv`. Two different "Two-Handed Weapon Specialization" talents are taken:
Warrior Arms at 26-28 and Paladin Retribution at 39-41.

| Level | Picks |
|---|---|
| 1-4 | Sinister Strike, Stance Mastery, Battle Stance, Victory Rush, Blessing Mastery, Blessing of Might |
| 10-14 | Bloodrage, Remorseless Attacks 2/2, Heroic Strike, Armored to the Teeth 3/3, Devastate |
| 15-20 | Relentless Strikes 3/3, Kick, Shield Bash, Blessing of Kings, Anger Management 3/3 |
| 21-28 | Impale 2/2, Deep Wounds 3/3, Tame Beast Abilities, Two-Handed Weapon Specialization (Warrior) 3/3 |
| 29-33 | Enrage 5/5, Windfury Weapon, Weapon Enhancement Mastery, Intercept, Sweeping Strikes, Windfury Totem, Air Totem Mastery |
| 34-41 | Death Wish, Dance of Death 3/3, Colossus Smash, Poleaxe Specialization 2/5, Two-Handed Weapon Specialization (Paladin) 3/3, Siegebreaker, Heroic Leap |
| 42-49 | Hunter vs. Wild 3/3, Shield Slam, Trueshot Aura, Elemental Weapons 3/3, Hunter Aspect Mastery, Aspect of the Beast, Blade Twisting 2/2 |
| 50-58 | Rampage, Honor Among Thieves 3/3, Pain Suppression, Shredding Blows 3/3, Water Walking, Far Sight, Poleaxe Specialization 5/5 |
| 59-64 | Deadliness 3/3, Bladestorm, Shattering Throw, Hunger For Blood, Unending Fury 3/3, Wrath of Air Totem |
| 65-70 | Gag Order 3/3, Shout Mastery, Demoralizing Shout, Water Breathing, Execute, Commanding Shout, Seal Fate 3/3, Misdirection, Bloodlust |

## Mystic Enchants

All 17 slots from level 1, none with an investment requirement: 1 Legendary, 3 Epic, 13 Rare (3 of them the
Worldforged Draconic Sinister Strike, the Worldforged limit). Slot list: `hoplite-enchants.tsv`.

## Rotation

The guide has no rotation page; this one is built from the spells and the owner's answers (2026-10-27).

Owner's rules:

- Shield equipped; a Ferocity pet of any family.
- **Devastate** does little damage but applies Sunder Armor. Below 5 stacks it takes turns with Thrust (`last cast
  not::Devastate`), under the combo point spenders. At 5 stacks it is cast again only when Sunder Armor is about to
  run out (under 4 seconds), above the spenders so the stacks are not lost.
- Blessing of Might on itself, Blessing of Kings on the others (`coa spread::Blessing of Kings,others`).
- Windfury Totem, not Wrath of Air Totem (both are Air totems).
- **Kick** (Spartan Kick knocks back) only when no other enemy stands within 10 yards of the caster; Shield Bash
  first.
- Combo points always spent at 5.

- **Pet**: no taming. The moment Hoplite is active on the bot (its aura is on), the bot learns Call Pet, Dismiss Pet
  and Revive Pet and gets a Winter Wolf as its pet (`src/Ai/Coa/CoaLegendaryPets.cpp`, owner's rule 27/10). The
  bot factory does it when it sets the bot up; the `coa legendary pet` line creates it if it is still missing,
  calls it back after that, or revives it.

Order, highest first:

1. Battle Stance, Windfury Weapon on the polearm, the pet, Pain Suppression below 25% health.
2. Interrupts: Shield Bash, then Kick on a lone caster.
3. Intercept, then Heroic Leap, to reach a target out of melee range; Misdirection on the tank.
4. Hard fights (`generic boost`): Bloodlust, Death Wish, Hunger For Blood once the target bleeds (Impaled), Shattering
   Throw. Bladestorm with 3+ enemies close by.
5. Bloodrage only when not active, Rampage, Victory Rush, Execute.
6. Sunder Armor refresh at 5 stacks; Sweeping Strikes with 2+ enemies.
7. Spenders at 5 combo points: Flurry of Spears with 2+ enemies, otherwise Shield Strike.
8. Colossus Smash, Javelin to keep Impaled, Javelin again after a Colossus Smash reset.
9. Devastate and Thrust taking turns while Sunder builds, then Thrust.
10. Demoralizing Shout, Windfury Totem.

Out of combat: Battle Stance, Windfury Weapon, pet, Blessing of Might, Kings on the group, Commanding Shout, Trueshot
Aura, Aspect of the Beast.

## Open for implementation

- **Heroic Leap** lands at a destination; check in game that `cast::` throws it at the target.
- **Gear**: a polearm and a shield together, which no vanilla class wears; the gear step of the bot setup has to
  allow it.
- Hero support itself: see `CLAUDE.md`, section 5.

Not tested in game.
