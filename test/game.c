#include "game.h"
#include <stdlib.h>

BEGIN_TEST(game) {
  EXECUTE_TEST(play_card, "Test that play_card correctly plays a card");
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
    .pli = {{.type = CLOVERS, .value = 7}, {.type = CLOVERS, .value = 8}, {.type = CLOVERS, .value = 9}, {.type = VOIDCARD}}
  };
  play_card(&game, 0, 0);
  ASSERT_EQ_PRI(1, game.players[0].n_cards, "%d");
  ASSERT_EQ_PRI(11, game.players[0].cards[0].value, "%d");
  ASSERT_EQ_PRI(HEARTS, game.pli[3].type, "%d");
  ASSERT_EQ_PRI(7, game.pli[3].value, "%d");
}
END_TEST()
