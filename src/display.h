#ifndef DISPLAY_H
#define DISPLAY_H

#include "rules.h"
#include <inttypes.h>
#include <stdbool.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#ifndef RESOURCES_DIR
#define RESOURCES_DIR "./resources/"
#endif

#define CARD_WIDTH 19
#define CARD_HEIGHT 28
#define DECK_PADDING 0
#define NUM_WIDTH 5
#define NUM_HEIGHT 5
#define COLOR_WIDTH 9
#define COLOR_HEIGHT 10
#define NUM1_X 3
#define NUM1_Y 3
#define NUM2_X 11
#define NUM2_Y 20
#define COLOR_X 5
#define COLOR_Y 9
#define GLYPH_WIDTH 5
#define GLYPH_HEIGHT 5
#define LINE_HEIGHT 7
#define GLYPH_MARGIN 1

#define TITLE_WIDTH 250
#define TITLE_HEIGHT 250

#define BG_RED 189
#define BG_GREEN 204
#define BG_BLUE 210

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff
#else
#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000
#endif

#define YOUR_TURN_MSG "It is your turn!"
#define ACTIVE_TRUMP_MSG "Active trump:"
#define ALLTRUMP_MSG "All trump"
#define NOTRUMP_MSG "No trump"
#define CONTRACTED_TEAM_MSG "Contracted:"
#define CHOSENCOLOUR_MSG "Point goal:"
#define GOAL_MSG "Goal:"
#define CAPOT_MSG "Capot"
#define GENERAL_MSG "General("
#define COINCHE_MSG "Coinche"
#define SURCOINCHE_MSG "Surcoinche"
#define INTRO_MSG "Press a key to start..."

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

/** Loads all of the textures, upscales them by `factor`, stitches them together and stores them for
* later use. Be sure to call `destroy_textures` later!
* @param renderer - The current window renderer, used to generate the texture off surfaces
* @param factor - The upscaling factor
**/
bool init_textures(SDL_Renderer* renderer, uint32_t factor);

/** Destroys the textures allocated by `init_card_textures`
**/
void destroy_textures();

/** Returns the SDL_Texture associated with the given card
* @param color - The card color
* @param number - The card value (cf. rules.h)
**/
SDL_Texture* get_texture(CardColor color, uint8_t number);

/** Returns the SDL_Rect corresponding to the dimensions of a card texture **/
SDL_Rect get_card_rect();

/** Renders the title
* @param renderer - The current renderer
* @param x - The X coordinate of the center of the title
* @param y - The Y coordinate of the center of the title
**/
void render_title(SDL_Renderer* renderer, uint32_t x, uint32_t y);

/** Renders a card
* @param renderer - The current renderer
* @param color - The card color
* @param number - The card value
* @param state - The state of the card (0 = front, 1 = front highlighted, 2 = back, 3 = back
* highlighted)
* @param dst - The destination coordinate
**/
void render_card(
  SDL_Renderer* renderer,
  CardColor color,
  uint8_t number,
  uint8_t state,
  SDL_Rect* dst);

/** Colorizes a white/transparent RGBA glyph
* @param glyph - The glyph surface
* @param color - The color it will have
**/
void colorize_glyph(SDL_Surface* glyph, uint32_t color);

/** Renders a single char
* @param renderer - The current renderer
* @param glyph - The char to render
* @param x - The x position of the glyph
* @param y - The y position of the glyph
* @param color - The color of the glyph
**/
void render_glyph(SDL_Renderer* renderer, char glyph, int32_t x, int32_t y, uint8_t color);

/** Renders a string
* @param renderer - The current renderer
* @param glyphs - The string to render
* @param x - The x position of the glyph
* @param y - The y position of the glyph
* @param color - The color of the glyph
**/
void render_text(SDL_Renderer* renderer, char* glyphs, int32_t x, int32_t y, uint8_t color);

/** Renders the player's deck
* @param renderer - The current renderer
* @param player - The player's structure
* @param x - The x position of the deck's center
* @param y - The y position of the deck's top
**/
void render_deck(SDL_Renderer* renderer, Player* player, uint32_t x, uint32_t y, int selected_card);

/** Returns which card of the deck is hovered
* @param player - The player's structure
* @param deck_x - The x position of the deck's center
* @param deck_y - The y position of the deck's top
* @param mouse_x - The mouse X position
* @param mouse_y - The mouse Y position
* @returns -1 if no card is hovered, u ∈ [0; 8) otherwise
**/
int get_hovered_card(
  Player* player,
  uint32_t deck_x,
  uint32_t deck_y,
  int32_t mouse_x,
  int32_t mouse_y);

/** Renders the cards at the center of the board
* @param renderer - The current renderer
* @param game - The current Game
* @param x - Center of the board
* @param y - Center of the board
**/
void render_round(SDL_Renderer* renderer, Game* game, uint32_t x, uint32_t y);

/** Renders a card that will be put at the center of the board
* @param renderer - The current renderer
* @param game - The current Game
* @param x - Center of the board
* @param y - Center of the board
* @param player - The player that is playing this card
* @param card - The card to display
* @param distance - The distance to its intended spot
**/
void render_card_anim(SDL_Renderer* renderer, Game* game, uint32_t x, uint32_t y, size_t player, Card card, float distance);

/** Renders an opponent's deck
* @param renderer - The current renderer
* @param ai - The AI's Player struct
* @param x - The deck's top left corner's X coordinate
* @param y - The deck's top left corner's Y coordinate
**/
void render_ai_deck(SDL_Renderer* renderer, Player* ai, uint32_t x, uint32_t y);

/** Calls render_deck, render_round, render_ai_deck and get_hovered_card. Does not update the view.
* @param renderer - The current renderer
* @param game - The current game
* @param hovered_card - The card that should be highlighted
* @param current_player - The player whose turn it is
* @param render_extras - Render scores, active contract, etc.
**/
void render_all(SDL_Renderer* renderer, Game* game, int hovered_card, size_t current_player, bool render_extras);

/** Renders a button
* @param renderer - The renderer
* @param text - The text to put inside of the button
* @param x - The x position of the button (top-left corner of the inner text)
* @param y - The y position of the button (top-left corner of the inner text)
* @param state - The state of the button (pressed/not pressed)
* @param color - The color of the text inside of the button
**/
void render_button(SDL_Renderer* renderer, char* text, uint32_t x, uint32_t y, bool state, uint8_t color);

/** Returns wether or not a button is being hovered
* @param text - The text to put inside of the button
* @param button_x - The x position of the button (top-left corner of the inner text)
* @param button_y - The y position of the button (top-left corner of the inner text)
* @param mouse_x - The mouse X position
* @param mouse_y - The mouse Y position
* @returns true if the button is hovered, false otherwise
**/
bool is_button_hovered(char* text, uint32_t button_x, uint32_t button_y, int32_t mouse_x, int32_t mouse_y);

/** Renders the bid made so far
* @param renderer - The current renderer
* @param game - The current game
**/
void render_bids(SDL_Renderer* renderer, Game* game);

/** Renders an arrow point to a player's deck
* @param renderer - The current renderer
* @param player - The player ot point towards
* @param progress - The progress of the animation
**/
void render_player_arrow_anim(SDL_Renderer* renderer, size_t player, float progress);

#endif // DISPLAY_H
