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
  Card[7] cards;
  uint8_t n_cards; // number of cards currently held
};
typedef struct Player Player;

struct Game {
  Player[4] players;
}

#endif // RULES_H
