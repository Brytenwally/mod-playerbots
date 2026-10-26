#!/usr/bin/env python3
"""Writes data/sql/playerbots/custom/area52_legendary_classes.sql from docs/area52/classes.

classes.tsv holds one row per Legendary class; <file>-build.tsv its picks and <file>-enchants.tsv its 17 enchant
slots. The playerbots database updater applies the SQL at startup, and again whenever the file changes.
"""
import csv
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]
CLASSES = pathlib.Path(ROOT, "docs/area52/classes")
OUTPUT = pathlib.Path(ROOT, "data/sql/playerbots/custom/area52_legendary_classes.sql")

ROLES = {"tank", "heal", "dps"}
POSITIONS = {"close", "ranged"}
ARMOR = {"cloth", "leather", "mail", "plate"}
WEAPONS = {"axe", "axe2", "bow", "gun", "mace", "mace2", "polearm", "sword", "sword2", "staff", "fist", "dagger",
           "thrown", "crossbow", "wand"}
STATS = {"agility", "strength", "intellect", "spirit", "stamina", "hit", "crit", "haste", "armor", "defense", "dodge",
         "parry", "block_value", "block_rating", "resilience", "health_regeneration", "spell_power",
         "spell_penetration", "heal_power", "mana_regeneration", "attack_power", "armor_penetration", "expertise",
         "melee_dps", "ranged_dps"}

TABLES = """CREATE TABLE IF NOT EXISTS `playerbots_legendary_class` (
  `spec` VARCHAR(32) NOT NULL COMMENT 'talents spec <spec>',
  `name` VARCHAR(64) NOT NULL,
  `legendary` INT UNSIGNED NOT NULL COMMENT 'Legendary Mystic Enchant spell, also its aura',
  `role` VARCHAR(8) NOT NULL COMMENT 'tank, heal or dps',
  `position` VARCHAR(8) NOT NULL COMMENT 'close or ranged',
  `primary_stat` TINYINT UNSIGNED NOT NULL COMMENT '0 Strength, 1 Agility, 3 Intellect, 4 Spirit, 6 Stamina',
  `combat` VARCHAR(64) NOT NULL COMMENT 'combat rotation, custom::<name> in playerbots_custom_strategy',
  `noncombat` VARCHAR(64) NOT NULL DEFAULT '' COMMENT 'non-combat rotation',
  `chance` TINYINT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'percent of the random bots playing it',
  `armor` VARCHAR(8) NOT NULL COMMENT 'heaviest armor: cloth, leather, mail or plate',
  `shield` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `weapons` VARCHAR(128) NOT NULL COMMENT 'comma separated: axe, axe2, bow, gun, mace, mace2, polearm, sword, sword2, staff, fist, dagger, thrown, crossbow, wand',
  `pet` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'creature entry of the pet the Legendary gives, 0 for none',
  PRIMARY KEY (`spec`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `playerbots_legendary_pick` (
  `spec` VARCHAR(32) NOT NULL,
  `spell` INT UNSIGNED NOT NULL COMMENT 'spell of the Free Pick entry, the rank picked',
  `level` TINYINT UNSIGNED NOT NULL COMMENT 'character level the pick is taken at',
  PRIMARY KEY (`spec`, `spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `playerbots_legendary_enchant` (
  `spec` VARCHAR(32) NOT NULL,
  `slot` TINYINT UNSIGNED NOT NULL COMMENT '1 to 17',
  `spell` INT UNSIGNED NOT NULL COMMENT 'Mystic Enchant spell, 0 for an empty slot',
  PRIMARY KEY (`spec`, `slot`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `playerbots_legendary_weight` (
  `spec` VARCHAR(32) NOT NULL,
  `stat` VARCHAR(32) NOT NULL COMMENT 'agility, strength, intellect, spirit, stamina, hit, crit, haste, armor, defense, dodge, parry, block_value, block_rating, resilience, health_regeneration, spell_power, spell_penetration, heal_power, mana_regeneration, attack_power, armor_penetration, expertise, melee_dps, ranged_dps',
  `weight` FLOAT NOT NULL,
  PRIMARY KEY (`spec`, `stat`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;"""


def rows(path):
    with open(path, newline="", encoding="utf-8") as handle:
        return list(csv.DictReader(handle, delimiter="\t"))


def text(value):
    return "'" + value.replace("\\", "\\\\").replace("'", "''") + "'"


def check(spec, field, value, allowed):
    if value not in allowed:
        raise SystemExit(f"{spec}: {field} '{value}' is not one of {sorted(allowed)}")


def main():
    classes = rows(CLASSES / "classes.tsv")
    specs = ", ".join(text(c["spec"]) for c in classes)
    class_rows, pick_rows, enchant_rows, weight_rows = [], [], [], []
    for c in classes:
        spec = c["spec"]
        check(spec, "role", c["role"], ROLES)
        check(spec, "position", c["position"], POSITIONS)
        check(spec, "armor", c["armor"], ARMOR)
        for weapon in c["weapons"].split(","):
            check(spec, "weapon", weapon, WEAPONS)
        class_rows.append("(" + ", ".join([
            text(spec), text(c["name"]), c["legendary"], text(c["role"]), text(c["position"]), c["primary_stat"],
            text(c["combat"]), text(c["noncombat"]), c["chance"], text(c["armor"]), c["shield"], text(c["weapons"]),
            c["pet"]]) + ")")

        seen = set()
        for pick in rows(CLASSES / f"{c['file']}-build.tsv"):
            if pick["spell"] in seen:
                raise SystemExit(f"{spec}: spell {pick['spell']} picked twice")
            seen.add(pick["spell"])
            pick_rows.append(f"({text(spec)}, {pick['spell']}, {pick['level']})")

        enchants = rows(CLASSES / f"{c['file']}-enchants.tsv")
        if len(enchants) != 17:
            raise SystemExit(f"{spec}: {len(enchants)} enchant slots, not 17")
        enchant_rows += [f"({text(spec)}, {slot}, {row['spell']})" for slot, row in enumerate(enchants, 1)]

        for pair in c["weights"].split(","):
            stat, weight = pair.split(":")
            check(spec, "stat", stat, STATS)
            weight_rows.append(f"({text(spec)}, {text(stat)}, {weight})")

    def block(table, columns, values):
        return (f"DELETE FROM `{table}` WHERE `spec` IN ({specs});\n"
                f"INSERT INTO `{table}` ({columns}) VALUES\n" + ",\n".join(values) + ";")

    out = [
        "-- GENERATED by apps/area52/generate_legendary_class_data.py from docs/area52/classes.",
        "-- Area 52 Freepick Legendary classes for the bots: settings, picks by level, Mystic Enchant slots and stat",
        "-- weights. The worldserver reads them at startup, so a tweak made in the database needs a restart only.",
        "-- Rerunning this file resets the classes it lists; classes added only in the database are kept.",
        "",
        TABLES,
        "",
        block("playerbots_legendary_class", "`spec`, `name`, `legendary`, `role`, `position`, `primary_stat`, "
              "`combat`, `noncombat`, `chance`, `armor`, `shield`, `weapons`, `pet`", class_rows),
        "",
        block("playerbots_legendary_pick", "`spec`, `spell`, `level`", pick_rows),
        "",
        block("playerbots_legendary_enchant", "`spec`, `slot`, `spell`", enchant_rows),
        "",
        block("playerbots_legendary_weight", "`spec`, `stat`, `weight`", weight_rows),
    ]
    OUTPUT.write_text("\n".join(out) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
