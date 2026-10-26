-- Area 52 Freepick: Naturalist (Legendary Mystic Enchant Seed of Life, 965937). Healer, ranged.
-- Build, enchants and the reasons for each line: docs/area52/classes/naturalist.md.
-- Inert until Heroes get CoaAiObjectContext and the Legendary class table picks custom::naturalist.
-- Seed of Life grants Bloom; Druid of Rot swaps Force of Nature for Force of Nature (Call Fungarians).

DELETE FROM `playerbots_custom_strategy` WHERE `owner` = 0 AND `name` IN ('naturalist', 'naturalist-nc');

INSERT INTO `playerbots_custom_strategy` (`name`, `idx`, `owner`, `action_line`) VALUES
('naturalist', 1, 0, 'coa tank aura below::Lifebloom,1>cast on tank::Lifebloom!96'),
('naturalist', 2, 0, 'party member critical health>cast heal party::Swiftmend!95'),
('naturalist', 3, 0, 'party member critical health>cast heal party::Lesser Healing Wave!94'),
('naturalist', 4, 0, 'combat party member dead>cast rez::Rebirth!93'),
('naturalist', 5, 0, 'coa enemy casting::Counterspell,1000>coa kick::Counterspell!92'),
('naturalist', 6, 0, 'coa all::coa party aura count::Seed of Life,3&coa party hurt::2,85>cast on aura::Bloom,Seed of Life!91'),
('naturalist', 7, 0, 'coa party hurt::3,75>cast heal aoe::Wild Growth!90'),
('naturalist', 8, 0, 'coa party hurt::3,75>cast heal tank::Efflorescence!89'),
('naturalist', 9, 0, 'coa tank aura below::Earth Shield,1>cast on tank::Earth Shield!88'),
('naturalist', 10, 0, 'coa tank health::1,60>cast on tank::Flourish!87'),
('naturalist', 11, 0, 'cure party::Cleanse>cast cure party::Cleanse!86'),
('naturalist', 12, 0, 'cure party::Abolish Curse>cast cure party::Abolish Curse!85'),
('naturalist', 13, 0, 'being attacked>cast buff::Fade!84'),
('naturalist', 14, 0, 'low mana>cast buff::Innervate!83'),
('naturalist', 15, 0, 'coa summon missing::Mana Spring Totem>cast::Mana Spring Totem!82'),
('naturalist', 16, 0, 'coa summon missing::Totem of Wrath>cast::Totem of Wrath!81'),
('naturalist', 17, 0, 'coa party aura spread::Rejuvenation,8>coa spread::Rejuvenation!70'),
('naturalist', 18, 0, 'party member medium health>cast heal party::Regrowth!65'),
('naturalist', 19, 0, 'coa all::high mana&coa tank health::40,75&coa tank aura below::Lifebloom,3>cast on tank::Lifebloom!60'),
('naturalist', 20, 0, 'party member almost full health>cast heal party::Nourish!55'),
('naturalist', 21, 0, 'coa none::coa party hurt::2,75&coa party hurt::1,50>cast::Force of Nature (Call Fungarians)!22'),
('naturalist', 22, 0, 'coa none::coa party hurt::2,75&coa party hurt::1,50>cast debuff::Insect Swarm!21'),
('naturalist', 23, 0, 'coa none::coa party hurt::2,75&coa party hurt::1,50>cast::Wrath!20'),
('naturalist-nc', 1, 0, 'coa no imbue>cast imbue::Earthliving Weapon!30'),
('naturalist-nc', 2, 0, 'buff missing::Mage Armor>cast buff::Mage Armor!29'),
('naturalist-nc', 3, 0, 'buff missing::Concentration Aura>cast buff::Concentration Aura!28'),
('naturalist-nc', 4, 0, 'buff missing::Mark of the Wild>cast buff party::Mark of the Wild!27'),
('naturalist-nc', 5, 0, 'buff missing::Blessing of Kings>cast buff party::Blessing of Kings!26'),
('naturalist-nc', 6, 0, 'cure party::Cleanse>cast cure party::Cleanse!25'),
('naturalist-nc', 7, 0, 'cure party::Abolish Curse>cast cure party::Abolish Curse!24');
