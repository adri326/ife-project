#include "ai.h"
#include "display.h"
#include "rules.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Otherwise the linker errors out due to SDL2 redefining `main` by default
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
  // Game game;
  // Teams active_team; // variable that will help each time a function needs to
  //                    // know what team is concerned, e.g.: for the contracts

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* window = SDL_CreateWindow(
    "This needs a title",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    512,
    512,
    0
  );

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE); // TODO: use hardware renderer
  SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  SDL_Surface* test = IMG_Load("../resources/card_colors.png");
  if (!test) {
    printf("Uh oh! %s\n", SDL_GetError());
  }
  SDL_Surface* test2 = zoom(test, 4);
  SDL_Texture* test2_tex = SDL_CreateTextureFromSurface(renderer, test2);
  SDL_Rect src_rect = {
    .x = 0,
    .y = 0,
    .w = 9,
    .h = 10,
  };
  SDL_Rect src2_rect = zoom_rect(&src_rect, 4);
  SDL_RenderCopy(renderer, test2_tex, &src2_rect, &src2_rect);
  SDL_FreeSurface(test);
  SDL_FreeSurface(test2);
  SDL_DestroyTexture(test2_tex);
  SDL_UpdateWindowSurface(window);

  SDL_Delay(3000);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
