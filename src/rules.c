#include "rules.h"

// TODO: dedupe

int turn_points(
  Game game,
  Player player1,
  Player player2,
  Teams active_team,
  Player looser1,
  Player looser2) {
  // variable that will be returned and corresponds to the turn points we want
  // to compute
  int total = 0;
  if (active_team == game.winning_team) {
    if (active_team == game.contracted_team) {
      // case: the team with the contract realizes it and gets more than 82
      // points, computing the points of this team
      switch (game.active_contract) {
        case SURCOINCHE:
          total = 4
              * (player1.trick_points_total + player2.trick_points_total
                 + player1.declaration_points + player2.declaration_points
                 + player1.belote + player2.belote + game.contract_points)
            + looser1.declaration_points + looser2.declaration_points;
          break;
        case COINCHE:
          total = 2
              * (player1.trick_points_total + player2.trick_points_total
                 + player1.declaration_points + player2.declaration_points
                 + player1.belote + player2.belote + game.contract_points)
            + looser1.declaration_points + looser2.declaration_points;
          break;
        default:
          total = player1.trick_points_total + player2.trick_points_total
            + player1.declaration_points + player2.declaration_points
            + player1.belote + player2.belote + game.contract_points;
      }
    } else {
      // case: the team with the contract doesn't realize it and we compute the
      // points of the defending team
      switch (game.active_contract) {
        case SURCOINCHE:
          total = 4
            * (162 + game.contract_points + player1.declaration_points
               + player2.declaration_points + looser1.declaration_points
               + looser2.declaration_points + player1.belote + player2.belote);
          break;
        case COINCHE:
          total = 2
            * (162 + game.contract_points + player1.declaration_points
               + player2.declaration_points + looser1.declaration_points
               + looser2.declaration_points + player1.belote + player2.belote);
          break;
        default:
          total = 162 + game.contract_points + player1.declaration_points
            + player2.declaration_points + looser1.declaration_points
            + looser2.declaration_points + player1.belote + player2.belote;
      }
    }
  } else {
    if (active_team == game.contracted_team) {
      // case: the team with the contract doesn't realize it and we compute
      // their points
      total = player1.belote + player2.belote;
    } else {
      // case: the team with the contract realizes it and gets more than 82
      // points, computing the points of the defending team
      switch (game.active_contract) {
        case SURCOINCHE:
          total = player1.belote + player2.belote;
          break;
        case COINCHE:
          total = player1.belote + player2.belote;
          break;
        default:
          total = player1.trick_points_total + player2.trick_points_total
            + player1.declaration_points + player2.declaration_points
            + player1.belote + player2.belote;
      }
    }
  }

  return total;
}
