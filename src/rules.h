#ifndef RULES_H
#define RULES_H

#include <inttypes.h>

enum CardColor {
  Hearts,
  Tiles,
  Clovers,
  Spikes
};
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
  int trick_points_total; //variable enabling a count of the points won each trick rather than having to stock all the cards won until the end of the 8 tricks
  int declaration_points; //variable to count the declaration that the player did. If the team of the player does not have the highest value declaration, then this variable will be equal to 0
  int tricks_won; //counts the number of tricks won by each player for the capot and general contracts
};
typedef struct Player Player;

enum ContractType {
ChosenColour,
Coinche,
Surcoinche,
Capot,
General
};
typedef enum ContractType ContractType;


struct Game {
  Player players[4];
  ContractType ActiveContract;
  int ContractPoints; //only if the active contract is a chosen colour, capot=250 or general=500. Otherwise =0
};

#endif // RULES_H
