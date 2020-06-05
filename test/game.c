#include "game.h"
#include <stdlib.h>

BEGIN_TEST(game) {
  EXECUTE_TEST(play_card, "Test that play_card correctly plays a card");
  EXECUTE_TEST(game_turn, "Test that game_turn makes every player play their card");
}
END_TEST()

BEGIN_TEST(play_card) {
  Player player_1 = {
    .n_cards = 2,
    .cards = {{.type = HEARTS, .value = 7}, {.type = HEARTS, .value = 11}}
  };
  Player player_2 = {
    .n_cards = 1,
    .cards = {{.type = HEARTS, .value = 8}}
  };
  Player player_3 = {
    .n_cards = 1,
    .cards = {{.type = HEARTS, .value = 9}}
  };
  Player player_4 = {
    .n_cards = 1,
    .cards = {{.type = HEARTS, .value = 10}}
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .pli = {{.type = VOIDCARD}, {.type = CLOVERS, .value = 7}, {.type = CLOVERS, .value = 8}, {.type = CLOVERS, .value = 9}}
  };
  play_card(&game, 0, 0);
  ASSERT_EQ_PRI(1, game.players[0].n_cards, "%d");
  ASSERT_EQ_PRI(HEARTS, game.players[0].cards[0].type, "Type(%d)");
  ASSERT_EQ_PRI(11, game.players[0].cards[0].value, "Value(%d)");
  ASSERT_EQ_PRI(HEARTS, game.pli[0].type, "Type(%d)");
  ASSERT_EQ_PRI(7, game.pli[0].value, "Value(%d)");
}
END_TEST()

BEGIN_TEST(game_turn) {
  // Assert that after a game_turn, all players have played a card
  Player player_1 = {
    .n_cards = 1,
    .cards = {{.type = HEARTS, .value = 7}}
  };
  Player player_2 = {
    .n_cards = 1,
    .cards = {{.type = HEARTS, .value = 8}}
  };
  Player player_3 = {
    .n_cards = 1,
    .cards = {{.type = HEARTS, .value = 9}}
  };
  Player player_4 = {
    .n_cards = 1,
    .cards = {{.type = HEARTS, .value = 10}}
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .pli = {{.type = VOIDCARD}, {.type = VOIDCARD}, {.type = VOIDCARD}, {.type = VOIDCARD}},
    .trick_leader_position = 0
  };
  Player players[4] = {player_1, player_2, player_3, player_4};
  ASSERT_EQ_MSG(true, game_turn(&game), "Game_turn returned false instead of true.");
  for (size_t n = 0; n < 4; n++) {
    ASSERT_EQ_MSG(0, game.players[0].n_cards, "Expected player %zu to have 0 cards, got %d", n, game.players[0].n_cards);
    ASSERT_EQ_PRI(players[n].cards[0].type, game.pli[n].type, "Type(%d)");
    ASSERT_EQ_PRI(players[n].cards[0].value, game.pli[n].value, "Value(%d)");
  }
}
END_TEST()
