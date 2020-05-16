#include "display.h"
#include <SDL2/SDL_image.h>

bool zoom_surface(SDL_Surface* src, SDL_Surface* dst) {
  if (src->w == 0 || src->h == 0) return false;

  uint32_t factor = dst->w / src->w;

  if (factor == 0 || dst->w != src->w * factor || dst->h != src->h * factor) return false;

  // Source and destination pointers
  uint32_t* src_p = (uint32_t*)src->pixels;
  uint32_t* dst_p = (uint32_t*)dst->pixels;

  // Substep counter, ranging from 0 to `factor` and incrementing src_p upon reaching `factor`
  uint32_t sub_x = 0;
  for (size_t y = 0; y < src->h; y++) {
    uint32_t* row_src_p = src_p;

    for (uint32_t sub_y = 0; sub_y < factor; sub_y++) {
      src_p = row_src_p; // reset the source pointer to the beginning of the row
      for (size_t x = 0; x < dst->w; x++) {

        *dst_p = *src_p; // Write to destination

        if (sub_x == factor - 1) {
          sub_x = 0;
          src_p++;
        } else sub_x++;

        dst_p++;
      }
    }
  }

  return true;
}

SDL_Surface* zoom(SDL_Surface* src, uint32_t factor) {
  SDL_Surface* dst = SDL_CreateRGBSurface(0, src->w * factor, src->h * factor, 32, RMASK, GMASK, BMASK, AMASK);
  zoom_surface(src, dst);
  return dst;
}

SDL_Rect zoom_rect(SDL_Rect* src, uint32_t factor) {
  SDL_Rect res = {
    .x = src->x * factor,
    .y = src->y * factor,
    .w = src->w * factor,
    .h = src->h * factor
  };
  return res;
}

static SDL_Texture* outlines = NULL;
static SDL_Texture* cards[4][8];
static SDL_Rect card_rect;

bool init_card_textures(SDL_Renderer* renderer, uint32_t factor) {
  SDL_Surface* outlines_raw = IMG_Load(RESOURCES_DIR "card_outlines.png");
  if (!outlines_raw) return false;
  SDL_Surface* outlines_surface = zoom(outlines_raw, factor);
  outlines = SDL_CreateTextureFromSurface(renderer, outlines_surface);
  SDL_FreeSurface(outlines_raw);

  SDL_Surface* nums_raw = IMG_Load(RESOURCES_DIR "card_nums.png");
  if (!nums_raw) return false;
  SDL_Surface* nums = zoom(nums_raw, factor);
  SDL_FreeSurface(nums_raw);

  SDL_Surface* colors_raw = IMG_Load(RESOURCES_DIR "card_colors.png");
  if (!colors_raw) return false;
  SDL_Surface* colors = zoom(colors_raw, factor);
  SDL_FreeSurface(colors_raw);

  // top-left number destination
  SDL_Rect num1_dst = {
    .x = NUM1_X * factor,
    .y = NUM1_Y * factor,
    .w = NUM_WIDTH * factor,
    .h = NUM_HEIGHT * factor,
  };

  // bottom-right number destination
  SDL_Rect num2_dst = {
    .x = NUM2_X * factor,
    .y = NUM2_Y * factor,
    .w = NUM_WIDTH * factor,
    .h = NUM_HEIGHT * factor,
  };

  // center color destination
  SDL_Rect color_dst = {
    .x = COLOR_X * factor,
    .y = COLOR_Y * factor,
    .w = COLOR_WIDTH * factor,
    .h = COLOR_HEIGHT * factor,
  };

  for (size_t color = 0; color < 4; color++) {
    for (size_t num = 0; num < 8; num++) {
      SDL_Surface* tmp_surface = SDL_CreateRGBSurface(0, CARD_WIDTH * factor, CARD_HEIGHT * factor, 32, RMASK, GMASK, BMASK, AMASK);

      SDL_Rect num_src = {
        .x = NUM_WIDTH * (7 - num) * factor,
        .y = color < 2 ? NUM_HEIGHT * factor : 0,
        .w = NUM_WIDTH * factor,
        .h = NUM_HEIGHT * factor
      };

      SDL_Rect color_src = {
        .x = (num == 7 ? 2 : num > 3 ? 1 : 0) * factor * COLOR_WIDTH,
        .y = color * COLOR_HEIGHT * factor,
        .w = COLOR_WIDTH * factor,
        .h = COLOR_HEIGHT * factor
      };

      if (SDL_BlitSurface(nums, &num_src, tmp_surface, &num1_dst)) return false;
      if (SDL_BlitSurface(nums, &num_src, tmp_surface, &num2_dst)) return false;
      if (SDL_BlitSurface(colors, &color_src, tmp_surface, &color_dst)) return false;

      cards[color][num] = SDL_CreateTextureFromSurface(renderer, tmp_surface);
      SDL_FreeSurface(tmp_surface);
    }
  }

  SDL_FreeSurface(outlines_surface);
  SDL_FreeSurface(nums);
  SDL_FreeSurface(colors);

  card_rect.x = 0;
  card_rect.y = 0;
  card_rect.w = CARD_WIDTH * factor;
  card_rect.h = CARD_HEIGHT * factor;

  return true;
}

void destroy_card_textures() {
  for (size_t color = 0; color < 4; color++) {
    for (size_t num = 0; num < 8; num++) {
      SDL_DestroyTexture(cards[color][num]);
    }
  }
  SDL_DestroyTexture(outlines);
}

SDL_Texture* get_texture(CardColor color, uint8_t number) {
  if (number >= 8) return NULL;
  return cards[color][number];
}

SDL_Rect get_card_rect() {
  SDL_Rect res = card_rect;
  return res;
}

void render_card(SDL_Renderer* renderer, CardColor color, uint8_t number, uint8_t state, SDL_Rect* dst) {
  SDL_Rect outline_src = {
    .x = card_rect.w * state,
    .y = 0,
    .w = card_rect.w,
    .h = card_rect.h
  };
  SDL_Rect card_src = get_card_rect();

  SDL_RenderCopy(renderer, outlines, &outline_src, dst);
  if (state < 2) SDL_RenderCopy(renderer, get_texture(color, number), &card_src, dst);
}
