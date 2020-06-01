#ifndef AI_H
#define AI_H

#include "rules.h"
#include <stdlib.h>

/** Entry point for the mid-game AI
* @param game - The current Game
* @param player - The AI's index
**/
void ai_turn(Game* game, size_t player);

#endif // AI_H
