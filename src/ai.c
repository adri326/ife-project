#include "ai.h"
#include "game.h"

void ai_turn(Game* game, size_t player) {
  play_card(game, player, 0);
}
