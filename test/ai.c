#include "ai.h"
#include <stdlib.h>

BEGIN_TEST(ai) {
  EXECUTE_TEST(can_ai_win, "Check that can_ai_win() correctly reports winning or loosing situations")
}
END_TEST()

BEGIN_TEST(can_ai_win) {
  for (TrumpColor trump = 0; trump < 4; trump++) {
    Player ai = {
      .cards = {{.type = HEARTS, .value = 7}},
      .n_cards = 1
    };
    Game game = {
      .players = {{}, ai, {}, {}},
      .pli = {{.type = CLOVERS, .value = 11}, {.type = VOIDCARD}, {.type = VOIDCARD}, {.type = VOIDCARD}},
      .active_trump = trump,
      .trick_color = CLOVERS,
      .trick_cut = false,
    };
    ASSERT_EQ_MSG(trump == TRUMP_HEARTS, can_ai_win(&game, 1), trump == TRUMP_HEARTS ? "Expected player 1 to be able to win with their trump card." : "Expected player 1 to be unable to win the trick.");
  }
}
{
  for (CardColor color = 0; color < 4; color++) {
    Player ai = {
      .cards = {{.type = color, .value = 13}},
      .n_cards = 1
    };
    Game game = {
      .players = {{}, ai, {}, {}},
      .pli = {{.type = color, .value = 11}, {.type = VOIDCARD}, {.type = VOIDCARD}, {.type = VOIDCARD}},
      .active_trump = (TrumpColor)(color + 1) % 4,
      .trick_color = color,
      .trick_cut = false,
    };
    ASSERT_MSG(can_ai_win(&game, 1),"Expected player 1 to be able to win with their card.");
  }
}
END_TEST()
