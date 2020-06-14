#ifndef RULES_H
#define RULES_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

// VOID represents an empty slot in order not to interfere with hand checks
// or trick played card checks
enum CardColor { HEARTS, TILES, CLOVERS, SPIKES, VOIDCARD };
typedef enum CardColor CardColor;

/** Represents a single card
`value` corresponds to:
- `0` -> Card used or slot unfilled
- `7` -> 7
- `8` -> 8
- `9` -> 9
- `10` -> 10
- `11` -> Jack
- `12` -> Queen
- `13` -> King
- `14` -> Ace
No differentiation is made between trump and non-trump cards here.
*/
struct Card {
  CardColor type; // TODO: rename to `color`
  uint8_t value;
};
typedef struct Card Card;

struct Player {
  Card cards[8];
  uint8_t cards_revealed; // bitmap of the cards that were revealed before the first round
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

enum TrumpColor { TRUMP_HEARTS, TRUMP_TILES, TRUMP_CLOVERS, TRUMP_SPIKES, ALLTRUMP, NOTRUMP };
typedef enum TrumpColor TrumpColor;

enum Teams {
  EW, // team east/west, double AI
  NS, // team south/north, player's team
};
typedef enum Teams Teams;

struct Game {
  Player players[4]; /**
  players: 0 = human/south,
  1 = west
  2 = north
  3 = east */
  ContractType active_contract;
  int contract_points; // to stock the points of the active contract
    // or of the contract contested by a coinche / surcoinche
  int general_attacker; // only relevant for a general contract
    // to know what player to check
  TrumpColor active_trump; // to stock the active trump
  Teams winning_team;
  Teams contracted_team;
  Card pli[4]; // to stock the cards played during a trick.
    // the number of the card corresponds to
    // the position of its player.
  CardColor trick_color; // to stock the colour of each trick to
    // then compare it to the card played
  int trick_cut; //=0 if not, =1 if cut, reset to 0 at the beginning of a trick
    // except if the trump is a chosen colour and the trick colour
    // is the same colour as the trump. In this specific case it
    // takes the value 1. A function will do this.
  int trick_leader_position; // to know who is winning the trick
};
typedef struct Game Game;

// TODO: rename turn_points to game_points or final_points

/**
The goal of this function is to compute the points of a team at the end of a 8
tricks turn. To do so, we give all the informations about the game and the 4
players to the function. The function will then return the points of the wanted
team which is an integer. This function doesn't check if the contract was
realized or not, at this point we already know it. Another function will do it.
*/
int turn_points(Game game, Teams active_team);

/**
The goal of the following function is to determine
either or not the contract was realized. To lighten
the code, for a coinche or surcoinche, the variable contract_points
will keep the points of the contract that was contested.
The function will then return which team won the 8 tricks turn by
telling if the contracted team filled the requirements or not.
*/
Teams contract_check(Game game);

/**
These functions will be used in the previous one to lighten the code and just
realize a switch to enter the winning team.
*/
#define attackers_win(team_to_switch) (team_to_switch)

#define defenders_win(team_to_switch) (!(team_to_switch))

/**
The following function will simply do a sum of the trick points.
The identification of the value of each card will be done in an other function.
The "10 of der" is computed too.
*/
int trick_points(Card card1, Card card2, Card card3, Card card4, Game game);

/**
This function will identify the value of each card entered. Useful for
the trick points but can also be used in other functions if needed.
*/
int card_value(Card card, Game game);

/** Determines if a move is possible or not.
* @param game - The current game
* @param card - The card to check against
* @param player_index - The player's index
* @returns 1 if the move is possible without cutting, 2 if it requires to cut, 0 otherwise.
* Note that a player may still be able to play a card of the wrong color if they have no
* other playable card.
**/
int move_check(Game* game, Card card, size_t player_index);

/** Determines after a move was done who is leading the round
* @param game - The current game, from which the leader's position will be extracted
* @param player_index - The current player position
**/
int leader_trick(Game* game, size_t player_index);

/** To check if a card can win against the current trick's best card played.
* @param game - The current game, from which the leader's position will be extracted
* @param card - The card to check against
*/
bool card_wins(Game* game, Card card, size_t player_index);

/**
To check a declaration.
*/
int declaration_check(Card cards[5]);

#endif // RULES_H
