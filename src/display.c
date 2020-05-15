#include "display.h"

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
  // Endianness :)
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  uint32_t rmask = 0xff000000;
  uint32_t gmask = 0x00ff0000;
  uint32_t bmask = 0x0000ff00;
  uint32_t amask = 0x000000ff;
#else
  uint32_t rmask = 0x000000ff;
  uint32_t gmask = 0x0000ff00;
  uint32_t bmask = 0x00ff0000;
  uint32_t amask = 0xff000000;
#endif
  SDL_Surface* dst = SDL_CreateRGBSurface(0, src->w * factor, src->h * factor, 32, rmask, gmask, bmask, amask);
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
