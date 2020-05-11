#ifndef RULES_H
#define RULES_H

#include <inttypes.h>

enum CardColor { HEARTS, TILES, CLOVERS, SPIKES };
typedef enum CardColor CardColor;

/** Represents a single card
`value` corresponds to:
- `0` -> Card used
- `7` -> 7
- `8` -> 8
- `9` -> 9
- `10` -> 10
- `11` -> Joker
- `12` -> Queen
- `13` -> King
- `14` -> As
No differentiation is made between trump and non-trump cards here.
*/
struct Card {
  CardColor type;
  uint8_t value;
};
typedef struct Card Card;

struct Player {
  Card cards[8];
  uint8_t n_cards; // number of cards currently held
  int trick_points_total; // variable enabling a count of the points won each
                          // trick rather than having to stock all the cards won
                          // until the end of the 8 tricks
  int declaration_points; // variable to count the declaration that the player
                          // did. If the team of the player does not have the
                          // highest value declaration, then this variable will
                          // be equal to 0.
  int tricks_won; // counts the number of tricks won by each player for the
                  // capot and general contracts
  int belote; // =0 if no belot, = 20 if belot
};
typedef struct Player Player;

enum ContractType { CHOSENCOLOUR, COINCHE, SURCOINCHE, CAPOT, GENERAL };
typedef enum ContractType ContractType;

enum TrumpColor { HEART, TILE, CLOVER, SPIKE, ALLTRUMP, NOTRUMP };
typedef enum TrumpColor TrumpColor;

enum Teams {
  EW, // team east/west, double AI
  NS, // team south/north, player's team
};
typedef enum Teams Teams;

struct Game {
  Player players[4];
  ContractType active_contract;
  int contract_points; // only if the active contract is a chosen colour,
                       // capot=250 or general=500. Otherwise =0
  TrumpColor active_trump; // to stock the active trump
  Teams winning_team;
  Teams contracted_team;
};
typedef struct Game Game;

/**
The goal of this function is to compute the points of a team at the end of a 8
tricks turn. To do so, we give all the informations about the game and the 4
players to the function. The function will then return the points of the wanted
team which is an integer. This function doesn't check if the contract was
realized or not, at this point we already know it. Another function will do it.
*/
int turn_points(
  Game game,
  Player player1,
  Player player2,
  Teams active_team,
  Player looser1,
  Player looser2);

#endif // RULES_H
