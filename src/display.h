#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <inttypes.h>
#include <SDL2/SDL.h>

/** Zooms a surface; assumes that `dst`'s dimensions is an integer multiple of these of `src`.
* Assumes that both `src` and `dst` are 32-bit RGBA surfaces.
* @param src - The source surface
* @param dst - The destination surface
* @returns true on success, false otherwise
**/
bool zoom_surface(SDL_Surface* src, SDL_Surface* dst);

/** Allocates a zoomed version of `src`, upscaled by a factor of `factor`.
* Assumes that `src` is a 32-bit RGBA surface.
* @param src - The source surface
* @param ratio - The upscaling factor
**/
SDL_Surface* zoom(SDL_Surface* src, uint32_t factor);

/** Upscales an SDL_Rect by `factor`.
* @param src - The source SDL_Rect
* @param factor - The upscaling factor
**/
SDL_Rect zoom_rect(SDL_Rect* src, uint32_t factor);

#endif // DISPLAY_H
