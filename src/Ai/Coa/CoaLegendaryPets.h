/*
 * Area 52 Freepick: pets that come with a Legendary Mystic Enchant (docs/area52/classes/).
 *
 * A Hoplite plays with a Ferocity pet. Instead of taming one, the bot gets a fixed pet and the spells to command it
 * the moment its Legendary is active (the owner's rule, 27/10): Call Pet, Dismiss Pet and Revive Pet, without Tame
 * Beast. The Legendary is read from its aura, which the core puts on the bot while the enchant is slotted.
 */
#ifndef PLAYERBOTS_COALEGENDARYPETS_H
#define PLAYERBOTS_COALEGENDARYPETS_H

#include "Define.h"

class Pet;
class Player;

// The creature the bot's Legendary gives as a pet, 0 when it gives none.
uint32 CoaLegendaryPetEntry(Player* bot);

// Teaches the pet commands and creates the Legendary's pet as the bot's current pet. Null when the Legendary gives
// none or the creature cannot be a pet.
Pet* CoaCreateLegendaryPet(Player* bot);

#endif
