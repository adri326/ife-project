#include "display.h"
#include <ctype.h>
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
static SDL_Texture* glyphs[16][8][8];
static uint32_t zoom_factor;

bool init_textures(SDL_Renderer* renderer, uint32_t factor) {
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

  // Glyphs

  SDL_Surface* glyphs_raw = IMG_Load(RESOURCES_DIR "glyphs.png");
  if (!glyphs_raw) return false;
  SDL_Surface* glyphs_surface = zoom(glyphs_raw, factor);
  SDL_FreeSurface(glyphs_raw);

  uint32_t glyph_colors[16] = {
    0x000000ff, // black
    0x0000b0ff, // blue
    0x00b000ff, // green
    0x00b0b0ff, // cyan
    0xb00000ff, // red
    0xb300ffff, // purple
    0xffb000ff, // orange
    0xb0b0b0ff, // light gray
    0x808080ff, // gray
    0x0000ffff, // light blue
    0x00ff00ff, // light green
    0x00ffffff, // light cyan
    0xff0000ff, // light red
    0xff00ffff, // magenta
    0xffff00ff, // yellow
    0xffffffff, // white
  };

  for (size_t y = 0; y < 8; y++) {
    for (size_t x = 0; x < 8; x++) {
      for (size_t color = 0; color < 16; color++) {
        SDL_Surface* tmp_surface = SDL_CreateRGBSurface(0, GLYPH_WIDTH * factor, GLYPH_HEIGHT * factor, 32, RMASK, GMASK, BMASK, AMASK);
        SDL_Rect src = {
          .x = GLYPH_WIDTH * factor * x,
          .y = GLYPH_HEIGHT * factor * y,
          .w = GLYPH_WIDTH * factor,
          .h = GLYPH_HEIGHT * factor
        };
        SDL_Rect dst = {
          .x = 0,
          .y = 0,
          .w = GLYPH_WIDTH * factor,
          .h = GLYPH_HEIGHT * factor
        };
        if (SDL_BlitSurface(glyphs_surface, &src, tmp_surface, &dst)) return false;
        colorize_glyph(tmp_surface, glyph_colors[color]);
        glyphs[color][x][y] = SDL_CreateTextureFromSurface(renderer, tmp_surface);
        SDL_FreeSurface(tmp_surface);
      }
    }
  }

  SDL_FreeSurface(glyphs_surface);

  zoom_factor = factor;
  return true;
}

void destroy_textures() {
  for (size_t color = 0; color < 4; color++) {
    for (size_t num = 0; num < 8; num++) {
      SDL_DestroyTexture(cards[color][num]);
    }
  }

  for (size_t y = 0; y < 8; y++) {
    for (size_t x = 0; x < 8; x++) {
      for (size_t color = 0; color < 16; color++) {
        SDL_DestroyTexture(glyphs[color][x][y]);
      }
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

void colorize_glyph(SDL_Surface* glyph, uint32_t color) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  uint32_t n_color = color;
#else
  uint32_t a = color & 0xff;
  uint32_t b = (color & 0xff00) >> 8;
  uint32_t g = (color & 0xff0000) >> 16;
  uint32_t r = (color & 0xff000000) >> 24;
  uint32_t n_color = r + (g << 8) + (b << 16) + (a << 24);
#endif
  uint32_t* pixels = (uint32_t*)glyph->pixels;
  for (size_t y = 0; y < glyph->h; y++) {
    for (size_t x = 0; x < glyph->w; x++) {
      pixels[x + y * glyph->w] &= n_color;
    }
  }
}

void render_glyph(SDL_Renderer* renderer, char glyph, int32_t x, int32_t y, uint8_t color) {
  if (color >= 16) return;
  SDL_Rect src = {
    .x = 0,
    .y = 0,
    .w = GLYPH_WIDTH * zoom_factor,
    .h = GLYPH_HEIGHT * zoom_factor
  };
  SDL_Rect dst = {
    .x = x,
    .y = y,
    .w = GLYPH_WIDTH * zoom_factor,
    .h = GLYPH_HEIGHT * zoom_factor
  };

  size_t index = 63;
  if ((glyph >= 'a' && glyph <= 'z') || (glyph >= 'A' && glyph <= 'Z')) {
    index = (size_t)(toupper(glyph) - 'A');
  } else if (glyph >= '0' && glyph <= '9') {
    index = 25 + glyph - '0';
  } else if (glyph >= 128 && glyph < 192) {
    index = glyph;
  } else {
    switch (glyph) {
      case '+':
        index = 41;
        break;
      case '-':
        index = 42;
        break;
      case '/':
        index = 43;
        break;
      case ',':
        index = 44;
        break;
      case '.':
        index = 45;
        break;
      case '!':
        index = 46;
        break;
      case '?':
        index = 47;
        break;
      case '#':
        index = 48;
        break;
      case ':':
        index = 49;
        break;
      case ';':
        index = 50;
        break;
      case '_':
        index = 51;
        break;
      case '<':
        index = 52;
        break;
      case '>':
        index = 53;
        break;
      case '"':
        index = 54;
        break;
      case ' ':
        index = 63;
        break;
    }
  }

  SDL_RenderCopy(renderer, glyphs[color][index % 8][index / 8], &src, &dst);
}

void render_text(SDL_Renderer* renderer, char* glyphs, int32_t x, int32_t y, uint8_t color) {
  int32_t current_x = x;
  int32_t current_y = y;
  while (*glyphs) {
    render_glyph(renderer, *glyphs, current_x, current_y, color);
    if (*glyphs == '\n') {
      current_x = x;
      current_y += LINE_HEIGHT * zoom_factor;
    } else {
      current_x += (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor;
    }
    glyphs++;
  }
}
