#include "ai.h"
#include "display.h"
#include "rules.h"
#include <stdio.h>
#include <stdlib.h>

// Otherwise the linker errors out due to SDL2 redefining `main` by default
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
  Game game;
  Teams active_team; // variable that will help each time a function needs to
                     // know what team is concerned, e.g.: for the contracts

  // SDL_Init(SDL_INIT_VIDEO);

  // SDL_Window* window = SDL_CreateWindow(
  //   "This needs a title",
  //   SDL_WINDOWPOS_UNDEFINED,
  //   SDL_WINDOWPOS_UNDEFINED,
  //   512,
  //   512,
  //   0
  // );

  // SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE); // TODO: use hardware renderer
  // SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  // SDL_RenderClear(renderer);
  // SDL_RenderPresent(renderer);

  printf("Hello world!");

  // SDL_Delay(3000);
  // SDL_DestroyWindow(window);
  // SDL_Quit();

  return 0;
}
