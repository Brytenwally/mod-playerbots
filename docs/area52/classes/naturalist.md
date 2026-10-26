# Naturalist

A Legendary "class": a Hero built around the Legendary Mystic Enchant **Seed of Life** (965937).
Source: the owner's screenshots of the player guide "Naturalist" by Literally Elbowttv (Archetypes window, build only,
no rotation page, read 2026-10-27), with the owner's answers of the same day. Resolved against the Freepick DBCs.

| | |
|---|---|
| Legendary | Seed of Life, 965937 (spell grant 965937 → Bloom 965939 in `AscensionMysticSpellGrants.h`) |
| Role / position | healer / ranged |
| Primary stat | Spirit |
| Equipment | Cloth or leather, staff |
| Combat strategy | `custom::naturalist` |
| Non-combat strategy | `custom::naturalist-nc` |
| Rotation rows | `data/sql/playerbots/custom/area52_naturalist.sql` |

## What the enchants change

- **Seed of Life**: Rejuvenation and Regrowth heal over time can crit, and their ticks plant **Seed of Life**
  (944827, stacks 8 times) on the target. Teaches **Bloom** (965939, 20 s cooldown): heals an ally carrying Seed of
  Life, then jumps to 3 more carrying it, consuming their seeds.
- **Druid of Rot: Call Fungarians**: Force of Nature becomes Force of Nature (Call Fungarians), whose fungarians
  poison nearby enemies.
- **Everliving**: Nature heals build stacks that make the next Swiftmend stronger.
- **Evergreen**: Regrowth and Healing Touch refresh Lifebloom on the target.

## Build

80 ability points and 71 talent points by level 80; the server's level cap is 60 for now, so the picks above it
wait. Every pick and rank: `naturalist-build.tsv`.

| Level | Picks |
|---|---|
| 1-12 | Stealth, Wrath, Paladin Aura Mastery, Concentration Aura, Conjure Water, Rejuvenation, Genesis 3/5, Regrowth |
| 13-20 | Genesis 5/5, Moonglow 3/3, Counterspell, Divine Intellect 3/3, Lesser Healing Wave, Disengage, Rebirth |
| 21-29 | Meditation 3/3, Portal Mastery, Vanish, Improved Rejuvenation 3/3, Ancestral Spirit, Pick Pocket, Nature's Splendor 3/3, Mage Armor Mastery, Mage Armor |
| 30-37 | Gift of Nature 5/5, Weapon Enhancement Mastery, Earthliving Weapon, Insect Swarm, Levitate, Living Seed 3/3, Blink |
| 38-48 | Purification 5/5, Flourish, Living Spirit 3/3, Swiftmend, Innervate, Cleanse, Nature's Rejuvenation 3/3, Fade, Nature's Grasp |
| 49-57 | Holy Guidance 3/3, Force of Nature, Mental Strength 2/3, Earth Shield, Arcane Mind 1/3, Abolish Curse, Nature's Blessing 3/3 |
| 58-65 | Gift of the Earthmother 5/5, Wild Growth, Lifebloom, Efflorescence, Mark of the Wild, Mental Strength 3/3, Arcane Mind 3/3, Blessing Mastery, Blessing of Kings |
| 66-70 | Nature's Bounty 5/5, Tremor Totem, Invisibility, Astral Recall |
| 71-80 | Water Totem Mastery, Revitalize 3/3, Mana Spring Totem, Call of the Elements, Fire Totem Mastery, Heart of the Wild 3/3, Totem of Wrath, Windwalk Totem, Ancestral Knowledge 3/3, Totemic Recall, Nourish, Enlightenment 1/3 |

## Mystic Enchants

All 17 slots from level 1, none with an investment requirement: 1 Legendary, 3 Epic, 10 Rare, 3 Uncommon. Slot list:
`naturalist-enchants.tsv`. The guide's "Blessing of the..." is Blessing of the Eternals (951554), the only Rare of
that name.

## Rotation

The guide has no rotation page; this one is built from the spells and the owner's answers (2026-10-27).

Owner's rules:

- **Rejuvenation on 7 or 8 group members**, tanks and healers first, unless something more urgent is due
  (`coa party aura spread::Rejuvenation,8` with `coa spread::Rejuvenation`). It also plants the seeds Bloom needs.
- **Lifebloom**: 1 stack on the tank comes before everything. 2 and 3 stacks only with high mana, the tank between
  40% and 75% health, and nothing stronger to cast (the line sits under the heals).
- Totems: Mana Spring Totem and Totem of Wrath; Tremor Totem and Windwalk Totem are ignored for now.
- **Damage** only while the group is healthy: none of it once 2 members are below 75% health or 1 is below 50%
  (`coa none::coa party hurt::2,75&coa party hurt::1,50`). Solo, the bot itself is the group.
- **Counterspell** is the last interrupt: only after the cast has run 1 second, so the group's other kicks come
  first.
- Not cast: Stealth, Vanish, Pick Pocket, Invisibility, Blink, Disengage, Levitate, Portal Mastery, Conjure Water,
  Tremor Totem, Windwalk Totem, Astral Recall.

Order, highest first:

1. Lifebloom on the tank.
2. Below 25% health: Swiftmend, else Lesser Healing Wave. Rebirth on a group member who died in combat.
3. Counterspell, last.
4. Bloom on the most hurt ally with Seed of Life, once 3 carry it and 2 are below 85%.
5. 3 or more below 75%: Wild Growth, Efflorescence on the tank.
6. Earth Shield on the tank; Flourish on a tank below 60%.
7. Cleanse, Abolish Curse; Fade when attacked; Innervate below 15% mana; Mana Spring Totem, Totem of Wrath.
8. Rejuvenation spread, Regrowth on medium damage, Lifebloom stacks 2-3, Nourish as the filler.
9. While healthy: Force of Nature (Call Fungarians), Insect Swarm, Wrath.

Out of combat: Earthliving Weapon, Mage Armor, Concentration Aura, Mark of the Wild and Blessing of Kings on the
group, Cleanse and Abolish Curse.

## Open for implementation

- **Innervate** goes out at 15% mana (`AiPlayerbot.LowMana`); a higher threshold needs a self mana trigger.
- **Totems** are recognised by name (`coa summon missing::Mana Spring Totem`); check in game that the summoned
  creatures carry those names.
- Hero support itself: see `CLAUDE.md`, section 5.

Not tested in game.
