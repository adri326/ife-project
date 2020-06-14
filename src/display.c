#include "display.h"
#include <SDL2/SDL_image.h>
#include <ctype.h>

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
        } else
          sub_x++;

        dst_p++;
      }
    }
  }

  return true;
}

SDL_Surface* zoom(SDL_Surface* src, uint32_t factor) {
  SDL_Surface* dst =
    SDL_CreateRGBSurface(0, src->w * factor, src->h * factor, 32, RMASK, GMASK, BMASK, AMASK);
  zoom_surface(src, dst);
  return dst;
}

SDL_Rect zoom_rect(SDL_Rect* src, uint32_t factor) {
  SDL_Rect res =
    {.x = src->x * factor, .y = src->y * factor, .w = src->w * factor, .h = src->h * factor};
  return res;
}

static SDL_Texture* outlines = NULL;
static SDL_Texture* buttons = NULL;
static SDL_Texture* cards[4][8];
static SDL_Rect card_rect;
static SDL_Texture* glyphs[16][8][8];
extern uint32_t zoom_factor;

#define SHIFT_Y(n) ((GLYPH_HEIGHT + GLYPH_MARGIN) * zoom_factor * (n))
#define SHIFT_X(n) ((n) * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor)

bool init_textures(SDL_Renderer* renderer, uint32_t factor) {
  SDL_Surface* outlines_raw = IMG_Load(RESOURCES_DIR "card_outlines.png");
  if (!outlines_raw) return false;
  SDL_Surface* outlines_surface = zoom(outlines_raw, factor);
  outlines = SDL_CreateTextureFromSurface(renderer, outlines_surface);
  SDL_FreeSurface(outlines_raw);

  SDL_Surface* buttons_raw = IMG_Load(RESOURCES_DIR "buttons.png");
  if (!buttons_raw) return false;
  SDL_Surface* buttons_surface = zoom(buttons_raw, factor);
  buttons = SDL_CreateTextureFromSurface(renderer, buttons_surface);
  SDL_FreeSurface(buttons_raw);

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
      SDL_Surface* tmp_surface = SDL_CreateRGBSurface(
        0,
        CARD_WIDTH * factor,
        CARD_HEIGHT * factor,
        32,
        RMASK,
        GMASK,
        BMASK,
        AMASK);

      SDL_Rect num_src = {
        .x = NUM_WIDTH * (7 - num) * factor,
        .y = color < 2 ? NUM_HEIGHT * factor : 0,
        .w = NUM_WIDTH * factor,
        .h = NUM_HEIGHT * factor};

      SDL_Rect color_src = {
        .x = (num == 7 ? 2 : num > 3 ? 1 : 0) * factor * COLOR_WIDTH,
        .y = color * COLOR_HEIGHT * factor,
        .w = COLOR_WIDTH * factor,
        .h = COLOR_HEIGHT * factor};

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
    0x606060ff, // dark gray
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
        SDL_Surface* tmp_surface = SDL_CreateRGBSurface(
          0,
          GLYPH_WIDTH * factor,
          GLYPH_HEIGHT * factor,
          32,
          RMASK,
          GMASK,
          BMASK,
          AMASK);
        SDL_Rect src = {
          .x = GLYPH_WIDTH * factor * x,
          .y = GLYPH_HEIGHT * factor * y,
          .w = GLYPH_WIDTH * factor,
          .h = GLYPH_HEIGHT * factor};
        SDL_Rect dst = {.x = 0, .y = 0, .w = GLYPH_WIDTH * factor, .h = GLYPH_HEIGHT * factor};
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
  SDL_DestroyTexture(buttons);
}

SDL_Texture* get_texture(CardColor color, uint8_t number) {
  if (number >= 8) return NULL;
  return cards[color][number];
}

SDL_Rect get_card_rect() {
  SDL_Rect res = card_rect;
  return res;
}

void render_card(
  SDL_Renderer* renderer,
  CardColor color,
  uint8_t number,
  uint8_t state,
  SDL_Rect* dst) {
  SDL_Rect outline_src = {
    .x = color == VOIDCARD ? card_rect.w * 4 : card_rect.w * state,
    .y = 0,
    .w = card_rect.w,
    .h = card_rect.h};
  SDL_Rect card_src = get_card_rect();

  SDL_RenderCopy(renderer, outlines, &outline_src, dst);
  if (state < 2 && color != VOIDCARD)
    SDL_RenderCopy(renderer, get_texture(color, number), &card_src, dst);
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
  SDL_Rect src = {.x = 0, .y = 0, .w = GLYPH_WIDTH * zoom_factor, .h = GLYPH_HEIGHT * zoom_factor};
  SDL_Rect dst = {.x = x, .y = y, .w = GLYPH_WIDTH * zoom_factor, .h = GLYPH_HEIGHT * zoom_factor};

  size_t index = 63;
  if ((glyph >= 'a' && glyph <= 'z') || (glyph >= 'A' && glyph <= 'Z')) {
    index = (size_t)(toupper(glyph) - 'A');
  } else if (glyph >= '0' && glyph <= '9') {
    index = 26 + glyph - '0';
  } else if (glyph >= -127 && glyph < -63) {
    index = glyph + 128;
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
      case '(':
        index = 52;
        break;
      case '>':
      case ')':
        index = 53;
        break;
      case '"':
        index = 54;
        break;
      case '^':
        index = 56;
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

void render_deck(
  SDL_Renderer* renderer,
  Player* player,
  uint32_t x,
  uint32_t y,
  int selected_card) {
  uint32_t sx = x - (CARD_WIDTH + DECK_PADDING) * zoom_factor * player->n_cards / 2
                + DECK_PADDING * zoom_factor / 2;
  for (size_t n = 0; n < player->n_cards; n++) {
    SDL_Rect dst_rect = {
      .x = sx + (CARD_WIDTH + DECK_PADDING) * n * zoom_factor,
      .y = y + DECK_PADDING * zoom_factor,
      .w = CARD_WIDTH * zoom_factor,
      .h = CARD_HEIGHT * zoom_factor};
    render_card(
      renderer,
      player->cards[n].type,
      player->cards[n].value - 7,
      selected_card == n,
      &dst_rect);
  }
}

int get_hovered_card(
  Player* player,
  uint32_t deck_x,
  uint32_t deck_y,
  int32_t mouse_x,
  int32_t mouse_y) {
  uint32_t sx = deck_x - (CARD_WIDTH + DECK_PADDING) * zoom_factor * player->n_cards / 2
                + DECK_PADDING * zoom_factor / 2;
  uint32_t ex = deck_x + (CARD_WIDTH + DECK_PADDING) * zoom_factor * player->n_cards / 2
                - DECK_PADDING * zoom_factor / 2;
  if (mouse_x < sx) return -1;
  if (mouse_x > ex) return -1;
  if (
    mouse_y < deck_y + DECK_PADDING * zoom_factor
    || mouse_y > deck_y + (CARD_HEIGHT + DECK_PADDING) * zoom_factor)
    return -1;
  for (size_t n = 0; n < player->n_cards; n++) {
    if (
      mouse_x >= sx + (CARD_WIDTH + DECK_PADDING) * n * zoom_factor
      && mouse_x <= sx + (CARD_WIDTH + DECK_PADDING) * n * zoom_factor + CARD_WIDTH * zoom_factor) {
      return n;
    }
  }
  return -1;
}

void render_button(
  SDL_Renderer* renderer,
  char* text,
  uint32_t x,
  uint32_t y,
  bool state,
  uint8_t color) {
  x -= GLYPH_WIDTH * zoom_factor;
  y -= GLYPH_HEIGHT * zoom_factor;
  {
    SDL_Rect src_rect = {
      .x = (GLYPH_WIDTH * 3 + GLYPH_MARGIN) * state * zoom_factor,
      .y = 0,
      .w = GLYPH_WIDTH * zoom_factor,
      .h = GLYPH_HEIGHT * 3 * zoom_factor
    };
    SDL_Rect dst_rect = {
      .x = x,
      .y = y,
      .w = GLYPH_WIDTH * zoom_factor,
      .h = GLYPH_HEIGHT * 3 * zoom_factor
    };
    SDL_RenderCopy(renderer, buttons, &src_rect, &dst_rect);
  }

  uint32_t x2 = x + GLYPH_WIDTH * zoom_factor;
  char* text2 = text;

  while (*(text2++)) {
    SDL_Rect src_rect = {
      .x = (GLYPH_WIDTH + (GLYPH_WIDTH * 3 + GLYPH_MARGIN) * state) * zoom_factor,
      .y = 0,
      .w = (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor,
      .h = GLYPH_HEIGHT * 3 * zoom_factor
    };
    SDL_Rect dst_rect = {
      .x = x2,
      .y = y,
      .w = (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor,
      .h = GLYPH_HEIGHT * 3 * zoom_factor
    };
    SDL_RenderCopy(renderer, buttons, &src_rect, &dst_rect);

    x2 += (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor;
  }

  SDL_Rect src_rect = {
    .x = (GLYPH_WIDTH * 2 + GLYPH_MARGIN + (GLYPH_WIDTH * 3 + GLYPH_MARGIN) * state) * zoom_factor,
    .y = 0,
    .w = GLYPH_WIDTH * zoom_factor,
    .h = GLYPH_HEIGHT * 3 * zoom_factor
  };
  SDL_Rect dst_rect = {
    .x = x2,
    .y = y,
    .w = GLYPH_WIDTH * zoom_factor,
    .h = GLYPH_HEIGHT * 3 * zoom_factor
  };
  SDL_RenderCopy(renderer, buttons, &src_rect, &dst_rect);

  render_text(renderer, text, x + GLYPH_WIDTH * zoom_factor, y + GLYPH_HEIGHT * zoom_factor, color);
}

bool is_button_hovered(
  char* text,
  uint32_t button_x,
  uint32_t button_y,
  int32_t mouse_x,
  int32_t mouse_y) {
  int32_t width = (GLYPH_WIDTH * 2 + (GLYPH_MARGIN + GLYPH_WIDTH) * strlen(text) - 2) * zoom_factor;
  button_x -= GLYPH_WIDTH * zoom_factor;
  button_y -= GLYPH_HEIGHT * zoom_factor;
  return mouse_x >= button_x + zoom_factor * 2 && mouse_y >= button_y + zoom_factor * 2 && mouse_x <= button_x + width
         && mouse_y <= button_y + GLYPH_HEIGHT * zoom_factor * 3 - zoom_factor * 2;
}

void render_round(SDL_Renderer* renderer, Game* game, uint32_t x, uint32_t y) {
  for (size_t n = 0; n < 4; n++) {
#define SELECT(a, b, c, d) (n < 2 ? (n == 0 ? (a) : (b)) : (n == 2 ? (c) : (d)))
    SDL_Rect dst_rect = {
      .x = x + SELECT(CARD_WIDTH, 0, CARD_WIDTH, CARD_WIDTH * 2) * zoom_factor
           - (int)(1.5 * CARD_WIDTH * zoom_factor),
      .y = y + SELECT(CARD_HEIGHT * 2, CARD_HEIGHT, 0, CARD_HEIGHT) * zoom_factor
           - (int)(1.5 * CARD_HEIGHT * zoom_factor),
      .w = CARD_WIDTH * zoom_factor,
      .h = CARD_HEIGHT * zoom_factor};
    render_card(renderer, game->pli[n].type, game->pli[n].value - 7, 0, &dst_rect);
  }
#undef SELECT
}

void render_card_anim(
  SDL_Renderer* renderer,
  Game* game,
  uint32_t x,
  uint32_t y,
  size_t player,
  Card card,
  float distance) {
#define SELECT(a, b, c, d) (player < 2 ? (player == 0 ? (a) : (b)) : (player == 2 ? (c) : (d)))
  SDL_Rect dst_rect = {
    .x = x
      + (int)(SELECT(CARD_WIDTH, -distance, CARD_WIDTH, CARD_WIDTH * 2 + distance) * zoom_factor)
      - (int)(1.5 * CARD_WIDTH * zoom_factor),
    .y = y
      + (int)(SELECT(CARD_HEIGHT * 2 + distance, CARD_HEIGHT, -distance, CARD_HEIGHT) * zoom_factor)
      - (int)(1.5 * CARD_HEIGHT * zoom_factor),
    .w = CARD_WIDTH * zoom_factor,
    .h = CARD_HEIGHT * zoom_factor};
  render_card(renderer, card.type, card.value - 7, 0, &dst_rect);
#undef SELECT
}

void render_ai_deck(SDL_Renderer* renderer, Player* ai, uint32_t x, uint32_t y) {
  for (size_t n = 0; n < 8; n++) {
    SDL_Rect dst_rect = {
      .x = x + CARD_WIDTH * (n % 4) * zoom_factor / 2,
      .y = y + CARD_HEIGHT * (n / 4) * zoom_factor / 2,
      .w = CARD_WIDTH * zoom_factor / 2,
      .h = CARD_HEIGHT * zoom_factor / 2};
    render_card(
      renderer,
      n < ai->n_cards ? ai->cards[n].type : VOIDCARD,
      ai->cards[n].value - 7,
      !(ai->cards_revealed & (1 << n)) * 2,
      &dst_rect);
  }
}

void render_score(SDL_Renderer* renderer, size_t player_index, int score, uint32_t x, uint32_t y) {
  char* player_index_str =
    player_index < 2 ? (player_index == 0 ? "S:" : "W:") : (player_index == 2 ? "N:" : "E:");
  render_text(renderer, player_index_str, x, y, 9);
  char score_str[7];
  sprintf(score_str, "%dpts", score);
  render_text(renderer, score_str, x + SHIFT_X(2), y, 9);
}

extern uint32_t window_width;
extern uint32_t window_height;

void render_all(SDL_Renderer* renderer, Game* game, int hovered_card, size_t current_player, bool render_extras) {
  SDL_SetRenderDrawColor(renderer, BG_RED, BG_GREEN, BG_BLUE, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  uint32_t deck_x = window_width / 2;
  uint32_t deck_y = window_height - (CARD_HEIGHT + DECK_PADDING) * 4;

  render_deck(renderer, &game->players[0], deck_x, deck_y, hovered_card);
  if (render_extras) render_round(renderer, game, window_width / 2, window_height / 2);
  render_ai_deck(renderer, &game->players[1], 0, window_height / 2 - CARD_HEIGHT * 2);
  render_ai_deck(renderer, &game->players[2], window_width / 2 - CARD_WIDTH * 4, 0);
  render_ai_deck(
    renderer,
    &game->players[3],
    window_width - CARD_WIDTH * 8,
    window_height / 2 - CARD_HEIGHT * 2);

  // Scores
  if (render_extras) {
    render_score(
      renderer,
      0,
      game->players[0].trick_points_total,
      deck_x + 2 * zoom_factor
        + (CARD_WIDTH + DECK_PADDING) * (game->players[0].n_cards) * zoom_factor / 2,
      window_height - GLYPH_HEIGHT * zoom_factor - 2 * zoom_factor);
    render_score(
      renderer,
      1,
      game->players[1].trick_points_total,
      2 * zoom_factor,
      window_height / 2 - CARD_HEIGHT * 2 - 2 * zoom_factor - GLYPH_HEIGHT * zoom_factor);
    render_score(
      renderer,
      2,
      game->players[2].trick_points_total,
      window_width / 2 + 2 * zoom_factor + CARD_WIDTH * zoom_factor,
      2 * zoom_factor);
    render_score(
      renderer,
      3,
      game->players[3].trick_points_total,
      window_width - 2 * zoom_factor - (GLYPH_WIDTH + GLYPH_MARGIN) * 8 * zoom_factor,
      window_height / 2 - CARD_HEIGHT * 2 - 2 * zoom_factor - GLYPH_HEIGHT * zoom_factor);
  }

  if (current_player == 0) {
    render_text(
      renderer,
      YOUR_TURN_MSG,
      window_width / 2
        - (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor * (strlen(YOUR_TURN_MSG) - 1) / 2,
      deck_y - GLYPH_HEIGHT * 2 * zoom_factor,
      15);
  }

  uint32_t info_x = GLYPH_MARGIN * zoom_factor;
  uint32_t info_y =
    window_height - (GLYPH_HEIGHT + GLYPH_MARGIN) * zoom_factor * 3 - CARD_HEIGHT * 2 * zoom_factor;

  // Extras - contract, coinched, etc.
  if (render_extras) {
    // Contracted team
    render_text(renderer, CONTRACTED_TEAM_MSG, info_x, info_y, 8);
    if (game->contracted_team == NS) {
      render_text(renderer, "N/S", info_x + SHIFT_X(strlen(CONTRACTED_TEAM_MSG)), info_y, 0);
    } else {
      render_text(renderer, "E/W", info_x + SHIFT_X(strlen(CONTRACTED_TEAM_MSG)), info_y, 0);
    }

    // Active trump
    if (game->active_trump < 4) {
      render_text(renderer, ACTIVE_TRUMP_MSG, info_x, info_y + SHIFT_Y(1), 8);
      char str[2] = {128 + 37 + game->active_trump, 0};
      render_text(
        renderer,
        str,
        info_x + SHIFT_X(strlen(ACTIVE_TRUMP_MSG)),
        info_y + SHIFT_Y(1),
        game->active_trump < 2 ? 12 : 0);
    } else if (game->active_trump == 4) {
      render_text(renderer, ALLTRUMP_MSG, info_x, info_y + SHIFT_Y(1), 8);
    } else {
      render_text(renderer, NOTRUMP_MSG, info_x, info_y + SHIFT_Y(1), 8);
    }

    bool coinched = game->active_contract == COINCHE || game->active_contract == SURCOINCHE;

    // Contract kind
    if (game->active_contract == CAPOT || (coinched && game->contract_points == CAPOT_POINTS)) {
      render_text(renderer, GOAL_MSG, info_x, info_y + SHIFT_Y(2), 8);
      render_text(renderer, CAPOT_MSG, info_x + SHIFT_X(strlen(GOAL_MSG)), info_y + SHIFT_Y(2), 0);
    } else if (
      game->active_contract == GENERAL || (coinched && game->contract_points == GENERAL_POINTS)) {
      render_text(renderer, GOAL_MSG, info_x, info_y + SHIFT_Y(2), 8);
      render_text(renderer, GENERAL_MSG, info_x + SHIFT_X(strlen(GOAL_MSG)), info_y + SHIFT_Y(2), 0);
      char* general_attacker_str = game->general_attacker < 2
                                    ? (game->general_attacker == 0 ? "S" : "W")
                                    : (game->general_attacker == 2 ? "N" : "E");
      render_text(
        renderer,
        general_attacker_str,
        info_x + SHIFT_X(strlen(GOAL_MSG) + strlen(GENERAL_MSG)),
        info_y + SHIFT_Y(2),
        12);
      render_text(
        renderer,
        ")",
        info_x + SHIFT_X(strlen(GOAL_MSG) + strlen(GENERAL_MSG) + 1),
        info_y + SHIFT_Y(2),
        0);
    } else if (game->active_contract == CHOSENCOLOUR || coinched) {
      render_text(renderer, CHOSENCOLOUR_MSG, info_x, info_y + SHIFT_Y(2), 8);
      char points_str[4];
      sprintf(points_str, "%d", game->contract_points);
      render_text(
        renderer,
        points_str,
        info_x + SHIFT_X(strlen(CHOSENCOLOUR_MSG)),
        info_y + SHIFT_Y(2),
        0);
    }

    // Coinche
    if (game->active_contract == COINCHE) {
      render_text(renderer, COINCHE_MSG, info_x, info_y + SHIFT_Y(3), 8);
    } else if (game->active_contract == SURCOINCHE) {
      render_text(renderer, SURCOINCHE_MSG, info_x, info_y + SHIFT_Y(3), 8);
    }
  }
}

void render_bids(SDL_Renderer* renderer, Game* game) {
  if (game->active_contract == -1) return;
#define PRINT_CENTER(text, y, color) render_text(renderer, (text), window_width / 2 - strlen(text) * (GLYPH_WIDTH + GLYPH_MARGIN) * zoom_factor / 2, y, color)
  char attacker_str[32] = {0};
  char contract_str[32] = {0};
  char* pos_str = game->general_attacker < 2 ? (game->general_attacker == 0 ? "South" : "West") : (game->general_attacker == 2 ? "North" : "East");
  sprintf(attacker_str, "Player %s bid:", pos_str);
  bool coinched = (game->active_contract == COINCHE || game->active_contract == SURCOINCHE);
  if (game->active_contract == CAPOT || (coinched && game->contract_points == CAPOT_POINTS)) {
    if (game->active_trump < 4) {
      sprintf(contract_str, "capot as %c", 128 + 37 + game->active_trump);
    } else if (game->active_trump == ALLTRUMP) {
      sprintf(contract_str, "capot, all trump");
    } else {
      sprintf(contract_str, "capot, no trump");
    }
  } else if (game->active_contract == GENERAL || (coinched && game->contract_points == GENERAL_POINTS)) {
    if (game->active_trump < 4) {
      sprintf(contract_str, "general as %c", 128 + 37 + game->active_trump);
    } else if (game->active_trump == ALLTRUMP) {
      sprintf(contract_str, "general, all trump");
    } else {
      sprintf(contract_str, "general, no trump");
    }
  } else {
    if (game->active_trump < 4) {
      sprintf(contract_str, "%d points as %c", game->contract_points, 128 + 37 + game->active_trump);
    } else if (game->active_trump == ALLTRUMP) {
      sprintf(contract_str, "%d points, all trump", game->contract_points);
    } else {
      sprintf(contract_str, "%d points, no trump", game->contract_points);
    }
  }
  PRINT_CENTER(attacker_str, window_height / 2 + SHIFT_Y(-2), 0);
  PRINT_CENTER(contract_str, window_height / 2 + SHIFT_Y(-1), 0);
  if (game->active_contract == COINCHE) {
    PRINT_CENTER("coinche", window_height / 2 + SHIFT_Y(-1), 0);
  } else if (game->active_contract == SURCOINCHE) {
    PRINT_CENTER("surcoinche", window_height / 2 + SHIFT_Y(-1), 0);
  }
}
