#include "ai.h"
#include "game.h"

bool ai_turn(Game* game, size_t player) {
  if (
    game->pli[(player + 1) % 4].type == VOIDCARD && game->pli[(player + 2) % 4].type == VOIDCARD
    && game->pli[(player + 3) % 4].type == VOIDCARD) {
    // if the AI is first to play, it will play a random strong card
    size_t card_index;
    int i = 0;
    do {
      card_index = (int)rand() % game->players[player].n_cards;
      i++;
      // we limit the number of loops in case the ai
      // does not have strong cards. It is faster than
      // checking what cards were already checked each time.
    } while (card_value(game->players[player].cards[card_index], *game) == 0 && i < 25);

    return play_card(game, player, card_index);
  } else if (can_ai_win(game, player)) {
    size_t card = ai_choose_winning_card(game, player);
    if (card >= game->players[player].n_cards) card = game->players[player].n_cards - 1;
    return play_card(game, player, card);
  } else {
    size_t card = ai_choose_weakest_card(game, player);
    if (card >= game->players[player].n_cards) card = game->players[player].n_cards - 1;
    return play_card(game, player, card);
  }
}

bool can_ai_win(Game* game, size_t player) {
  bool wins = false;
  for (int i = 0; i < game->players[player].n_cards; i++) {
    if (
      move_check(game, game->players[player].cards[i], player)
      && card_wins(game, game->players[player].cards[i], player)) {
      wins = true;
    }
  } // we check if any card in the hand can win
  return wins;
}

size_t ai_choose_winning_card(Game* game, size_t player) {
  // ai can win, we look for the lowest value card so that it does
  Player* curr_player = &game->players[player];
  size_t best_card_index = 0;
  for (size_t i = 1; i < 8; i++) {
    Card curr_card = curr_player->cards[i];
    Card curr_best_card = curr_player->cards[best_card_index];
    if (!move_check(game, curr_card, player) || !card_wins(game, curr_card, player)) continue;

    if (card_value(curr_card, *game) < card_value(curr_best_card, *game)) {
      // if card can't win, we don't even consider it.
      // if it can and have a lowest value, we play it.
      best_card_index = i;
    } else if (
      card_value(curr_card, *game) == card_value(curr_best_card, *game)
      && curr_card.value < curr_best_card.value) {
      // for 7,8 and sometimes 9, value is 0, we have to make a difference
      // don't need to check when value is higher, won't change best_card_index
      best_card_index = i;
      // no need to check when value is higher, won't change
    } // best_card_index. No need to check when equal because it means
    // both cards can be played, having the smallest value yet.
  }
  return best_card_index;
}

size_t ai_choose_weakest_card(Game* game, size_t player) {
  // ai can't win, we just look for the lowest value card
  Player* curr_player = &game->players[player];
  size_t best_card_index = -1;

  for (size_t i = 1; i < curr_player->n_cards; i++) {
    Card curr_card = curr_player->cards[i];
    Card curr_best_card = curr_player->cards[best_card_index];
    if (!move_check(game, curr_card, player)) continue;

    if (card_value(curr_card, *game) < card_value(curr_best_card, *game)) {
      // the card has a lower value
      best_card_index = i;
    } else if (
      card_value(curr_card, *game) == card_value(curr_best_card, *game)
      && curr_card.value < curr_best_card.value) {
      // for 7,8 and sometimes 9, value is 0, we have to make a difference
      // don't need to check when value is higher, won't change best_card_index
      best_card_index = i;
      // no need to check when value is higher, won't change
      // best_card_index. No need to check when equal because it means
    } // both cards can be played, having the smallest value yet.
  }

  if (best_card_index == -1) {
    // no good card to play; play any playable card. TODO: sort cards?
    for (size_t n = 0; n < curr_player->n_cards; n++) {
      if (move_check(game, curr_player->cards[n], player)) return n;
    }
  }

  return best_card_index;
}

void ai_announce_contract(Game* game, size_t player) {
  int number_stong_cards;
  for (int j = 0; j < 4; j++) {
    // we test every card color one by one
    number_stong_cards = 0;
    for (int i = 0; i < 8; i++) {
      // we check how many strong cards the ai has
      if (
        game->players[player].cards[i].value > 8 && (int)game->players[player].cards[i].type == j) {
        number_stong_cards = number_stong_cards + 1;
      }
    }
    if (number_stong_cards > 3 && game->contract_points < 120) {
      // we announce the contract depending on the hand
      game->active_contract = CHOSENCOLOUR;
      game->contract_points = 120;
      game->active_trump = (TrumpColor)j;
      game->contracted_team = (Teams)player % 2;
      game->general_attacker = player;
    } else if (number_stong_cards == 3 && game->contract_points < 80) {
      game->active_contract = CHOSENCOLOUR;
      game->contract_points = 80;
      game->active_trump = (TrumpColor)j;
      game->contracted_team = (Teams)player % 2;
      game->general_attacker = player;
    }
  }
}
