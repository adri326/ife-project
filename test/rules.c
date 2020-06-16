#include "rules.h"
#include <stdlib.h>

BEGIN_TEST(rules) {
  EXECUTE_TEST(turn_points, "Test the turn_points() function");
  EXECUTE_TEST(contract_check, "Test the contract_check() function");
  EXECUTE_TEST(card_value, "Test that card_value() returns the right card value");
  EXECUTE_TEST(trick_points, "Test that trick_points() returns the sum of 4 card_value()");
  EXECUTE_TEST(trump_equality, "Check that the equality between CardType and TrumpColor is valid");
  EXECUTE_TEST(move_check, "Test that move_check() lets you play the right cards");
  EXECUTE_TEST(leader_trick, "Test that leader_trick() correctly returns which player is leading");
}
END_TEST()

BEGIN_TEST(turn_points) {
  Player player_1 = {
    .trick_points_total = 60,
    .tricks_won = 3,
  };
  Player player_2 = {
    .trick_points_total = 40,
    .tricks_won = 2,
  };
  Player player_3 = {.trick_points_total = 40, .tricks_won = 2, .belote = 20};
  Player player_4 = {
    .trick_points_total = 15,
    .declaration_points = 20,
    .tricks_won = 1,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = SURCOINCHE,
    .contract_points = 100,
    .active_trump = TRUMP_HEARTS,
    .winning_team = NS,
    .contracted_team = NS,
  };
  int final_score_ew = turn_points(game, NS);
  ASSERT_EQ_MSG(
    900,
    final_score_ew,
    "Check the final score of the E/W team (expected 900, got %d)",
    final_score_ew);
  int final_score_ns = turn_points(game, EW);
  ASSERT_EQ_MSG(
    0,
    final_score_ns,
    "Check the final score of the N/S team (expected 0, got %d)",
    final_score_ns);
}
{
  Player player_1 = {
    .trick_points_total = 60,
    .tricks_won = 3,
  };
  Player player_2 = {
    .trick_points_total = 40,
    .tricks_won = 2,
  };
  Player player_3 = {.trick_points_total = 40, .tricks_won = 2, .belote = 20};
  Player player_4 = {
    .trick_points_total = 15,
    .declaration_points = 20,
    .tricks_won = 1,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = CHOSENCOLOUR,
    .contract_points = 100,
    .active_trump = TRUMP_HEARTS,
    .winning_team = NS,
    .contracted_team = NS,
  };
  int final_score_ew = turn_points(game, NS);
  ASSERT_EQ_MSG(
    220,
    final_score_ew,
    "Check the final score of the E/W team (expected 220, got %d)",
    final_score_ew);
  int final_score_ns = turn_points(game, EW);
  ASSERT_EQ_MSG(
    75,
    final_score_ns,
    "Check the final score of the N/S team (expected 0, got %d)",
    final_score_ns);
}
END_TEST()

BEGIN_TEST(contract_check) {
  // CHOSENCOLOUR: NS fulfills it
  Player player_1 = {
    .trick_points_total = 50,
    .tricks_won = 3,
  };
  Player player_2 = {
    .trick_points_total = 40,
    .tricks_won = 2,
  };
  Player player_3 = {.trick_points_total = 40, .tricks_won = 2, .belote = 20};
  Player player_4 = {
    .trick_points_total = 15,
    .declaration_points = 20,
    .tricks_won = 1,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = CHOSENCOLOUR,
    .contract_points = 100,
    .active_trump = TRUMP_HEARTS,
    .winning_team = NS,
    .contracted_team = NS,
  };
  Teams winners = contract_check(game);
  ASSERT_EQ_MSG(
    NS,
    winners,
    "Check the winner of a fulfilled CHOSENCOLOUR game (expected NS, got %s)",
    (winners == NS ? "NS" : "EW"));
  game.active_contract = COINCHE;
  winners = contract_check(game);
  ASSERT_EQ_MSG(
    NS,
    winners,
    "Check the winner of a non-fulfilled coinched CHOSENCOLOUR game (expected "
    "NS, got %s)",
    (winners == NS ? "NS" : "EW"));
}
{
  // CHOSENCOLOUR: NS does not fulfills it
  Player player_1 = {
    .trick_points_total = 10,
    .tricks_won = 1,
  };
  Player player_2 = {
    .trick_points_total = 110,
    .tricks_won = 4,
  };
  Player player_3 = {.trick_points_total = 40, .tricks_won = 2, .belote = 20};
  Player player_4 = {
    .trick_points_total = 15,
    .declaration_points = 20,
    .tricks_won = 1,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = CHOSENCOLOUR,
    .contract_points = 100,
    .active_trump = TRUMP_HEARTS,
    .winning_team = NS,
    .contracted_team = NS,
  };
  Teams winners = contract_check(game);
  ASSERT_EQ_MSG(
    EW,
    winners,
    "Check the winner of a non-fulfilled CHOSENCOLOUR game (expected EW, got "
    "%s)",
    (winners == NS ? "NS" : "EW"));
  game.active_contract = COINCHE;
  winners = contract_check(game);
  ASSERT_EQ_MSG(
    EW,
    winners,
    "Check the winner of a fulfilled coinched CHOSENCOLOUR game (expected EW, "
    "got %s)",
    (winners == NS ? "NS" : "EW"));
}
{
  // CAPOT: NS fulfills it
  Player player_1 = {
    .trick_points_total = 140,
    .tricks_won = 5,
  };
  Player player_2 = {
    .trick_points_total = 0,
    .tricks_won = 0,
  };
  Player player_3 = {.trick_points_total = 70, .tricks_won = 3, .belote = 20};
  Player player_4 = {
    .trick_points_total = 0,
    .declaration_points = 20,
    .tricks_won = 0,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = CAPOT,
    .contract_points = 250,
    .active_trump = TRUMP_HEARTS,
    .winning_team = NS,
    .contracted_team = NS,
  };
  Teams winners = contract_check(game);
  ASSERT_EQ_MSG(
    NS,
    winners,
    "Check the winner of a fulfilled CAPOT game (expected NS, got %s)",
    (winners == NS ? "NS" : "EW"));
  game.active_contract = COINCHE;
  game.contracted_team = EW;
  winners = contract_check(game);
  ASSERT_EQ_MSG(
    NS,
    winners,
    "Check the winner of a non-fulfilled coinched CAPOT game (expected NS, got "
    "%s)",
    (winners == NS ? "NS" : "EW"));
}
{
  // CAPOT: NS does not fulfills it
  Player player_1 = {
    .trick_points_total = 100,
    .tricks_won = 4,
  };
  Player player_2 = {
    .trick_points_total = 30,
    .tricks_won = 1,
  };
  Player player_3 = {.trick_points_total = 70, .tricks_won = 3, .belote = 20};
  Player player_4 = {
    .trick_points_total = 0,
    .declaration_points = 20,
    .tricks_won = 0,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = CAPOT,
    .contract_points = 250,
    .active_trump = TRUMP_HEARTS,
    .winning_team = NS,
    .contracted_team = NS,
  };
  Teams winners = contract_check(game);
  ASSERT_EQ_MSG(
    EW,
    winners,
    "Check the winner of a non-fulfilled CAPOT game (expected EW, got %s)",
    (winners == NS ? "NS" : "EW"));
  game.active_contract = COINCHE;
  winners = contract_check(game);
  ASSERT_EQ_MSG(
    NS,
    winners,
    "Check the winner of a fulfilled coinched CAPOT game (expected NS, got %s)",
    (winners == NS ? "NS" : "EW"));
}
{
  // GENERAL: player_1 fulfills it
  Player player_1 = {
    .trick_points_total = 320,
    .tricks_won = 8,
  };
  Player player_2 = {
    .trick_points_total = 0,
    .tricks_won = 0,
  };
  Player player_3 = {.trick_points_total = 70, .tricks_won = 0, .belote = 20};
  Player player_4 = {
    .trick_points_total = 0,
    .declaration_points = 20,
    .tricks_won = 0,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = GENERAL,
    .contract_points = 250,
    .active_trump = TRUMP_HEARTS,
    .general_attacker = 0,
    .winning_team = NS,
    .contracted_team = NS,
  };
  Teams winners = contract_check(game);
  ASSERT_EQ_MSG(
    NS,
    winners,
    "Check the winner of a fulfilled GENERAL game (expected NS, got %s)",
    (winners == NS ? "NS" : "EW"));
  game.active_contract = COINCHE;
  game.contracted_team = EW;
  winners = contract_check(game);
  ASSERT_EQ_MSG(
    NS,
    winners,
    "Check the winner of a non-fulfilled coinched GENERAL game (expected NS, "
    "got %s)",
    (winners == NS ? "NS" : "EW"));
}
{
  // GENERAL: player_1 does not fulfills it
  Player player_1 = {
    .trick_points_total = 100,
    .tricks_won = 4,
  };
  Player player_2 = {
    .trick_points_total = 30,
    .tricks_won = 1,
  };
  Player player_3 = {.trick_points_total = 70, .tricks_won = 3, .belote = 20};
  Player player_4 = {
    .trick_points_total = 0,
    .declaration_points = 20,
    .tricks_won = 0,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = GENERAL,
    .contract_points = 250,
    .active_trump = TRUMP_HEARTS,
    .general_attacker = 0,
    .winning_team = NS,
    .contracted_team = NS,
  };
  Teams winners = contract_check(game);
  ASSERT_EQ_MSG(
    EW,
    winners,
    "Check the winner of a non-fulfilled GENERAL game (expected EW, got %s)",
    (winners == NS ? "NS" : "EW"));
  game.active_contract = COINCHE;
  winners = contract_check(game);
  ASSERT_EQ_MSG(
    NS,
    winners,
    "Check the winner of a fulfilled coinched GENERAL game (expected NS, got "
    "%s)",
    (winners == NS ? "NS" : "EW"));
}
END_TEST()

DECL_TEST(card_value_alltrump)
DECL_TEST(card_value_notrump)
DECL_TEST(card_value_trump)
DECL_TEST(card_value_not_trump)

BEGIN_TEST(card_value) {
  EXECUTE_TEST(card_value_alltrump, "Test the card_value() function against an ALLTRUMP game");
  EXECUTE_TEST(card_value_notrump, "Test the card_value() function against a NOTRUMP game");
  EXECUTE_TEST(
    card_value_trump, "Test the card_value() function against a HEARTS game and HEARTS cards");
  EXECUTE_TEST(
    card_value_not_trump, "Test the card_value() function against a HEARTS game and CLOVERS cards");
}
END_TEST()

#define CARD_VALUE_TEST(name, act_trump, card_type, ...)                                           \
  BEGIN_TEST(name) {                                                                               \
    int values[8] = {__VA_ARGS__};                                                                 \
    Game game = {.active_trump = (int)act_trump};                                                  \
    for (int n = 7; n < 15; n++) {                                                                 \
      Card card = {.value = n, .type = card_type};                                                 \
      ASSERT_EQ_PRI(values[n - 7], card_value(card, game), "%d");                                  \
    }                                                                                              \
  }                                                                                                \
  END_TEST()

CARD_VALUE_TEST(card_value_alltrump, ALLTRUMP, HEARTS, 0, 0, 9, 5, 14, 1, 3, 6)
CARD_VALUE_TEST(card_value_notrump, NOTRUMP, HEARTS, 0, 0, 0, 10, 2, 3, 4, 19)
CARD_VALUE_TEST(card_value_trump, HEARTS, HEARTS, 0, 0, 14, 10, 20, 3, 4, 11)
CARD_VALUE_TEST(card_value_not_trump, HEARTS, CLOVERS, 0, 0, 0, 10, 2, 3, 4, 11)

BEGIN_TEST(trick_points) {
  // 50 random tests
  for (int n = 0; n < 50; n++) {
    Game game = {.active_trump = rand() % 6, .players = {{.n_cards = rand() % 2}}};
    Card card1 = {.value = 7 + rand() % 8, .type = rand() % 4};
    Card card2 = {.value = 7 + rand() % 8, .type = rand() % 4};
    Card card3 = {.value = 7 + rand() % 8, .type = rand() % 4};
    Card card4 = {.value = 7 + rand() % 8, .type = rand() % 4};
    ASSERT_EQ_PRI(
      card_value(card1, game) + card_value(card2, game) + card_value(card3, game)
        + card_value(card4, game) + (game.players[0].n_cards == 0 ? 10 : 0),
      trick_points(card1, card2, card3, card4, game),
      "%d");
  }
}
END_TEST()

BEGIN_TEST(trump_equality) {
  ASSERT_EQ_MSG((int)HEARTS, (int)TRUMP_HEARTS, "HEARTS == TRUMP_HEARTS");
  ASSERT_EQ_MSG((int)TILES, (int)TRUMP_TILES, "TILES == TRUMP_TILES");
  ASSERT_EQ_MSG((int)CLOVERS, (int)TRUMP_CLOVERS, "CLOVERS == TRUMP_CLOVERS");
  ASSERT_EQ_MSG((int)SPIKES, (int)TRUMP_SPIKES, "SPIKES == TRUMP_SPIKES");
  ASSERT_EQ_MSG((int)HEARTS, 0, "HEARTS == 0");
  ASSERT_EQ_MSG((int)TILES, 1, "TILES == 1");
  ASSERT_EQ_MSG((int)CLOVERS, 2, "CLOVERS == 2");
  ASSERT_EQ_MSG((int)SPIKES, 3, "SPIKES == 3");
}
END_TEST()

BEGIN_TEST(move_check) {
  for (TrumpColor trump = 0; trump < 6; trump++) {
    Game game = {.active_trump = trump, .trick_cut = false, .trick_leader_position = 0};
    for (size_t n = 0; n < 4; n++) {
      game.pli[n].type = VOIDCARD;
    }
    for (CardColor color = 0; color < 4; color++) {
      for (uint8_t value = 7; value < 15; value++) {
        Card card = {
          .type = color,
          .value = value
        };
        ASSERT_MSG(
          move_check(&game, card, 0),
          "Expected card (%d, %d) to be playable as the first player, no matter what!",
          color,
          value);
      }
    }
  }
}
END_TEST()

BEGIN_TEST(leader_trick) {
  size_t order[8] = {7, 8, 9, 11, 12, 13, 10, 14};
  for (TrumpColor trump = 0; trump < 4; trump++) {
    for (uint8_t value = 0; value < 7; value++) {
      {
        Game game = {
          .active_trump = (trump + 1) % 4,
          .trick_cut = false,
          .trick_leader_position = 0,
          .trick_color = (int)trump,
          .pli = {{.type = (int)trump, .value = order[value]}, {.type = (int)trump, .value = order[value + 1]}}
        };
        ASSERT_EQ_MSG(1, leader_trick(&game, 1), "Expected player 1 to be the trick leader, got player 0. Trump(%d), CardValue(%zu // %zu)", (trump + 1) % 4, order[value], order[value + 1]);
      }
      {
        Game game = {
          .active_trump = (trump + 1) % 4,
          .trick_cut = false,
          .trick_leader_position = 0,
          .trick_color = (int)trump,
          .pli = {{.type = (int)trump, .value = order[value + 1]}, {.type = (int)trump, .value = order[value]}}
        };
        ASSERT_EQ_MSG(0, leader_trick(&game, 1), "Expected player 0 to be the trick leader, got player 1. Trump(%d), CardValue(%zu // %zu)", (trump + 1) % 4, order[value + 1], order[value]);
      }
    }
  }
}
{
  size_t order[8] = {7, 8, 12, 13, 10, 14, 9, 11};
  for (TrumpColor trump = 0; trump < 4; trump++) {
    for (uint8_t value = 0; value < 7; value++) {
      {
        Game game = {
          .active_trump = trump,
          .trick_cut = false,
          .trick_leader_position = 0,
          .trick_color = (int)trump,
          .pli = {{.type = (int)trump, .value = order[value]}, {.type = (int)trump, .value = order[value + 1]}}
        };
        ASSERT_EQ_MSG(1, leader_trick(&game, 1), "Expected player 1 to be the trick leader, got player 0. Trump(%d), CardValue(%zu // %zu)", trump, order[value], order[value + 1]);
      }
      {
        Game game = {
          .active_trump = trump,
          .trick_cut = false,
          .trick_leader_position = 0,
          .trick_color = (int)trump,
          .pli = {{.type = (int)trump, .value = order[value + 1]}, {.type = (int)trump, .value = order[value]}}
        };
        ASSERT_EQ_MSG(0, leader_trick(&game, 1), "Expected player 0 to be the trick leader, got player 1. Trump(%d), CardValue(%zu // %zu)", trump, order[value + 1], order[value]);
      }
    }
  }
}
{
  size_t order[8] = {7, 8, 9, 11, 12, 13, 10, 14};
  for (TrumpColor trump = 0; trump < 4; trump++) {
    for (uint8_t value = 0; value < 7; value++) {
      {
        Game game = {
          .active_trump = (trump + 1) % 4,
          .trick_leader_position = 0,
          .trick_color = (int)trump,
          .pli = {{.type = (int)trump, .value = order[value]}, {.type = ((int)trump + 1) % 4, .value = 7}},
          .trick_cut = true,
        };
        ASSERT_EQ_MSG(1, leader_trick(&game, 1), "Expected player 1 to be the trick leader, got player 0. Trump(%d), CardValue(%zu // %d)", (trump + 1) % 4, order[value], 7);
      }
      {
        Game game = {
          .active_trump = (trump + 1) % 4,
          .trick_leader_position = 0,
          .trick_color = ((int)trump + 1) % 4,
          .pli = {{.type = ((int)trump + 1) % 4, .value = 7}, {.type = (int)trump, .value = order[value]}},
          .trick_cut = false,
        };
        ASSERT_EQ_MSG(0, leader_trick(&game, 1), "Expected player 0 to be the trick leader, got player 1. Trump(%d), CardValue(%d // %zu)", (trump + 1) % 4, 7, order[value]);
      }
    }
  }
}
END_TEST()

// TODO: all of the other cases
