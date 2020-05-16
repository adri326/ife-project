#include "rules.h"

// TODO: dedupe
// TODO: more verbose argument naming?

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
      int base_points = player1.trick_points_total + player2.trick_points_total
        + player1.declaration_points + player2.declaration_points
        + player1.belote + player2.belote + game.contract_points;
      int declaration_points =
        looser1.declaration_points + looser2.declaration_points;
      // case: the team with the contract realizes it and gets more than 82
      // points, computing the points of this team
      switch (game.active_contract) {
        case SURCOINCHE:
          total = 4 * base_points + declaration_points;
          break;
        case COINCHE:
          total = 2 * base_points + declaration_points;
          break;
        default:
          total = base_points + declaration_points;
      }
    } else {
      int base_points = 162 + game.contract_points + player1.declaration_points
        + player2.declaration_points + looser1.declaration_points
        + looser2.declaration_points + player1.belote + player2.belote;
      // case: the team with the contract doesn't realize it and we compute the
      // points of the defending team
      switch (game.active_contract) {
        case SURCOINCHE:
          total = 4 * base_points;
          break;
        case COINCHE:
          total = 2 * base_points;
          break;
        default:
          total = base_points;
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

Teams defenders_win(
  Teams team_to_switch) { // selects the opposite team of the one entered
  Teams to_return;
  switch (team_to_switch) {
    case EW:
      to_return = NS;
      break;
    case NS:
      to_return = EW;
      break;
  }
  return to_return;
}

Teams attackers_win(Teams team_to_switch) { // selects the team entered
  Teams to_return;
  switch (team_to_switch) {
    case EW:
      to_return = EW;
      break;
    case NS:
      to_return = NS;
      break;
  }
  return team_to_switch;
}

Teams contract_check(Game game) {
  int attacker1, attacker2, defender1, defender2;
  Teams winners;
  switch (game.contracted_team) // to easily manipulate the different player
                                // structures afterwards
  {
    case EW:
      attacker1 = 1;
      attacker2 = 3;
      defender1 = 0;
      defender2 = 2;
      break;
    case NS:
      attacker1 = 0;
      attacker2 = 2;
      defender1 = 1;
      defender2 = 3;
      break;
  }
  if (
    game.players[attacker1].trick_points_total
      + game.players[attacker2].trick_points_total
    < 82) { // the team under contract didn't manage to win 82 points or more,
            // the other team instantly wins
    winners = defenders_win(game.contracted_team);
  } else {
    switch (game.active_contract) {
      case CHOSENCOLOUR: // the attackers need to have reached their goal amount
                         // of points, stocked in contract_points
        if (
          game.players[attacker1].trick_points_total
            + game.players[attacker2].trick_points_total
            + game.players[attacker1].belote + game.players[attacker2].belote
            + game.players[attacker1].declaration_points
            + game.players[attacker2].declaration_points
          < game.contract_points) {
          winners = defenders_win(game.contracted_team);
        } else {
          winners = attackers_win(game.contracted_team);
        }
        break;
      case CAPOT: // the attackers need to have won all tricks, so the two
                  // attackers together must have won 8 tricks
        if (
          game.players[attacker1].tricks_won
            + game.players[attacker2].tricks_won
          == 8) {
          winners = attackers_win(game.contracted_team);
        } else {
          winners = defenders_win(game.contracted_team);
        }
        break;
      case GENERAL: // a single player needs to have won the 8 tricks. Their
                    // number is in general_attacker
        if (game.players[game.general_attacker].tricks_won == 8) {
          winners = attackers_win(game.contracted_team);
        } else {
          winners = defenders_win(game.contracted_team);
        }
        break;
      case COINCHE: // the defender team must not have filled their contract for
                    // the coinche to realize.
        // the contract to verify corresponds to the points in contract_points:
        // 250 = capot, 500 = general, other = default = classic point goal
        // contract
        switch (game.contract_points) {
          case 250:
            if (
              game.players[defender1].tricks_won
                + game.players[defender2].tricks_won
              == 8) {
              winners = defenders_win(game.contracted_team);
            } else {
              winners = attackers_win(game.contracted_team);
            }
            break;
          case 500:
            if (game.players[game.general_attacker].tricks_won == 8) {
              winners = defenders_win(game.contracted_team);
            } else {
              winners = attackers_win(game.contracted_team);
            }
            break;
          default:
            if (
              game.players[defender1].trick_points_total
                + game.players[defender2].trick_points_total
                + game.players[defender1].belote
                + game.players[defender2].belote
                + game.players[defender1].declaration_points
                + game.players[defender2].declaration_points
              < game.contract_points) {
              winners = attackers_win(game.contracted_team);
            } else {
              winners = defenders_win(game.contracted_team);
            }
        }
        break;
      case SURCOINCHE: // exactly the same than the coinche but this time the
                       // attacker team must have filled their contract for the
                       // surcoinche to be realized
        switch (game.contract_points) {
          case 250:
            if (
              game.players[attacker1].tricks_won
                + game.players[attacker2].tricks_won
              == 8) {
              winners = attackers_win(game.contracted_team);
            } else {
              winners = defenders_win(game.contracted_team);
            }
            break;
          case 500:
            if (game.players[game.general_attacker].tricks_won == 8) {
              winners = attackers_win(game.contracted_team);
            } else {
              winners = defenders_win(game.contracted_team);
            }
            break;
          default:
            if (
              game.players[attacker1].trick_points_total
                + game.players[attacker2].trick_points_total
                + game.players[attacker1].belote
                + game.players[attacker2].belote
                + game.players[attacker1].declaration_points
                + game.players[attacker2].declaration_points
              < game.contract_points) {
              winners = defenders_win(game.contracted_team);
            } else {
              winners = attackers_win(game.contracted_team);
            }
        }
        break;
    }
  }
  return winners;
}

int trick_points(Card card1, Card card2, Card card3, Card card4, Game game) {
  int total; // to return
  total = card_value(card1, game) + card_value(card2, game)
    + card_value(card3, game) + card_value(card4, game);
  // As announced, this function is a simple sum.
  // The main part of this step in the game is
  // coded in the function card_value.
  if (game.players[0].n_cards == 0) { // To take into account the "10 of der".
    total = total + 10;
  }
  return total;
}

int card_value(Card card, Game game) {
  int value; // to return
  switch (game.active_trump) // values depend mainly of the trump
  {
    case ALLTRUMP:
      switch (card.value) {
        case 9:
          value = 9; // 9
          break;
        case 10:
          value = 5; // 10
          break;
        case 11:
          value = 14; // Jack
          break;
        case 12:
          value = 1; // Queen
          break;
        case 13:
          value = 3; // King
          break;
        case 14:
          value = 6; // Ace
          break;
        default:
          value = 0; // 7 and 8
      }
      break;
    case NOTRUMP:
      switch (card.value) {
        case 10:
          value = 10; // 10
          break;
        case 11:
          value = 2; // Jack
          break;
        case 12:
          value = 3; // Queen
          break;
        case 13:
          value = 4; // King
          break;
        case 14:
          value = 19; // Ace
          break;
        default:
          value = 0; // 7, 8 and 9
      }
      break;
    default: // for all the chosen colour trumps
      if (card.type == game.active_trump) { // We have to make a difference
                                            // between trump and non trump
                                            // cards.
        switch (card.value) {
          case 9:
            value = 14; // 9
            break;
          case 10:
            value = 10; // 10
            break;
          case 11:
            value = 20; // Jack
            break;
          case 12:
            value = 3; // Queen
            break;
          case 13:
            value = 4; // King
            break;
          case 14:
            value = 11; // Ace
            break;
          default:
            value = 0; // 7 and 8
        }
      } else {
        switch (card.value) {
          case 10:
            value = 10; // 10
            break;
          case 11:
            value = 2; // Jack
            break;
          case 12:
            value = 3; // Queen
            break;
          case 13:
            value = 4; // King
            break;
          case 14:
            value = 11; // Ace
            break;
          default:
            value = 0; // 7, 8 and 9
        }
      }
  }
  return value;
}
