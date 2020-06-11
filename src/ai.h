#ifndef AI_H
#define AI_H

#include "rules.h"
#include <stdlib.h>
#include <stdbool.h>

/** Entry point for the mid-game AI
* @param game - The current Game
* @param player - The AI's index
**/
void ai_turn(Game* game, size_t player);

/**
Check if the AI can beat the current leader.
*/
bool ai_win(Game* game, size_t player);

/**
Determine the lowest value card that the AI can play to win if possible and
the lowest value card otherwise.
*/
size_t ai_choose_card(Game* game, size_t player);

#endif // AI_H
