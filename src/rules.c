#include "rules.h"

// TODO: dedupe
// TODO: more verbose argument naming?

int turn_points(Game game, Teams active_team) {
  Player* player_1 = &game.players[active_team == NS ? 0 : 1];
  Player* player_2 = &game.players[active_team == NS ? 2 : 3];
  Player* looser_1 = &game.players[game.winning_team == EW ? 0 : 1];
  Player* looser_2 = &game.players[game.winning_team == EW ? 2 : 3];

  if (active_team == game.winning_team) {
    if (active_team == game.contracted_team) {
      int base_points = player_1->trick_points_total + player_2->trick_points_total
        + player_1->declaration_points + player_2->declaration_points + player_1->belote
        + player_2->belote + game.contract_points;
      int declaration_points = looser_1->declaration_points + looser_2->declaration_points;
      // case: the team with the contract realizes it and gets more than 82
      // points, computing the points of this team
      switch (game.active_contract) {
        case SURCOINCHE:
          return 4 * base_points + declaration_points;
        case COINCHE:
          return 2 * base_points + declaration_points;
        default:
          return base_points;
      }
    } else {
      int base_points = 162 + game.contract_points + player_1->declaration_points
        + player_2->declaration_points + looser_1->declaration_points + looser_2->declaration_points
        + player_1->belote + player_2->belote;
      // case: the team with the contract doesn't realize it and we compute the
      // points of the defending team
      switch (game.active_contract) {
        case SURCOINCHE:
          return 4 * base_points;
        case COINCHE:
          return 2 * base_points;
        default:
          return base_points;
      }
    }
  } else {
    if (active_team == game.contracted_team) {
      // case: the team with the contract doesn't realize it and we compute
      // their points
      return player_1->belote + player_2->belote;
    } else {
      // case: the team with the contract realizes it and gets more than 82
      // points, computing the points of the defending team
      switch (game.active_contract) {
        case SURCOINCHE:
          return player_1->belote + player_2->belote;
        case COINCHE:
          return player_1->belote + player_2->belote;
        default:
          return player_1->trick_points_total + player_2->trick_points_total
            + player_1->declaration_points + player_2->declaration_points + player_1->belote
            + player_2->belote;
      }
    }
  }
}

Teams contract_check(Game game) {
  Player* attacker_1;
  Player* attacker_2;
  Player* defender_1;
  Player* defender_2;

  // to easily manipulate the different player structures afterwards
  switch (game.contracted_team) {
    case EW:
      attacker_1 = &game.players[1];
      attacker_2 = &game.players[3];
      defender_1 = &game.players[0];
      defender_2 = &game.players[2];
      break;
    case NS:
      attacker_1 = &game.players[0];
      attacker_2 = &game.players[2];
      defender_1 = &game.players[1];
      defender_2 = &game.players[3];
      break;
  }
  if (attacker_1->trick_points_total + attacker_2->trick_points_total < 82) {
    // the team under contract didn't manage to win 82 points or more,
    // the other team instantly wins
    return defenders_win(game.contracted_team);
  } else {
    switch (game.active_contract) {
      case CHOSENCOLOUR: // the attackers need to have reached their goal amount
                         // of points, stocked in contract_points
        if (
          attacker_1->trick_points_total + attacker_2->trick_points_total + attacker_1->belote
            + attacker_2->belote + attacker_1->declaration_points + attacker_2->declaration_points
          < game.contract_points) {
          return defenders_win(game.contracted_team);
        } else {
          return attackers_win(game.contracted_team);
        }
        break;
      case CAPOT: // the attackers need to have won all tricks, so the two
                  // attackers together must have won 8 tricks
        if (attacker_1->tricks_won + attacker_2->tricks_won == 8) {
          return attackers_win(game.contracted_team);
        } else {
          return defenders_win(game.contracted_team);
        }
        break;
      case GENERAL: // a single player needs to have won the 8 tricks. Their
                    // number is in general_attacker
        if (game.players[game.general_attacker].tricks_won == 8) {
          return attackers_win(game.contracted_team);
        } else {
          return defenders_win(game.contracted_team);
        }
        break;
      case COINCHE: // the defender team must not have filled their contract for
                    // the coinche to realize.
        // the contract to verify corresponds to the points in contract_points:
        // 250 = capot, 500 = general, other = default = classic point goal
        // contract
        switch (game.contract_points) {
          case 250:
            if (defender_1->tricks_won + defender_2->tricks_won == 8) {
              return defenders_win(game.contracted_team);
            } else {
              return attackers_win(game.contracted_team);
            }
            break;
          case 500:
            if (game.players[game.general_attacker].tricks_won == 8) {
              return defenders_win(game.contracted_team);
            } else {
              return attackers_win(game.contracted_team);
            }
            break;
          default:
            if (
              defender_1->trick_points_total + defender_2->trick_points_total + defender_1->belote
                + defender_2->belote + defender_1->declaration_points
                + defender_2->declaration_points
              < game.contract_points) {
              return attackers_win(game.contracted_team);
            } else {
              return defenders_win(game.contracted_team);
            }
        }
        break;
      case SURCOINCHE: // exactly the same than the coinche but this time the
                       // attacker team must have filled their contract for the
                       // surcoinche to be realized
        switch (game.contract_points) {
          case 250:
            if (attacker_1->tricks_won + attacker_2->tricks_won == 8) {
              return attackers_win(game.contracted_team);
            } else {
              return defenders_win(game.contracted_team);
            }
            break;
          case 500:
            if (game.players[game.general_attacker].tricks_won == 8) {
              return attackers_win(game.contracted_team);
            } else {
              return defenders_win(game.contracted_team);
            }
            break;
          default:
            if (
              attacker_1->trick_points_total + attacker_2->trick_points_total + attacker_1->belote
                + attacker_2->belote + attacker_1->declaration_points
                + attacker_2->declaration_points
              < game.contract_points) {
              return defenders_win(game.contracted_team);
            } else {
              return attackers_win(game.contracted_team);
            }
        }
        break;
    }
  }

  return NS; // should not be reached
}

int trick_points(Card card1, Card card2, Card card3, Card card4, Game game) {
  int total; // to return
  total = card_value(card1, game) + card_value(card2, game) + card_value(card3, game)
    + card_value(card4, game);
  // As announced, this function is a simple sum.
  // The main part of this step in the game is
  // coded in the function card_value.
  if (game.players[0].n_cards == 0) { // To take into account the "10 of der".
    total = total + 10;
  }
  return total;
}

int card_value(Card card, Game game) {
  switch (game.active_trump) // values depend mainly of the trump
  {
    case ALLTRUMP:
      switch (card.value) {
        case 9:
          return 9; // 9
        case 10:
          return 5; // 10
        case 11:
          return 14; // Jack
        case 12:
          return 1; // Queen
        case 13:
          return 3; // King
        case 14:
          return 6; // Ace
        default:
          return 0; // 7 and 8
      }
      break;
    case NOTRUMP:
      switch (card.value) {
        case 10:
          return 10; // 10
        case 11:
          return 2; // Jack
        case 12:
          return 3; // Queen
        case 13:
          return 4; // King
        case 14:
          return 19; // Ace
        default:
          return 0; // 7, 8 and 9
      }
      break;
    default: // for all the chosen colour trumps
      if ((int)card.type == (int)game.active_trump) { // We have to make a difference
                                            // between trump and non trump
                                            // cards.
        switch (card.value) {
          case 9:
            return 14; // 9
          case 10:
            return 10; // 10
          case 11:
            return 20; // Jack
          case 12:
            return 3; // Queen
          case 13:
            return 4; // King
          case 14:
            return 11; // Ace
          default:
            return 0; // 7 and 8
        }
      } else {
        switch (card.value) {
          case 10:
            return 10; // 10
          case 11:
            return 2; // Jack
          case 12:
            return 3; // Queen
          case 13:
            return 4; // King
          case 14:
            return 11; // Ace
          default:
            return 0; // 7, 8 and 9
        }
      }
  }
}
