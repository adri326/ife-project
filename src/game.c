#include "game.h"
#include "ai.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef TEST_ENV
#include "../test/test.h"

bool players_turn(Game* game) {
  Player* player = &game->players[0];
  for (size_t n = 0; n < player->n_cards; n++) {
    if (move_check(*game, player->cards[n], 0, &game->trick_cut, game->trick_leader_position)) {
      play_card(game, 0, n);
      break;
    }
    if (n == player->n_cards - 1) {
      ASSERT_INDENT();
      printf("Error: No card could be played!");
      return false;
    }
  }

  return true;
}

#else

bool players_turn(Game* game) {
  return false; // TODO
}

#endif

void play_card(Game* game, size_t player_index, size_t card_index) {
#ifdef TEST_ENV
  if (player_index >= 4) {
    ASSERT_INDENT();
    printf("Player index is out of bound (%zu)", player_index);
    return;
  }
#else
  if (player_index >= 4) return;
#endif

  Player* player = &game->players[player_index];

#ifdef TEST_ENV
  if (card_index >= player->n_cards) {
    ASSERT_INDENT();
    printf(
      "Card index is out of bound (%zu >= %" PRIu8 ")",
      card_index,
      player->n_cards);
    return;
  }
#else
  if (card_index >= player->n_cards) return;
#endif

  // Append the card to the current turn, if possible
  if (game->pli[player_index].type == VOIDCARD) {
    game->pli[player_index] = player->cards[card_index];
  }
#ifdef TEST_ENV
  else {
    ASSERT_INDENT();
    printf("No more space left for player %zu's card!", player_index);
  }
#else
  else return;
#endif

  // Shift the player's cards by one
  for (size_t n = card_index; n < player->n_cards - 1; n++) {
    player->cards[n] = player->cards[n + 1];
  }

  player->n_cards--;
}

bool game_turn(Game* game) {
  uint8_t played_mask = 0b0000;
  int current_player = game->trick_leader_position;
  if (current_player < 0 || current_player >= 4) current_player = 0; // default to 0

  // Empty `pli`
  for (size_t n = 0; n < 4; n++) {
    game->pli[n].type = VOIDCARD;
  }

  for (; played_mask < 0b1111; current_player = (current_player + 1) % 4) {
    played_mask |= 1 << current_player;
    if (current_player == 0) {
      if (!players_turn(game)) return false;
    } else {
      ai_turn(game, (size_t)current_player);
    }
  }

  update_scores(game);

  return true;
}

void update_scores(Game* game) {
  // TODO
}

bool play_all_turns(Game* game, size_t first_player_index) {
  game->trick_leader_position = first_player_index < 0 ? 0 : (first_player_index >= 4 ? 3 : first_player_index);

  for (size_t n = 0; n < 8; n++) {
    if (!game_turn(game)) return false;
  }

  return true;
}
