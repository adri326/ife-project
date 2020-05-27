#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <stdbool.h>
#include "rules.h"

/** Placeholder function to determine which card the player wishes to play. Modifies `game`.
* @param game - The current game
* @returns true if the game should go on, false if the game should be halted
**/
bool players_turn(Game* game);

/** Modifies `game` as to make player no. `player_index` player their `card_index`-th card.
* @param game - The current game
* @param player_index - The current player's position
* @param card_index - The played card's index
**/
void play_card(Game* game, size_t player_index, size_t card_index);

#endif // GAME_H
