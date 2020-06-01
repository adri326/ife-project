#include "ai.h"
#include "display.h"
#include "rules.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Otherwise the linker errors out due to SDL2 redefining `main` by default
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifdef _WIN32
#define IS_WIN
#endif
#ifdef _WIN64
#define IS_WIN
#endif
#ifdef IS_WIN
#include <windows.h>
#define SLEEP(x) SleepEx(x, true)
#else
#include <unistd.h>
#define SLEEP(x) usleep(x * 1000)
#endif

void display_test(void);
void render_card_test(SDL_Renderer* renderer, uint8_t state);
void render_deck_test(
  SDL_Renderer* renderer,
  Player* player,
  uint32_t x,
  uint32_t y,
  int32_t mouse_x,
  int32_t mouse_y);
void render_round_test(SDL_Renderer* renderer, Game* game, uint32_t x, uint32_t y);
void render_ai_deck_test(SDL_Renderer* renderer, Player* ai, uint32_t x, uint32_t y);

int main(int argc, char* argv[]) {
  // Game game;
  // Teams active_team; // variable that will help each time a function needs to
  //                    // know what team is concerned, e.g.: for the contracts

  display_test();

  return 0;
}

void display_test() {
  SDL_Init(SDL_INIT_VIDEO);
  uint32_t WIDTH = (CARD_WIDTH + 2) * 12 * 4;
  const uint32_t HEIGHT = (CARD_HEIGHT + 2) * 8 * 4;

  SDL_Window* window = SDL_CreateWindow(
    "This needs a title",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    WIDTH,
    HEIGHT,
    0);

  SDL_Renderer* renderer =
    SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE); // TODO: use hardware renderer
  SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  if (!init_textures(renderer, 4)) { printf("Uh oh! %s\n", SDL_GetError()); }

  bool exit = false;
  // uint8_t state = 0;
  int32_t mouse_x = 0;
  int32_t mouse_y = 0;

  Player player = {
    .n_cards = 6,
    .cards = {
      {.type = HEARTS, .value = 7},
      {.type = TILES, .value = 8},
      {.type = CLOVERS, .value = 9},
      {.type = SPIKES, .value = 10},
      {.type = HEARTS, .value = 11},
      {.type = TILES, .value = 12},
      {.type = CLOVERS, .value = 13},
      {.type = SPIKES, .value = 14}}};

  Player computer1 = {
    .cards_revealed = 0b00011100,
    .n_cards = 5,
    .cards = {
      {.type = HEARTS, .value = 7},
      {.type = TILES, .value = 8},
      {.type = CLOVERS, .value = 7},
      {.type = CLOVERS, .value = 8},
      {.type = CLOVERS, .value = 9},
      {.type = TILES, .value = 12},
      {.type = CLOVERS, .value = 13},
      {.type = SPIKES, .value = 14}}};

  Player computer2 = {
    .cards_revealed = 0,
    .n_cards = 5,
    .cards = {
      {.type = HEARTS, .value = 7},
      {.type = TILES, .value = 8},
      {.type = CLOVERS, .value = 7},
      {.type = CLOVERS, .value = 8},
      {.type = CLOVERS, .value = 9},
      {.type = TILES, .value = 12},
      {.type = CLOVERS, .value = 13},
      {.type = SPIKES, .value = 14}}};

  Player computer3 = {
    .cards_revealed = 0,
    .n_cards = 6,
    .cards = {
      {.type = HEARTS, .value = 7},
      {.type = TILES, .value = 8},
      {.type = CLOVERS, .value = 7},
      {.type = CLOVERS, .value = 8},
      {.type = CLOVERS, .value = 9},
      {.type = TILES, .value = 12},
      {.type = CLOVERS, .value = 13},
      {.type = SPIKES, .value = 14}}};

  Game game = {
    .pli = {
      {.type = HEARTS, .value = 7},
      {.type = TILES, .value = 11},
      {.type = VOIDCARD},
      {.type = VOIDCARD}}};

  while (!exit) {
    SDL_Event event;
    SLEEP(50);
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            exit = true;
            break;
          case SDLK_1:
            // state = 0;
            break;
          case SDLK_2:
            // state = 1;
            break;
          case SDLK_3:
            // state = 2;
            break;
          case SDLK_4:
            // state = 3;
            break;
        }
      } else if (event.type == SDL_QUIT) {
        exit = true;
      } else if (event.type == SDL_MOUSEMOTION) {
        mouse_x = event.motion.x;
        mouse_y = event.motion.y;
      }
    }

    SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // render_card_test(renderer, state);
    render_deck_test(
      renderer,
      &player,
      WIDTH / 2,
      HEIGHT - (CARD_HEIGHT + DECK_PADDING) * 4,
      mouse_x,
      mouse_y);
    render_round_test(renderer, &game, WIDTH / 2, HEIGHT / 2);
    render_ai_deck_test(renderer, &computer1, 0, HEIGHT / 2 - CARD_HEIGHT * 2);
    render_ai_deck_test(renderer, &computer2, WIDTH / 2 - CARD_WIDTH * 4, 0);
    render_ai_deck_test(renderer, &computer3, WIDTH - CARD_WIDTH * 8, HEIGHT / 2 - CARD_HEIGHT * 2);

    SDL_RenderPresent(renderer);
  }

  destroy_textures();
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void render_card_test(SDL_Renderer* renderer, uint8_t state) {
  for (CardColor color = 0; color < 4; color++) {
    for (uint8_t num = 0; num < 8; num++) {
      SDL_Rect dst_rect = get_card_rect();
      dst_rect.x = num * (CARD_WIDTH + 2) * 4 + 1;
      dst_rect.y = color * (CARD_HEIGHT + 2) * 4 + 1;
      render_card(renderer, color, num, state, &dst_rect);
    }
  }
  // render_text(renderer, "Hello, world!", 4, 4, 0);
}

void render_deck_test(
  SDL_Renderer* renderer,
  Player* player,
  uint32_t x,
  uint32_t y,
  int32_t mouse_x,
  int32_t mouse_y) {
  render_deck(renderer, player, x, y, get_hovered_card(player, x, y, mouse_x, mouse_y));
}

void render_round_test(SDL_Renderer* renderer, Game* game, uint32_t x, uint32_t y) {
  render_round(renderer, game, 1, x, y);
}

void render_ai_deck_test(SDL_Renderer* renderer, Player* ai, uint32_t x, uint32_t y) {
  render_ai_deck(renderer, ai, x, y);
}
