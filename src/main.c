#include "ai.h"
#include "display.h"
#include "rules.h"
#include "game.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Otherwise the linker errors out due to SDL2 redefining `main` by default
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

uint32_t zoom_factor;
uint32_t window_width;
uint32_t window_height;
SDL_Renderer* renderer;

void display_test(void);

int main(int argc, char* argv[]) {
  srand(time(0));
  // Game game;
  // Teams active_team; // variable that will help each time a function needs to
  //                    // know what team is concerned, e.g.: for the contracts

  display_test();

  return 0;
}

void display_test() {
  SDL_Init(SDL_INIT_VIDEO);
  zoom_factor = 4;
  window_width = (CARD_WIDTH + 2) * 12 * 4;
  window_height = (CARD_HEIGHT + 2) * 8 * 4;

  SDL_Window* window = SDL_CreateWindow(
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
    .players = {player, computer1, computer2, computer3},
    .pli = {
      {.type = VOIDCARD},
      {.type = HEARTS, .value = 7},
      {.type = TILES, .value = 11},
      {.type = VOIDCARD}}};

  ai_turn(&game, 3);
  players_turn(&game);

  destroy_textures();
  SDL_DestroyWindow(window);
  SDL_Quit();
}
