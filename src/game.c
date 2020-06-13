#include "game.h"
#include "ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef TEST_ENV
#include "display.h"
#include <SDL2/SDL.h>
#endif

#ifdef TEST_ENV
#include "../test/test.h"

bool players_turn(Game* game) {
  Player* player = &game->players[0];
  for (size_t n = 0; n < player->n_cards; n++) {
    if (move_check(game, player->cards[n], 0)) {
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

extern SDL_Renderer* renderer;
extern uint32_t window_width;
extern uint32_t window_height;
extern uint32_t zoom_factor;

bool players_turn(Game* game) {
  int32_t mouse_x = 0;
  int32_t mouse_y = 0;
  bool exit = false;

  uint32_t deck_x = window_width / 2;
  uint32_t deck_y = window_height - (CARD_HEIGHT + DECK_PADDING) * 4;

  int hovered_card = -1;

  // Display loop
  while (!exit) {
    SDL_Event event;
    SLEEP(50);

    // Poll events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYUP) { // Key pressed
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            exit = true;
            break;
        }
      } else if (event.type == SDL_QUIT) { // Window closed
        exit = true;
      } else if (event.type == SDL_MOUSEMOTION) { // Mouse moved: update hovered_card
        mouse_x = event.motion.x;
        mouse_y = event.motion.y;
        hovered_card = get_hovered_card(&game->players[0], deck_x, deck_y, mouse_x, mouse_y);
        if (hovered_card != -1 && !move_check(game, game->players[0].cards[hovered_card], 0)) {
          hovered_card = -1;
        }
      } else if (event.type == SDL_MOUSEBUTTONDOWN) { // Mouse pressed: play card if possible
        if (hovered_card != -1) {
          play_card(game, 0, (size_t)hovered_card);
          return true;
        }
      }
    }

    // Render the current game state
    render_all(renderer, game, hovered_card, 0);

    SDL_RenderPresent(renderer);
  }

  return false;
}

#endif
// TODO: handle result for AIs
bool play_card(Game* game, size_t player_index, size_t card_index) {
#ifdef TEST_ENV
  if (player_index >= 4) {
    ASSERT_INDENT();
    printf("Player index is out of bound (%zu)", player_index);
    return false;
  }
#else
  if (player_index >= 4) return false;
#endif

  Player* player = &game->players[player_index];

  // Check that the card index is within bounds
#ifdef TEST_ENV
  if (card_index >= player->n_cards) {
    ASSERT_INDENT();
    printf(
      "Card index is out of bound (%zu >= %" PRIu8 ")",
      card_index,
      player->n_cards);
    return false;
  }
#else
  if (card_index >= player->n_cards) return false;
#endif

#ifdef TEST_ENV
  if (game->pli[player_index].type != VOIDCARD) {
    ASSERT_INDENT();
    printf("No more space left for player %zu's card!", player_index);
  }
#else
  if (game->pli[player_index].type != VOIDCARD) return false;
#endif

  Card current_card = player->cards[card_index];

  // Shift the player's cards by one
  for (size_t n = card_index; n < player->n_cards - 1; n++) {
    player->cards[n] = player->cards[n + 1];
  }

  player->n_cards--;

#ifndef TEST_ENV
  // Animation loop
  uint32_t loop_start = SDL_GetTicks();
  const uint32_t anim_length = 500;
  while (SDL_GetTicks() - loop_start < anim_length) {
    SDL_Event event;
    SLEEP(25);

    // Poll events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYUP) { // Key pressed
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            return false;
        }
      } else if (event.type == SDL_QUIT) { // Window closed
        return false;
      }
    }

    // Render the current game state
    render_all(renderer, game, -1, player_index);

    float progress = (float)(SDL_GetTicks() - loop_start) / anim_length;
    uint32_t distance = (1 - INTERPOLATE_EASE_IN_OUT(progress)) * (CARD_WIDTH + 1);
    render_card_anim(renderer, game, window_width / 2, window_height / 2, player_index, current_card, distance);

    SDL_RenderPresent(renderer);
  }
#endif

  bool is_first_card = true;
  for (size_t n = 0; n < 4; n++) {
    if (game->pli[n].type != VOIDCARD) is_first_card = false;
  }
  if (is_first_card) {
    game->trick_color = current_card.type;
  }
  // Append the card to the current turn, if possible
  game->pli[player_index] = current_card;

  return true;
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
    game->trick_leader_position = leader_trick(game, (size_t)current_player);
  }

  update_scores(game);

  return true;
}

void update_scores(Game* game) {
  // TODO
}

bool play_all_turns(Game* game, size_t first_player_index) {
  game->trick_leader_position =
    first_player_index < 0 ? 0 : (first_player_index >= 4 ? 3 : first_player_index);

  for (size_t n = 0; n < 8; n++) {
    if (!game_turn(game)) return false;
  }

  return true;
}

void distribute_cards(Card cards[32], Game* game) {
  size_t n = 0;
  int position = 0; // TODO: grab the last dealer
  #define GIVE(v) for (size_t u = 0; u < (v); u++) game->players[position].cards[game->players[position].n_cards++] = cards[n++];
  for (size_t x = 0; x < 4; x++) {
    position = (position + 1) % 4;
    game->players[position].n_cards = 0;
    GIVE(3);
  }
  for (size_t x = 0; x < 4; x++) {
    position = (position + 1) % 4;
    GIVE(2);
  }
  for (size_t x = 0; x < 4; x++) {
    position = (position + 1) % 4;
    GIVE(3);
  }
}

void init_cards(Card cards[32]) {
  for (size_t n = 0; n < 32; n++) {
    cards[n].type = n / 8;
    cards[n].value = (n % 8) + 7;
  }
}

void shuffle_cards(Card cards[32]) {
  Card old_cards[32];
  for (size_t n = 0; n < 32; n++) {
    old_cards[n] = cards[n];
  }
  for (size_t n = 0; n < 32; n++) {
    size_t index = rand() % (32 - n);
    cards[n] = old_cards[index];
    // lshift cards
    for (size_t o = index; o < 32 - n - 1; o++) {
      old_cards[o] = old_cards[o + 1];
    }
  }
}

int dealing_phase(Game* game, size_t dealer) {
  game->contract_points = 0;
  for (int i = 1; i < 5; i++) {
    if ((dealer + i) % 4 == 0) {
      if (!player_announce_contract(game)) return 2;
    } else {
      ai_announce_contract(game, (dealer + i) % 4);
    }
  }
  int previous_contract_points = game->contract_points;
  if (game->contract_points == 0) { // nobody took a contract, we have to give new cards.
    return 0;
  } else if (game->contract_points == previous_contract_points) {
    return 1; // everyone passed after 1st player
  }
  previous_contract_points = game->contract_points;
  int player = (dealer + 1) % 4;
  int consecutive_pass = 0;
  do {
    if (player == 0) {
      if (!player_announce_contract(game)) return 2;
    } else {
      ai_announce_contract(game, player);
    }
    player = (player + 1) % 4;
    if (previous_contract_points == game->contract_points) {
      consecutive_pass = consecutive_pass + 1;
    } else {
      consecutive_pass = 0;
    }
    previous_contract_points = game->contract_points;
  } while (consecutive_pass < 3);
  // we make everyone announce one after an other until 3 people have passed.
  return 1;
}

bool player_announce_contract(Game* game) {
  return true;
}
