#include "rules.h"

BEGIN_TEST(turn_points) {
  Player player_1 = {
    .trick_points_total = 60,
    .declaration_points = 0,
    .tricks_won = 3,
    .belote = 0
  };
  Player player_2 = {
    .trick_points_total = 40,
    .declaration_points = 0,
    .tricks_won = 2,
    .belote = 0
  };
  Player player_3 = {
    .trick_points_total = 40,
    .declaration_points = 0,
    .tricks_won = 2,
    .belote = 20
  };
  Player player_4 = {
    .trick_points_total = 15,
    .declaration_points = 20,
    .tricks_won = 1,
    .belote = 0
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = SURCOINCHE,
    .contract_points = 400,
    .general_attacker = 0,
    .active_trump = HEART,
    .winning_team = EW,
    .contracted_team = EW,
  };
  int final_score_ew = turn_points(game, player_1, player_3, EW, player_2, player_4);
  ASSERT_EQ_MSG(2100, final_score_ew, "Check the final score of the E/W team (expected 2100, got %d)", final_score_ew);
  int final_score_ns = turn_points(game, player_2, player_4, NS, player_2, player_4);
  ASSERT_EQ_MSG(0, final_score_ns, "Check the final score of the N/S team (expected 0, got %d)", final_score_ns);
}
{
  Player player_1 = {
    .trick_points_total = 60,
    .declaration_points = 0,
    .tricks_won = 3,
    .belote = 0
  };
  Player player_2 = {
    .trick_points_total = 40,
    .declaration_points = 0,
    .tricks_won = 2,
    .belote = 0
  };
  Player player_3 = {
    .trick_points_total = 40,
    .declaration_points = 0,
    .tricks_won = 2,
    .belote = 20
  };
  Player player_4 = {
    .trick_points_total = 15,
    .declaration_points = 20,
    .tricks_won = 1,
    .belote = 0
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = CHOSENCOLOUR,
    .contract_points = 100,
    .general_attacker = 0,
    .active_trump = HEART,
    .winning_team = EW,
    .contracted_team = EW,
  };
  int final_score_ew = turn_points(game, player_1, player_3, EW, player_1, player_3);
  ASSERT_EQ_MSG(220, final_score_ew, "Check the final score of the E/W team (expected 220, got %d)", final_score_ew);
  int final_score_ns = turn_points(game, player_2, player_4, NS, player_1, player_3);
  ASSERT_EQ_MSG(75, final_score_ns, "Check the final score of the N/S team (expected 0, got %d)", final_score_ns);
}
END_TEST()
