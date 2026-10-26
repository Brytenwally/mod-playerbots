-- Area 52 Freepick: Hoplite (Legendary Mystic Enchant Hoplite, 978217). Damage, close.
-- Build, enchants and the reasons for each line: docs/area52/classes/hoplite.md.
-- Inert until Heroes get CoaAiObjectContext and the Legendary class table picks custom::hoplite.
-- Spell names resolve against the bot's spellbook: Thrust, Shield Strike, Javelin and Flurry of Spears are the names
-- the Legendary swaps in for Sinister Strike, Shield Slam, Heroic Strike and Siegebreaker. They need Battle Stance,
-- a polearm and a shield.

DELETE FROM `playerbots_custom_strategy` WHERE `owner` = 0 AND `name` IN ('hoplite', 'hoplite-nc');

INSERT INTO `playerbots_custom_strategy` (`name`, `idx`, `owner`, `action_line`) VALUES
('hoplite', 1, 0, 'buff missing::Battle Stance>cast buff::Battle Stance!98'),
('hoplite', 2, 0, 'coa no imbue>cast imbue::Windfury Weapon!97'),
('hoplite', 3, 0, 'no pet>coa legendary pet!96'),
('hoplite', 4, 0, 'critical health>cast buff::Pain Suppression!95'),
('hoplite', 5, 0, 'coa enemy casting::Shield Bash>coa kick::Shield Bash!94'),
('hoplite', 6, 0, 'coa enemy casting::Kick,alone>coa kick::Kick!93'),
('hoplite', 7, 0, 'enemy out of melee>cast::Intercept!90'),
('hoplite', 8, 0, 'enemy out of melee>cast::Heroic Leap!89'),
('hoplite', 9, 0, 'coa tank aura below::Misdirection,1>cast on tank::Misdirection!88'),
('hoplite', 10, 0, 'generic boost>cast buff::Bloodlust!87'),
('hoplite', 11, 0, 'generic boost>cast buff::Death Wish!86'),
('hoplite', 12, 0, 'coa all::generic boost&target has aura::Impaled>cast buff::Hunger For Blood!85'),
('hoplite', 13, 0, 'generic boost>cast::Shattering Throw!84'),
('hoplite', 14, 0, 'medium aoe>cast::Bladestorm!83'),
('hoplite', 15, 0, 'buff missing::Bloodrage>cast buff::Bloodrage!82'),
('hoplite', 16, 0, 'buff missing::Rampage>cast buff::Rampage!81'),
('hoplite', 17, 0, 'can cast::Victory Rush>cast melee::Victory Rush!80'),
('hoplite', 18, 0, 'target critical health>cast melee::Execute!79'),
('hoplite', 19, 0, 'target aura expiring::Sunder Armor,4>cast melee::Devastate!78'),
('hoplite', 20, 0, 'light aoe>cast buff::Sweeping Strikes!77'),
('hoplite', 21, 0, 'coa all::combo points 5 available&light aoe>cast melee::Flurry of Spears!76'),
('hoplite', 22, 0, 'combo points 5 available>cast melee::Shield Strike!75'),
('hoplite', 23, 0, 'can cast::Colossus Smash>cast melee::Colossus Smash!70'),
('hoplite', 24, 0, 'debuff missing::Impaled>cast debuff::Javelin!69'),
('hoplite', 25, 0, 'can cast::Javelin>cast::Javelin!65'),
('hoplite', 26, 0, 'coa all::target stacks below max::Sunder Armor&last cast not::Devastate>cast melee::Devastate!61'),
('hoplite', 27, 0, 'can cast::Thrust>cast melee::Thrust!60'),
('hoplite', 28, 0, 'debuff missing::Demoralizing Shout>cast::Demoralizing Shout!20'),
('hoplite', 29, 0, 'coa summon missing::Windfury Totem>cast::Windfury Totem!19'),
('hoplite-nc', 1, 0, 'buff missing::Battle Stance>cast buff::Battle Stance!30'),
('hoplite-nc', 2, 0, 'coa no imbue>cast imbue::Windfury Weapon!29'),
('hoplite-nc', 3, 0, 'no pet>coa legendary pet!28'),
('hoplite-nc', 4, 0, 'buff missing::Blessing of Might>cast buff::Blessing of Might!27'),
('hoplite-nc', 5, 0, 'coa party aura spread::Blessing of Kings,40>coa spread::Blessing of Kings,others!26'),
('hoplite-nc', 6, 0, 'buff missing::Commanding Shout>cast buff::Commanding Shout!25'),
('hoplite-nc', 7, 0, 'buff missing::Trueshot Aura>cast buff::Trueshot Aura!24'),
('hoplite-nc', 8, 0, 'buff missing::Aspect of the Beast>cast buff::Aspect of the Beast!23');
