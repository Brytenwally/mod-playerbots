-- Area 52 Freepick: Crusader (Legendary Mystic Enchant Consecrated Strikes, 965295). Damage, close.
-- Build, enchants and the reasons for each line: docs/area52/classes/crusader.md.
-- Templar's Slash comes with the Legendary and Exorcist's Slash with its enchant; both resolve by name against the
-- bot's spellbook. Blessing of Might on itself (from level 62), Kings on itself until then and on tanks and damage
-- dealers, Wisdom on healers.

DELETE FROM `playerbots_custom_strategy` WHERE `owner` = 0 AND `name` IN ('crusader', 'crusader-nc');

INSERT INTO `playerbots_custom_strategy` (`name`, `idx`, `owner`, `action_line`) VALUES
('crusader', 1, 0, 'buff missing::Seal of Command>cast buff::Seal of Command!98'),
('crusader', 2, 0, 'critical health>cast buff::Divine Shield!95'),
('crusader', 3, 0, 'low health>cast buff::Divine Protection!94'),
('crusader', 4, 0, 'low health>cast buff::Power Word: Shield!93'),
('crusader', 5, 0, 'critical health>cast heal::Flash of Light!92'),
('crusader', 6, 0, 'coa enemy casting::Rebuke>coa kick::Rebuke!90'),
('crusader', 7, 0, 'coa enemy casting::Hammer of Justice>coa kick::Hammer of Justice!89'),
('crusader', 8, 0, 'cure party::Purify>cast cure party::Purify!86'),
('crusader', 9, 0, 'coa party hurt::3,60>cast buff::Light of Dawn!85'),
('crusader', 10, 0, 'light aoe>cast melee::Divine Storm!70'),
('crusader', 11, 0, 'light aoe>cast melee::Templar''s Slash!69'),
('crusader', 12, 0, 'can cast::Execution Sentence>cast::Execution Sentence!65'),
('crusader', 13, 0, 'can cast::Judgement of Light>cast::Judgement of Light!64'),
('crusader', 14, 0, 'can cast::Crusader Strike>cast melee::Crusader Strike!63'),
('crusader', 15, 0, 'can cast::Divine Storm>cast melee::Divine Storm!62'),
('crusader', 16, 0, 'can cast::Exorcist''s Slash>cast melee::Exorcist''s Slash!61'),
('crusader', 17, 0, 'can cast::Templar''s Slash>cast melee::Templar''s Slash!60'),
('crusader', 18, 0, 'can cast::Exorcism>cast::Exorcism!58'),
('crusader', 19, 0, 'can cast::Holy Strike>cast melee::Holy Strike!56'),
('crusader', 20, 0, 'can cast::Heroic Strike>cast melee::Heroic Strike!55'),
('crusader-nc', 1, 0, 'buff missing::Seal of Command>cast buff::Seal of Command!30'),
('crusader-nc', 2, 0, 'buff missing::Blessing of Might>cast buff::Blessing of Might!29'),
('crusader-nc', 3, 0, 'coa all::buff missing::Blessing of Kings&coa none::has aura::Blessing of Might>cast buff::Blessing of Kings!28'),
('crusader-nc', 4, 0, 'coa party aura spread::Blessing of Kings,40>coa spread::Blessing of Kings,others,tank,dps!27'),
('crusader-nc', 5, 0, 'coa party aura spread::Blessing of Wisdom,40>coa spread::Blessing of Wisdom,others,heal!26'),
('crusader-nc', 6, 0, 'buff missing::Trueshot Aura>cast buff::Trueshot Aura!25'),
('crusader-nc', 7, 0, 'cure party::Purify>cast cure party::Purify!24');
