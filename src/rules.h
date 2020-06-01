#ifndef RULES_H
#define RULES_H

#include <inttypes.h>

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
  CardColor trick_colour; // to stock the colour of each trick to
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

/**
The following function must determine during a trick if a move is possible
or not. For this, we need to give the function all the required informations
about the game and played card and the position of the player in case we need
to check their hand. The function will return 0 if the move is impossible and
1 otherwise so that we can easily put a control with error message in main.
The check is done for any player so that we can use it to control the AI
if needed. We also need to be able to modify the trick_cut variable if
needed that's why we transmit it with a pointer. Finally, for the very
specific case where the player has a trump, no right colour for a chosen
colour trump, and does not want to cut which is possible if the player's
ally is leading the trick, we need to have the position of the trick leader.
*/
int move_check(Game game, Card card, int position, int* cut, int leader_position);

/**
We now need a function that determines, if the move was possible, who
is leading after the move. It will return the position of the leader by
simply comparing the former leader's card to the last card played.
Therefore this function will be called after each move.
The first player to make a move during a trick will automatically
be set as leader by the function. To do so, we need the general informations of game,
the position of the last player to have made a move, and the current
leader's position which will correspond to the variable trick_leader_position.
If it is the first move of the trick, this variable won't matter.
This same variable will then take the value returned by this function in the main.
This function will also initialize the trick_cut variable on the first move of a trick.
*/
int leader_trick(Game game, int position, int current_leader_position, int* cut);

#endif // RULES_H
