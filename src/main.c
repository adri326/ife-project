#include "ai.h"
#include "display.h"
#include "rules.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
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
    (CARD_WIDTH + 2) * 8 * 4,
    (CARD_HEIGHT + 2) * 4 * 4,
    0
  );

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE); // TODO: use hardware renderer
  SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  if (!init_card_textures(renderer, 4)) {
    printf("Uh oh! %s\n", SDL_GetError());
  }

  bool exit = false;
  uint8_t state = 0;

  while (!exit) {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    for (CardColor color = 0; color < 4; color++) {
      for (uint8_t num = 0; num < 8; num++) {
        SDL_Rect dst_rect = get_card_rect();
        dst_rect.x = num * (CARD_WIDTH + 2) * 4 + 1;
        dst_rect.y = color * (CARD_HEIGHT + 2) * 4 + 1;
        render_card(renderer, color, num, state, &dst_rect);
      }
    }
    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYUP) {
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            exit = true;
            break;
          case SDLK_1:
            state = 0;
            break;
          case SDLK_2:
            state = 1;
            break;
          case SDLK_3:
            state = 2;
            break;
          case SDLK_4:
            state = 3;
            break;
        }
      } else if (event.type == SDL_QUIT) exit = true;
    }
    SLEEP(200);
  }

  destroy_card_textures();
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
