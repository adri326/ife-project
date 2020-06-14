#include "ai.h"
#include "display.h"
#include "rules.h"
#include "game.h"
#include "leaderboards.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Otherwise the linker errors out due to SDL2 redefining `main` by default
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

uint32_t zoom_factor;
uint32_t window_width;
uint32_t window_height;
SDL_Window* window;
SDL_Renderer* renderer;

void run_game(void);
bool display_title(void);
void display_init(void);
void display_destroy(void);
bool display_main_menu(void);

int main(int argc, char* argv[]) {
  srand(time(0));

  display_init();
  while (true) {
    if (!display_title()) break;
    if (!display_main_menu()) break;

    run_game();
  }

  display_destroy();

  return 0;
}

void display_init() {
  SDL_Init(SDL_INIT_VIDEO);
  zoom_factor = 4;
  window_width = (CARD_WIDTH + 2) * 12 * 4;
  window_height = (CARD_HEIGHT + 2) * 8 * 4;

  window = SDL_CreateWindow(
    "This needs a title",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    window_width,
    window_height,
    0);

  renderer =
    SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE); // TODO: use hardware renderer
  SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  if (!init_textures(renderer, 4)) { printf("Uh oh! %s\n", SDL_GetError()); }
}

bool display_title() {
  // Display loop
  int anim_start = SDL_GetTicks();
  while (true) {
    SDL_Event event;
    SLEEP(50);

    // Poll events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYUP) { // Key pressed
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            return false;
          default:
            return true;
        }
      } else if (event.type == SDL_QUIT) { // Window closed
        return false;
      } else if (event.type == SDL_MOUSEBUTTONDOWN) { // Mouse pressed
        return true;
      }
    }

    SDL_SetRenderDrawColor(renderer, BG_RED, BG_GREEN, BG_BLUE, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    render_title(renderer, window_width / 2, window_height / 4);

    if (((SDL_GetTicks() - anim_start) / 1000) % 2 == 0) {
      render_text(
        renderer,
        INTRO_MSG,
        window_width / 2 - strlen(INTRO_MSG) * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2,
        window_height - GLYPH_HEIGHT * 2 * zoom_factor,
        0
      );
    }

    SDL_RenderPresent(renderer);
  }
}

bool display_main_menu() {
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
#define SHIFT_Y(n) ((GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor * n)
#define HANDLE_BUTTON(text, x, y) if (is_button_hovered((text), x - strlen(text) * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2, y, mouse_x, mouse_y))

  BestScores best_scores = load_best_scores(0);
  // Display loop
  while (true) {
    SDL_Event event;
    SLEEP(50);

    // Poll events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYUP) { // Key pressed
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            return false;
          default:
            if (screen == 1) screen = 0;
        }
      } else if (event.type == SDL_QUIT) { // Window closed
        return false;
      } else if (event.type == SDL_MOUSEMOTION) { // Mouse moved
        mouse_x = event.motion.x;
        mouse_y = event.motion.y;
      } else if (event.type == SDL_MOUSEBUTTONDOWN) { // Mouse pressed: play card if possible
        switch (screen) {
          case 0:
            HANDLE_BUTTON("Play!", window_width / 2, window_height / 2 + SHIFT_Y(-2)) {
              return true;
            }
            HANDLE_BUTTON("Leaderboards", window_width / 2, window_height / 2 + SHIFT_Y(0)) {
              screen = 1;
            }
            HANDLE_BUTTON("Exit", window_width / 2, window_height / 2 + SHIFT_Y(2)) {
              return false;
            }
            break;
          case 1:
            screen = 0;
            break;
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, BG_RED, BG_GREEN, BG_BLUE, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    if (screen == 0) render_title(renderer, window_width / 2, window_height / 4);

    switch (screen) {
      case 0:
        BUTTON_CENTER("Play!", window_height / 2 + SHIFT_Y(-2), 0);
        BUTTON_CENTER("Leaderboards", window_height / 2 + SHIFT_Y(0), 0);
        BUTTON_CENTER("Exit", window_height / 2 + SHIFT_Y(2), 0);
        break;
      case 1:
        PRINT_CENTER("Leaderboards", window_height / 4, 9);
        for (size_t n = 0; n < 10; n++) {
          if (best_scores.scores[n].name[0] == 0) break;
          char line_str[50];
          sprintf(
            line_str,
            "%c%c%c: %zu wins / %zu points",
            (char)toupper(best_scores.scores[n].name[0]),
            (char)toupper(best_scores.scores[n].name[1]),
            (char)toupper(best_scores.scores[n].name[2]),
            best_scores.scores[n].wins,
            best_scores.scores[n].points
          );
          PRINT_CENTER(line_str, window_height / 4 + SHIFT_Y(2 + n), 0);
        }
    }

    SDL_RenderPresent(renderer);
  }
}

void run_game() {
  Game game = {
    .players = {{}, {}, {}, {}},
    .active_trump = TRUMP_HEARTS,
    .contract_points = 500,
    .active_contract = -1,
    .general_attacker = -1,
    .pli = {
      {.type = VOIDCARD},
      {.type = VOIDCARD},
      {.type = VOIDCARD},
      {.type = VOIDCARD}}};

  Card cards[32];
  init_cards(cards);
  int dealing_res;
  do {
    shuffle_cards(cards);
    distribute_cards(cards, &game);
    for (size_t n = 0; n < 4; n++) {
      game.players[n].n_cards = 5;
    }
    dealing_res = dealing_phase(&game, 0);
    if (dealing_res == 2) return;
  } while (dealing_res == 0);
  for (size_t n = 0; n < 4; n++) {
    game.players[n].n_cards = 8;
  }

  play_all_turns(&game, 1);
  // ai_turn(&game, 1);
  // ai_turn(&game, 2);
  // ai_turn(&game, 3);
  // players_turn(&game);
}

void display_destroy() {
  destroy_textures();
  SDL_DestroyWindow(window);
  SDL_Quit();
}
