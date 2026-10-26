-- Area 52 Freepick: Mystic Knight (Legendary Mystic Enchant Eldritch Knight, 81116). Tank, close.
-- Build, enchants and the reasons for each line: docs/area52/classes/mystic-knight.md.
-- Inert until Heroes get CoaAiObjectContext and the Legendary class table picks custom::mystic-knight.
-- Spell names resolve against the bot's spellbook: Consecration (Emanate), Deconstruct and Eldritch Wrath are the
-- names the enchants swap in, so Consecration, Devastate and Holy Wrath are never cast.

DELETE FROM `playerbots_custom_strategy` WHERE `owner` = 0 AND `name` IN ('mystic-knight', 'mystic-knight-nc');

INSERT INTO `playerbots_custom_strategy` (`name`, `idx`, `owner`, `action_line`) VALUES
('mystic-knight', 1, 0, 'coa all::coa channeling::Invocation&coa aggro lost>cancel channel!99'),
('mystic-knight', 2, 0, 'buff missing::Mana-forged Barrier>cast buff::Mana-forged Barrier!98'),
('mystic-knight', 3, 0, 'buff missing::Seal of Wisdom>cast buff::Seal of Wisdom!97'),
('mystic-knight', 4, 0, 'buff missing::Eldritch Weapon>cast buff::Eldritch Effusion!96'),
('mystic-knight', 5, 0, 'buff missing::Devotion Aura>cast buff::Devotion Aura!95'),
('mystic-knight', 6, 0, 'critical health>cast buff::Pain Suppression!94'),
('mystic-knight', 7, 0, 'critical health>cast buff::Dispersion!93'),
('mystic-knight', 8, 0, 'low health>cast buff::Evasion!92'),
('mystic-knight', 9, 0, 'party member critical health>cast heal party::Pain Suppression!91'),
('mystic-knight', 10, 0, 'lose aggro>cast::Flesh Hook!90'),
('mystic-knight', 11, 0, 'lose aggro>cast::Challenging Shout!89'),
('mystic-knight', 12, 0, 'enemy out of melee>cast::Intercept!88'),
('mystic-knight', 13, 0, 'enemy out of melee>cast::Charge!87'),
('mystic-knight', 14, 0, 'cure party::Cleanse>cast cure party::Cleanse!86'),
('mystic-knight', 15, 0, 'medium aoe>cast::Capacitor Totem!85'),
('mystic-knight', 16, 0, 'coa party low mana::2,30>cast::Brilliance Aura!84'),
('mystic-knight', 17, 0, 'can cast::Judgement of Wisdom>cast::Judgement of Wisdom!60'),
('mystic-knight', 18, 0, 'can cast::Consecration (Emanate)>cast::Consecration (Emanate)!59'),
('mystic-knight', 19, 0, 'can cast::Eldritch Wrath>cast::Eldritch Wrath!58'),
('mystic-knight', 20, 0, 'coa all::low mana&coa steady aggro>cast::Invocation!61'),
('mystic-knight', 21, 0, 'can cast::Deconstruct>cast melee::Deconstruct!56'),
('mystic-knight', 22, 0, 'light aoe>cast::Arcane Orb!55'),
('mystic-knight', 23, 0, 'light aoe>cast::Arcane Explosion!54'),
('mystic-knight', 24, 0, 'can cast::Arcane Barrage>cast::Arcane Barrage!53'),
('mystic-knight', 25, 0, 'can cast::Sinister Strike>cast melee::Sinister Strike!52'),
('mystic-knight-nc', 1, 0, 'buff missing::Mana-forged Barrier>cast buff::Mana-forged Barrier!30'),
('mystic-knight-nc', 2, 0, 'buff missing::Mage Armor>cast buff::Mage Armor!29'),
('mystic-knight-nc', 3, 0, 'buff missing::Seal of Wisdom>cast buff::Seal of Wisdom!28'),
('mystic-knight-nc', 4, 0, 'buff missing::Eldritch Weapon>cast buff::Eldritch Effusion!27'),
('mystic-knight-nc', 5, 0, 'buff missing::Devotion Aura>cast buff::Devotion Aura!26'),
('mystic-knight-nc', 6, 0, 'buff missing::Blessing of Sanctuary>cast buff::Blessing of Sanctuary!25'),
('mystic-knight-nc', 7, 0, 'coa summon missing>cast::Summon Felguard!24'),
('mystic-knight-nc', 8, 0, 'coa summon missing>cast::Summon Voidwalker!23'),
('mystic-knight-nc', 9, 0, 'buff missing::Soul Link>cast buff::Soul Link!22'),
('mystic-knight-nc', 10, 0, 'buff missing::Arcane Intellect>cast buff party::Arcane Intellect!21'),
('mystic-knight-nc', 11, 0, 'buff missing::Power Word: Fortitude>cast buff party::Power Word: Fortitude!20'),
('mystic-knight-nc', 12, 0, 'buff missing::Mark of the Wild>cast buff party::Mark of the Wild!19'),
('mystic-knight-nc', 13, 0, 'cure party::Cleanse>cast cure party::Cleanse!18');
