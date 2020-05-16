#include "rules.h"

BEGIN_TEST(turn_points) {
  Player player_1 = {
    .trick_points_total = 60,
    .tricks_won = 3,
  };
  Player player_2 = {
    .trick_points_total = 40,
    .tricks_won = 2,
  };
  Player player_3 = {
    .trick_points_total = 40,
    .tricks_won = 2,
    .belote = 20
  };
  Player player_4 = {
    .trick_points_total = 15,
    .declaration_points = 20,
    .tricks_won = 1,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = SURCOINCHE,
    .contract_points = 400,
    .active_trump = HEART,
    .winning_team = NS,
    .contracted_team = NS,
  };
  int final_score_ew = turn_points(game, player_1, player_3, NS, player_2, player_4);
  ASSERT_EQ_MSG(2100, final_score_ew, "Check the final score of the E/W team (expected 2100, got %d)", final_score_ew);
  int final_score_ns = turn_points(game, player_2, player_4, EW, player_2, player_4);
  ASSERT_EQ_MSG(0, final_score_ns, "Check the final score of the N/S team (expected 0, got %d)", final_score_ns);
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
  Player player_3 = {
    .trick_points_total = 40,
    .tricks_won = 2,
    .belote = 20
  };
  Player player_4 = {
    .trick_points_total = 15,
    .declaration_points = 20,
    .tricks_won = 1,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = CHOSENCOLOUR,
    .contract_points = 100,
    .active_trump = HEART,
    .winning_team = NS,
    .contracted_team = NS,
  };
  int final_score_ew = turn_points(game, player_1, player_3, NS, player_1, player_3);
  ASSERT_EQ_MSG(220, final_score_ew, "Check the final score of the E/W team (expected 220, got %d)", final_score_ew);
  int final_score_ns = turn_points(game, player_2, player_4, EW, player_1, player_3);
  ASSERT_EQ_MSG(75, final_score_ns, "Check the final score of the N/S team (expected 0, got %d)", final_score_ns);
}
END_TEST()

BEGIN_TEST(contract_check) {
  Player player_1 = {
    .trick_points_total = 50,
    .tricks_won = 3,
  };
  Player player_2 = {
    .trick_points_total = 40,
    .tricks_won = 2,
  };
  Player player_3 = {
    .trick_points_total = 40,
    .tricks_won = 2,
    .belote = 20
  };
  Player player_4 = {
    .trick_points_total = 15,
    .declaration_points = 20,
    .tricks_won = 1,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = CHOSENCOLOUR,
    .contract_points = 100,
    .active_trump = HEART,
    .winning_team = NS,
    .contracted_team = NS,
  };
  Teams winners = contract_check(game);
  ASSERT_EQ_MSG(NS, winners, "Check the winner of a fulfilled CHOSENCOLOUR game (expected NS, got %s)", (winners == NS ? "NS" : "EW"));
  game.active_contract = COINCHE;
  // game.contract_points *= 2;
  winners = contract_check(game);
  ASSERT_EQ_MSG(NS, winners, "Check the winner of a non-fulfilled coinched CHOSENCOLOUR game (expected NS, got %s)", (winners == NS ? "NS" : "EW"));
}
{
  Player player_1 = {
    .trick_points_total = 10,
    .tricks_won = 1,
  };
  Player player_2 = {
    .trick_points_total = 110,
    .tricks_won = 4,
  };
  Player player_3 = {
    .trick_points_total = 40,
    .tricks_won = 2,
    .belote = 20
  };
  Player player_4 = {
    .trick_points_total = 15,
    .declaration_points = 20,
    .tricks_won = 1,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = CHOSENCOLOUR,
    .contract_points = 100,
    .active_trump = HEART,
    .winning_team = NS,
    .contracted_team = NS,
  };
  Teams winners = contract_check(game);
  ASSERT_EQ_MSG(EW, winners, "Check the winner of a non-fulfilled CHOSENCOLOUR game (expected EW, got %s)", (winners == NS ? "NS" : "EW"));
  game.active_contract = COINCHE;
  // game.contract_points *= 2;
  winners = contract_check(game);
  ASSERT_EQ_MSG(EW, winners, "Check the winner of a fulfilled coinched CHOSENCOLOUR game (expected EW, got %s)", (winners == NS ? "NS" : "EW"));
}
{
  Player player_1 = {
    .trick_points_total = 140,
    .tricks_won = 5,
  };
  Player player_2 = {
    .trick_points_total = 0,
    .tricks_won = 0,
  };
  Player player_3 = {
    .trick_points_total = 70,
    .tricks_won = 3,
    .belote = 20
  };
  Player player_4 = {
    .trick_points_total = 0,
    .declaration_points = 20,
    .tricks_won = 0,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = CAPOT,
    .contract_points = 250,
    .active_trump = HEART,
    .winning_team = NS,
    .contracted_team = NS,
  };
  Teams winners = contract_check(game);
  ASSERT_EQ_MSG(NS, winners, "Check the winner of a fulfilled CAPOT game (expected NS, got %s)", (winners == NS ? "NS" : "EW"));
}
{
  Player player_1 = {
    .trick_points_total = 100,
    .tricks_won = 4,
  };
  Player player_2 = {
    .trick_points_total = 30,
    .tricks_won = 1,
  };
  Player player_3 = {
    .trick_points_total = 70,
    .tricks_won = 3,
    .belote = 20
  };
  Player player_4 = {
    .trick_points_total = 0,
    .declaration_points = 20,
    .tricks_won = 0,
  };
  Game game = {
    .players = {player_1, player_2, player_3, player_4},
    .active_contract = CAPOT,
    .contract_points = 250,
    .active_trump = HEART,
    .winning_team = NS,
    .contracted_team = NS,
  };
  Teams winners = contract_check(game);
  ASSERT_EQ_MSG(EW, winners, "Check the winner of a non-fulfilled CAPOT game (expected EW, got %s)", (winners == NS ? "NS" : "EW"));
}
END_TEST()

// TODO: all of the other cases
