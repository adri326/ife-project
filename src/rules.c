#include "rules.h"
#include <stdlib.h>
#include <stdbool.h>

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
                        + player_1->declaration_points + player_2->declaration_points
                        + player_1->belote + player_2->belote + game.contract_points;
      int declaration_points = looser_1->declaration_points + looser_2->declaration_points;
      // case: the team with the contract realizes it and gets more than 82 points, computing the points of this team
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
                        + player_2->declaration_points + looser_1->declaration_points
                        + looser_2->declaration_points + player_1->belote + player_2->belote;
      // case: the team with the contract doesn't realize it and we compute the points of the defending team
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
      // case: the team with the contract doesn't realize it and we compute their points
      return player_1->belote + player_2->belote;
    } else {
      // case: the team with the contract realizes it and gets more than 82 points, computing the points of the defending team
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

  // To easily manipulate the different player structures afterwards
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
    // The team under contract didn't manage to win 82 points or more, the other team instantly wins
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
      case COINCHE: // the defender team must not have filled their contract for the coinche to realize.
        // The contract to verify corresponds to the points in contract_points:
        // 250 = capot, 500 = general, other = default = classic point goal contract
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
      case SURCOINCHE:
        // exactly the same than the coinche but this time the
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
  // The main part of this step in the game is handled in the function card_value.
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
    default: // for all the chosen color trumps
      if (
        (int)card.type
        == (int)game.active_trump) { // We handle trump and non trump cards diffrently.
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
            return 0; // 7, 8 and 9 + VOIDCARD/0 unused spot
        }
      }
  }
}

int move_check(Game* game, Card card, size_t player_index) {
  Player* player = &game->players[player_index];

  bool was_anything_played = false;
  for (size_t n = 0; n < 4; n++) {
    if (game->pli[n].type != VOIDCARD) {
      was_anything_played = true;
      break;
    }
  }
  if (!was_anything_played) return 1;

  if (game->active_trump == ALLTRUMP || game->active_trump == NOTRUMP) { // no cut possible
    if (card.type == game->trick_color) {
      return true; // easiest case, everything is good
    } else { // wrong color, we need to check the hand first
      for (int i = 0; i < 8; i++) {
        if (player->cards[i].type == game->trick_color) { return 0; }
      }
      return 1;
    }
  } else { // if the trick can be cut
    if (game->trick_cut) { // the trick was cut, only trump matters
      if ((int)card.type == (int)game->active_trump) {
        // the player may play with a trump-colored card
        return 1;
      } else {
        bool has_trump_card = true;
        // the player may only play another color if they cannot play a trump card (?)
        for (int i = 0; i < 8; i++) {
          if ((int)player->cards[i].type == (int)game->active_trump) { has_trump_card = false; }
        }
        return has_trump_card;
      }
    } else {
      // the trick was not cut yet, the player may decide to cut
      if (card.type == game->trick_color) { // the player plays the right color, everything is ok
        return 1;
      } else {
        bool has_trick_color = true;
        // the player may only play a card of the other color if they have no more cards of the trick color in their hand
        for (int i = 0; i < 8; i++) {
          if (player->cards[i].type == game->trick_color) { has_trick_color = false; }
        }
        if (has_trick_color) {
          return 0;
        } else { // the player doesn't have the right color
          if ((int)card.type == (int)game->active_trump) { // player cuts
            return 2;
          } else {
            // the player may play another color only if they cannot cut already
            bool can_cut = false;
            for (int i = 0; i < 8; i++) { // no need to reset check, it is =1 in this case
              if ((int)player->cards[i].type == (int)game->active_trump) { can_cut = true; }
            }
            if (can_cut) {
              // The player may only play a non-trump if their partner is leading the round
              return game->trick_leader_position == (player_index + 2) % 4;
            } else {
              // the player has no trump in hand, nothing more to ask
              return 1;
            }
          }
        }
      }
    }
  }
}

int leader_trick(Game game, int position, int current_leader_position, int* cut) {
  Card* card1 = &game.pli[(position + 1) % 4]; // to check either it is
  Card* card2 = &game.pli[(position + 2) % 4]; // the first move of the trick
  Card* card3 = &game.pli[(position + 3) % 4]; // or not
  Card* player_card = &game.pli[position];
  Card* leader_card = &game.pli[current_leader_position];
  if (
    card1->type == VOIDCARD && card2->type == VOIDCARD
    && card3->type == VOIDCARD) { // it is the first move
    if ((int)player_card->type == (int)game.active_trump) {
      // we initialize the cut variable by determining if the trick color is the trump or not
      *cut = 1;
    } else {
      *cut = 0;
    }
    return position;
  } else {
    switch (game.trick_cut) // the "best" color are changed if the trick was not cut
    {
      case 1: // trick was cut, we only have to check for trumps
        if ((int)leader_card->type == (int)game.active_trump) { // leader played trump
          if (
            (int)player_card->type
            == (int)game.active_trump) { // player played trump, we need to compare the values
            if (
              card_value(*player_card, game)
              > card_value(*leader_card, game)) { // a better card is played
              return position;
            } else {
              if (
                card_value(*player_card, game)
                < card_value(*leader_card, game)) { // a worst card is played
                return current_leader_position;
              } else { // player and leader played 7 and 8 trump
                if (player_card->value > leader_card->value) {
                  return position;
                } else {
                  return current_leader_position;
                }
              }
            }
          } else { // player didn't play trump, can't win against a trump
            return current_leader_position;
          }
        } else { // leader didn't play trump, player cut
          return position;
        }
        break;
      case 0: // trick wasn't cut, leader has played the right color (the opposite is impossible here)
        if (
          player_card->type
          == game.trick_color) { // player played the right color, we need to compare values
          if (
            card_value(*player_card, game)
            > card_value(*leader_card, game)) { // a better card is played
            return position;
          } else {
            if (
              card_value(*player_card, game)
              < card_value(*leader_card, game)) { // a worst card is played
              return current_leader_position;
            } else { // player and leader played 7,8 or 9 of the right color
              if (player_card->value > leader_card->value) {
                return position;
              } else {
                return current_leader_position;
              }
            }
          }
        } else { // can't win against the good color without cutting
          return current_leader_position;
        }
        break;
    }
  }
  return -1; // TODO: explicit exhaustive branching
}
