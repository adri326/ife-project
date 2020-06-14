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
    render_all(renderer, game, hovered_card, 0, true);

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
    render_all(renderer, game, -1, player_index, true);

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
  size_t current_player = (size_t)game->trick_leader_position;
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
      if (!ai_turn(game, current_player)) return false;
    }
    game->trick_leader_position = leader_trick(game, current_player);
  }

  update_scores(game);
  printf("(%d, %d) : (%d, %d) : (%d, %d) : (%d, %d) -> %d\n",
    game->pli[0].type, game->pli[0].value,
    game->pli[1].type, game->pli[1].value,
    game->pli[2].type, game->pli[2].value,
    game->pli[3].type, game->pli[3].value,
    game->trick_leader_position
  );

#ifndef TEST_ENV
  // Animation loop
  uint32_t loop_start = SDL_GetTicks();
  const uint32_t anim_length = 1000;
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
    render_all(renderer, game, -1, -1, true);

    char* winner_str = game->trick_leader_position < 2
      ? (game->trick_leader_position == 0 ? "You won the trick" : "West won the trick")
      : (game->trick_leader_position == 2 ? "North won the trick" : "East won the trick");

    render_text(
      renderer,
      winner_str,
      window_width / 2 - strlen(winner_str) * (GLYPH_MARGIN + GLYPH_WIDTH) * zoom_factor / 2,
      window_height / 2 - GLYPH_HEIGHT * zoom_factor * 12,
      0
    );

    render_player_arrow_anim(
      renderer,
      game->trick_leader_position,
      INTERPOLATE_EASE_IN_OUT((float)(SDL_GetTicks() - loop_start) / anim_length)
    );

    SDL_RenderPresent(renderer);
  }
#endif

  return true;
}

void update_scores(Game* game) {
  // Maybe do an animation?
  game->players[game->trick_leader_position].trick_points_total += trick_points(
    game->pli[0],
    game->pli[1],
    game->pli[2],
    game->pli[3],
    *game
  );
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

bool anim_dealer(Game* game, size_t dealer) {
#ifndef TEST_ENV
  uint32_t loop_start = SDL_GetTicks();
  const uint32_t anim_length = game->general_attacker == dealer ? 1000 : 500;
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
    render_all(renderer, game, -1, -1, false);
    render_bids(renderer, game);
    render_player_arrow_anim(
      renderer,
      dealer,
      INTERPOLATE_EASE_IN_OUT((float)(SDL_GetTicks() - loop_start) / anim_length)
    );

    SDL_RenderPresent(renderer);
  }
#endif
  return true;
}

int dealing_phase(Game* game, size_t dealer) {
  game->contract_points = 0;
  for (int i = 1; i < 5; i++) {
    if ((dealer + i) % 4 == 0) {
      if (!player_announce_contract(game)) return 2;
    } else {
      ai_announce_contract(game, (dealer + i) % 4);
    }
    if (!anim_dealer(game, (dealer + i) % 4)) return 2;
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
    if (!anim_dealer(game, player)) return 2;
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

/*
Dear code reader,
Sorry.
*/
bool player_announce_contract(Game* game) {
#ifdef TEST_ENV
  return true;
#else
  int screen = 0;
  int32_t mouse_x = 0;
  int32_t mouse_y = 0;

  #define PRINT_CENTER_AROUND(text, x, y, color) render_text(renderer, (text), x - strlen(text) * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2, y, color)
  #define PRINT_CENTER(text, y, color) PRINT_CENTER_AROUND(text, window_width / 2, y, color)
  #define BUTTON_CENTER_AROUND(text, x, y, color) render_button( \
    renderer, \
    (text), \
    x - strlen(text) * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2, \
    y, \
    is_button_hovered((text), x - strlen(text) * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2, y, mouse_x, mouse_y), \
    color)
  #define BUTTON_CENTER(text, y, color) BUTTON_CENTER_AROUND(text, window_width / 2, y, color)
  #define SHIFT_Y(n) ((GLYPH_HEIGHT + GLYPH_MARGIN) * zoom_factor * (n))
  #define SHIFT_X(n) ((n) * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor)
  #define HANDLE_BUTTON(text, x, y) if (is_button_hovered((text), x - strlen(text) * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2, y, mouse_x, mouse_y))
  #define COLUMN_SPACING (GLYPH_WIDTH * 10 * zoom_factor)
  #define BUTTON_SPACING ((GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor * 3)
  if (game->contract_points < 80) game->contract_points = 80;
  int points = game->contract_points + 10;
  int contract_type = 0;
  int color = 0;
  while (true) {
    SDL_Event event;
    SLEEP(50);

    // Poll events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYUP) { // Key pressed
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            return false;
        }
      } else if (event.type == SDL_QUIT) { // Window closed
        return false;
      } else if (event.type == SDL_MOUSEMOTION) { // Mouse moved
        mouse_x = event.motion.x;
        mouse_y = event.motion.y;
      } else if (event.type == SDL_MOUSEBUTTONDOWN) { // Mouse pressed: handle buttons
        switch (screen) {
          case 0:
            HANDLE_BUTTON("Pass", window_width / 2, window_height / 2 + SHIFT_Y(2)) {
              return true;
            }
            HANDLE_BUTTON("Bid", window_width / 2, window_height / 2 + SHIFT_Y(4)) {
              screen = 1;
              continue;
            }
            if (game->general_attacker == 3 && game->active_contract != SURCOINCHE) {
              HANDLE_BUTTON(game->active_contract == COINCHE ? "Surcoinche" : "Coinche", window_width / 2, window_height / 2 + SHIFT_Y(6)) {
                if (game->active_contract == COINCHE) game->active_contract = SURCOINCHE;
                else game->active_contract = COINCHE;
                return true;
              }
            }
            break;
          case 1:
            HANDLE_BUTTON("Points:", window_width / 2 - COLUMN_SPACING, window_height / 2 + SHIFT_Y(-8)) {
              contract_type = 0;
            }
            HANDLE_BUTTON("Capot", window_width / 2, window_height / 2 + SHIFT_Y(-8)) {
              contract_type = 1;
            }
            HANDLE_BUTTON("General", window_width / 2 + COLUMN_SPACING, window_height / 2 + SHIFT_Y(-8)) {
              contract_type = 2;
            }

            HANDLE_BUTTON("-", window_width / 2 - COLUMN_SPACING - GLYPH_WIDTH * 4 * zoom_factor, window_height / 2 + SHIFT_Y(-5)) {
              if (points > game->contract_points + 10) points -= 10;
            }
            HANDLE_BUTTON("+", window_width / 2 - COLUMN_SPACING + GLYPH_WIDTH * 4 * zoom_factor, window_height / 2 + SHIFT_Y(-5)) {
              if (points < 240) points += 10;
            }
            for (size_t current_color = 0; current_color < 4; current_color++) {
              HANDLE_BUTTON(".", window_width / 2 + BUTTON_SPACING * (2 * current_color - 3) / 2, window_height / 2 + SHIFT_Y(0)) {
                color = current_color;
              }
            }
            HANDLE_BUTTON(
              "All trump",
              window_width / 2 - (strlen("All trump") + 2) * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2,
              window_height / 2 + SHIFT_Y(3)
            ) {
              color = 4;
            }
            HANDLE_BUTTON(
              "No trump",
              window_width / 2 + (strlen("No trump") + 2) * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2,
              window_height / 2 + SHIFT_Y(3)
            ) {
              color = 5;
            }
            HANDLE_BUTTON("Bid!", window_width / 2, window_height / 2 + SHIFT_Y(6)) {
              game->active_contract = contract_type == 0 ? CHOSENCOLOUR : contract_type == 1 ? CAPOT : GENERAL;
              game->contract_points = contract_type == 0 ? points : contract_type == 1 ? CAPOT_POINTS : GENERAL_POINTS;
              game->active_trump = color;
              return true;
            }
        }
      }
    }

    // Render the current game state
    render_all(renderer, game, -1, -1, false);

    switch (screen) {
      case 0:
        render_bids(renderer, game);
        BUTTON_CENTER("Pass", window_height / 2 + SHIFT_Y(2), 0);
        BUTTON_CENTER("Bid", window_height / 2 + SHIFT_Y(4), 0);
        if (game->general_attacker == 3 && game->active_contract != SURCOINCHE) {
          BUTTON_CENTER(game->active_contract == COINCHE ? "Surcoinche" : "Coinche", window_height / 2 + SHIFT_Y(6), 0);
        }
        break;
      case 1:
        {
          char points_str[4];
          sprintf(points_str, "%d", points);
          render_button(
            renderer,
            "Points:",
            window_width / 2 - COLUMN_SPACING - strlen("Points:") * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2,
            window_height / 2 + SHIFT_Y(-8),
            contract_type == 0,
            0
          );
          PRINT_CENTER_AROUND(points_str, window_width / 2 - COLUMN_SPACING, window_height / 2 + SHIFT_Y(-5), 0);
          BUTTON_CENTER_AROUND("-", window_width / 2 - COLUMN_SPACING - GLYPH_WIDTH * 4 * zoom_factor, window_height / 2 + SHIFT_Y(-5), 0);
          BUTTON_CENTER_AROUND("+", window_width / 2 - COLUMN_SPACING + GLYPH_WIDTH * 4 * zoom_factor, window_height / 2 + SHIFT_Y(-5), 0);

          render_button(
            renderer,
            "Capot",
            window_width / 2 - strlen("Capot") * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2,
            window_height / 2 + SHIFT_Y(-8),
            contract_type == 1,
            0
          );

          render_button(
            renderer,
            "General",
            window_width / 2 + COLUMN_SPACING - strlen("General") * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2,
            window_height / 2 + SHIFT_Y(-8),
            contract_type == 2,
            0
          );

          PRINT_CENTER("Trump:", window_height / 2 + SHIFT_Y(-2), 8);

          for (size_t current_color = 0; current_color < 4; current_color++) {
            char color_str[2] = {128 + 37 + current_color, 0};
            render_button(
              renderer,
              color_str,
              window_width / 2 + BUTTON_SPACING * (2 * current_color - 3) / 2 - (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2,
              window_height / 2 + SHIFT_Y(0),
              color == current_color,
              current_color < 2 ? 12 : 0
            );
          }

          render_button(
            renderer,
            "All trump",
            window_width / 2 - (strlen("All trump") + 2) * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor,
            window_height / 2 + SHIFT_Y(3),
            color == 4,
            0
          );
          render_button(
            renderer,
            "No trump",
            window_width / 2 + 2 * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor,
            window_height / 2 + SHIFT_Y(3),
            color == 5,
            0
          );

          BUTTON_CENTER("Bid!", window_height / 2 + SHIFT_Y(6), 0);
          break;
        }
    }

    SDL_RenderPresent(renderer);
  }
#endif
}
