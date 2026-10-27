# mod-playerbots for Area 52 Freepick

This is the **`area52-freepick`** branch of [Brytenwally/mod-playerbots](https://github.com/Brytenwally/mod-playerbots).
It brings playerbots to the **Free Pick** realm of the Area 52 server. A Free Pick realm has no classes, only the
**Hero** (class 10). Each level a Hero gets ability points (AE) and talent points (TE) and spends them on spells and
talents from any class. Its role comes from its picks and its **Mystic Enchants**, above all its **Legendary Mystic
Enchant**.

These bots treat each Legendary Mystic Enchant as a class, a **Legendary class**: a build, 17 enchant slots, stat
weights, a role and a rotation. Everything that describes a class lives in the playerbots database, so a class can be
tuned, renamed or added with SQL and a server restart, no rebuild.

This branch builds on the Conquest of Azeroth work of the `coa` and `coa-bot-strategies` branches; see
**[README_COA.md](README_COA.md)** for the CoA bots in general. The original mod-playerbots README, with its
installation guides and credits, follows [below](#original-mod-playerbots-readme).

## The Legendary classes

| Class | `spec` | Legendary Mystic Enchant | Role | Weapons | Random bots |
|---|---|---|---|---|---|
| Mystic Knight | `mknight` | Eldritch Knight (81116) | Tank | one-handed sword and shield | 20% |
| Hoplite | `hoplite` | Hoplite (978217) | Melee damage | polearm and shield, thrown | 60% |
| Naturalist | `naturalist` | Seed of Life (965937) | Healer | staff | 20% |

Each class has a page with its build, enchants and rotation in [docs/area52/classes](docs/area52/classes).

## What the bots do

- **Random bots are Heroes.** With `AiPlayerbot.Area52HeroBots = 1`, the bot accounts are filled with Heroes. Each
  random Hero takes a Legendary class by the `chance` column, and always the same one for the same character.
- **They earn their spells.** A Hero bot spends its own AE and TE on its class's picks for its level, through the
  core's Free Pick rules. It never learns spells through GM commands, and its picks follow it level by level.
- **They wear their enchants.** Every bot account gets the whole Mystic Enchant collection. The bot slots the 17
  enchants of its class; its Legendary turns on once the slots are valid.
- **They gear for their class.** Autogear and the gear choices use the class's stat weights and weapon types. A
  Hoplite gets a shield and holds it beside its polearm once its Legendary is on.
- **They play their rotation.** Combat and out-of-combat rotations are custom strategies (`custom::<name>`): buffs
  that are not cast again while the same effect is up, taunts and aggro tools, cooldowns and pets.
- **They fill their role.** Tanks tank, healers heal, and the Dungeon Finder queues each bot by its class's role.
- **Class changes are clean.** Changing class drops the old picks, talents and enchants before the new ones go on.

## Commands

| Command | What it does |
|---|---|
| `.playerbot bot addclass <class> [male\|female]` | Logs in a Hero from the addclass accounts with that Legendary class, for example `addclass mknight` |
| `talents spec list` | Explains the categories |
| `talents spec list tank\|healer\|mdps\|rdps` | Lists the Legendary classes of a category |
| `talents spec <class>` | Switches the bot to that Legendary class |

A class is named by its `spec` or its `name`, ignoring case, spaces and punctuation: `mknight`, `MKnight` and
`mysticknight` are the same class. The category comes from the class's role and position: dps and close is `mdps`,
dps and ranged is `rdps`.

## Classes in the database

The playerbots database updater applies `data/sql/playerbots/custom/area52_*.sql` at startup and again whenever a
file changes. The worldserver reads the classes once, at startup.

| Table | Holds |
|---|---|
| `playerbots_legendary_class` | `spec`, `name`, Legendary spell, role, position, primary stat, rotations, chance, armor, shield, weapons, pet |
| `playerbots_legendary_pick` | The build: each spell or talent rank and the level it is taken at |
| `playerbots_legendary_enchant` | The 17 Mystic Enchant slots |
| `playerbots_legendary_weight` | Stat weights for gear |
| `playerbots_custom_strategy` | The rotations (`area52_<class>.sql`) |

To tune a class, change its rows and restart the worldserver. To add one, add its rows to all five tables. In this
repository the class data comes from the TSV files in [docs/area52/classes](docs/area52/classes);
`apps/area52/generate_legendary_class_data.py` turns them into `area52_legendary_classes.sql`.

## Building

The branch builds as an ordinary playerbots module inside the core it was made for,
[Brytenwally/azerothcore-wotlk-coa](https://github.com/Brytenwally/azerothcore-wotlk-coa) `main` (the Area 52 core with
Conquest of Azeroth updates). Clone it to `modules/mod-playerbots` and build the core. A Windows build with the server
SQL included is made by [Brytenwally/Freepick-build](https://github.com/Brytenwally/Freepick-build).

On a core without Free Pick the Legendary class code compiles out and the bots behave as on the `coa` branch.

## Credits

This branch is a small addition to the work of many people:

- **The mod-playerbots team and contributors**, [mod-playerbots/mod-playerbots](https://github.com/mod-playerbots/mod-playerbots),
  who wrote and maintain the playerbots module; see their README below and [AUTHORS.md](AUTHORS.md).
- **[@ZhengPeiRu21](https://github.com/ZhengPeiRu21)** and **[@celguar](https://github.com/celguar)**, whose
  playerbots mod-playerbots is based on, and **ike3**, author of the original
  [Playerbot AI](https://github.com/ike3/mangosbot).
- **[@Zyth45](https://github.com/Zyth45)**, whose [Conquest of Azeroth fork](https://github.com/Zyth45/mod-playerbots)
  of mod-playerbots this branch comes from.
- **[jealous-sound](https://github.com/jealous-sound/azerothcore-wotlk-coa)**, for the Conquest of Azeroth core.
- **[@CWO4PapaBear](https://github.com/CWO4PapaBear)**, for the Area 52 core this branch plays on.
- **Veritest** and **Literally Elbowttv**, whose Area 52 player guides (Mystic Knight and Hoplite; Naturalist) the
  bot builds follow.
- **[AzerothCore](https://www.azerothcore.org/)**.

---

## Original mod-playerbots README

<p align="center">
    <a href="https://github.com/mod-playerbots/mod-playerbots/blob/master/README.md">English</a>
    |
    <a href="https://github.com/mod-playerbots/mod-playerbots/blob/master/README_CN.md">中文</a>
    |
    <a href="https://github.com/mod-playerbots/mod-playerbots/blob/master/README_ES.md">Español</a>
</p>


<div align="center">
  <img src="banner.png" alt="Playerbots Banner" width="700px">
</div>

<div align="center">
    <img src="https://github.com/mod-playerbots/mod-playerbots/actions/workflows/macos_build.yml/badge.svg">
    <img src="https://github.com/mod-playerbots/mod-playerbots/actions/workflows/core_build.yml/badge.svg">
    <img src="https://github.com/mod-playerbots/mod-playerbots/actions/workflows/windows_build.yml/badge.svg">
</div>

# Playerbots Module
`mod-playerbots` is an [AzerothCore](https://www.azerothcore.org/) module that adds player-like bots to a server. The project is based off [IKE3's Playerbots](https://github.com/ike3/mangosbot).

Features include:

- The ability to log in alt characters as bots, allowing players to interact with their other characters, form parties, level up, and more
- Random bots that wander through the world, complete quests, and otherwise behave like players, simulating the MMO experience
- Bots capable of running most raids and battlegrounds
- Highly configurable settings to define how bots behave
- Excellent performance, even when running thousands of bots

We also have a **[Discord server](https://discord.gg/NQm5QShwf9)** where you can discuss the project, ask questions, and get involved in the community!

## Installation

Supported platforms are Ubuntu, Windows, and macOS. Other Linux distributions may work, but may not receive support.

> **Important:** All `mod-playerbots` installations require a custom fork of AzerothCore: [mod-playerbots/azerothcore-wotlk (Playerbot branch)](https://github.com/mod-playerbots/azerothcore-wotlk/tree/Playerbot). The standard AzerothCore repository will **not** work.

### Quick Start

```bash
git clone https://github.com/mod-playerbots/azerothcore-wotlk.git --branch=Playerbot
cd azerothcore-wotlk/modules
git clone https://github.com/mod-playerbots/mod-playerbots.git --branch=master
```

Then build the server following the platform-specific instructions in our **[Installation Guide](https://github.com/mod-playerbots/mod-playerbots/wiki/Installation-Guide)**.

> **Testing branch:** A `test-staging` branch is available with the latest features and fixes before they are merged into `master`. To use it, clone with `--branch=test-staging` instead. Note that this branch may contain unstable or breaking changes — use it at your own risk and only if you are comfortable troubleshooting issues.

### Detailed Guides

| Guide | Description |
|---|---|
| **[Installation Guide](https://github.com/mod-playerbots/mod-playerbots/wiki/Installation-Guide)** | Full step-by-step instructions for clean installs, migrating from existing AzerothCore, Docker setup, adding modules, and updating |
| **[Troubleshooting](https://github.com/mod-playerbots/mod-playerbots/wiki/Troubleshooting)** | Solutions to the most common build errors, database issues, configuration mistakes, crashes, and platform-specific problems |

For additional references, see the [AzerothCore Installation Guide](https://www.azerothcore.org/wiki/installation) and [Installing a Module](https://www.azerothcore.org/wiki/installing-a-module) pages.

## Documentation

The [Playerbots Wiki](https://github.com/mod-playerbots/mod-playerbots/wiki) contains an extensive overview of AddOns, commands, raids with programmed bot strategies, and recommended performance configurations. Please note that documentation may be incomplete or out-of-date in some sections, and contributions are welcome.

Bots are controlled via chat commands. For larger bot groups, this can be cumbersome. Because of this, community members have developed client AddOns to allow controlling bots through the in-game UI. We recommend you check out their projects listed in the [AddOns and Submodules](https://github.com/mod-playerbots/mod-playerbots/wiki/Playerbot-Addons-and-Sub%E2%80%90Modules) page.

## Contributing

This project is still under development. We encourage anyone to make contributions, anything from pull requests to reporting issues. If you encounter any errors or experience crashes, we encourage you [report them as GitHub issues](https://github.com/mod-playerbots/mod-playerbots/issues/new?template=bug_report.md). Your valuable feedback will help us improve this project collaboratively.

If you make coding contributions, `mod-playerbots` complies with the [C++ Code Standards](https://www.azerothcore.org/wiki/cpp-code-standards) established by AzerothCore. Each Pull Request must include all test scenarios the author performed, along with their results, to demonstrate that the changes were properly verified.

We recommend joining the [Discord server](https://discord.gg/NQm5QShwf9) to make your contributions to the project easier, as a lot of active support is carried out through this server.

Please click on the "⭐" button to stay up to date and help us gain more visibility on GitHub!

## Acknowledgements

`mod-playerbots` is based on [ZhengPeiRu21/mod-playerbots](https://github.com/ZhengPeiRu21/mod-playerbots) and [celguar/mangosbot-bots](https://github.com/celguar/mangosbot-bots). We extend our gratitude to [@ZhengPeiRu21](https://github.com/ZhengPeiRu21) and [@celguar](https://github.com/celguar) for their continued efforts in maintaining the module.

Also, a thank you to the many contributors who've helped build this project:

<a href="https://github.com/mod-playerbots/mod-playerbots/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=mod-playerbots/mod-playerbots" />
</a>
