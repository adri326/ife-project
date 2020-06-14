#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "rules.h"


#ifdef _WIN32
#define IS_WIN
#endif
#ifdef _WIN64
#define IS_WIN
#endif
#ifdef IS_WIN
#include <windows.h>
#define SLEEP(x) SleepEx(x, true)
#else
#include <unistd.h>
#define SLEEP(x) usleep(x * 1000)
#endif

#ifndef M_PI
#define M_PI 3.141592653589
#endif

#define INTERPOLATE_SINE(x) (sin(M_PI * (float)(x) - M_PI / 2.0) / 2.0 + 0.5)
#define INTERPOLATE_EASE_IN_OUT(x) ((x) <= 0.5 ? (x) * (x) * 2 : 1 - (1 - (x)) * (1 - (x)) * 2)
#define INTERPOLATE_EASE_IN(x) ((x) * (x))
#define INTERPOLATE_EASE_OUT(x) (1 - ((x) - 1) * ((x) - 1))

/** Placeholder function to determine which card the player wishes to play. Modifies `game`.
* @param game - The current game
* @returns true if the game should go on, false if the game should be halted
**/
bool players_turn(Game* game);

/** Modifies `game` as to make player no. `player_index` player their `card_index`-th card.
* @param game - The current game
* @param player_index - The current player's position
* @param card_index - The played card's index
* @returns true on success, false if an error occured or if the user decided to close the window
**/
bool play_card(Game* game, size_t player_index, size_t card_index);

/** Plays a game's turn. Updates scores and calls both `ia_turn` and `players_turn`.
* @param game - The current game
* @returns true if the game should go on, false if the game should be halted
**/
bool game_turn(Game* game);

/** Updates the scores of the game.
* @param game - The current game
**/
void update_scores(Game* game);

/** Plays out a game. Scores will be incrementally updated as `game_turn` is called.
* @param game - The current game
* @returns true if the game went on, false if the game halted
**/
bool play_all_turns(Game* game, size_t first_player_index);

/** Distributes the 32 cards to the 4 players
* Note: the last two cards can be hidden during the announcement phase by overriding each player's n_cards
* @param cards - The set of cards
* @param game - The current game
**/
void distribute_cards(Card cards[32], Game* game);

/** Initializes 32 cards
* @param cards - The set of cards, will be modified
**/
void init_cards(Card cards[32]);

/** Shuffles the 32 cards
* All combinations should be theoretically possible
* @param cards - The set of cards, will be modified
**/
void shuffle_cards(Card cards[32]);

/**
Player announces their contract. Changes the variables in the Game
structure directly.
*/
bool player_announce_contract(Game* game);

/**
Using player and ai announce_contract functions,
this function manages the dealing phase. Returns
0 if nobody took a contract and 1 on success and 2 on exit
*/
int dealing_phase(Game* game,size_t dealer);

/** Prints the end screen, telling the player wether or not they won
* @param game - The current game
* @returns true if the user entered their initials, false otherwise
**/
bool game_end_screen(Game* game);

#endif // GAME_H
