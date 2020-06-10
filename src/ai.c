#include "ai.h"
#include "game.h"

void ai_turn(Game* game, size_t player) {
    if (game->pli[(player+1)%4].type==VOIDCARD
        && game->pli[(player+2)%4].type==VOIDCARD
        && game->pli[(player+3)%4].type==VOIDCARD)
    { //if the AI is first to play, it will play a random strong card
        size_t card_index;
        int i=0;
        do {
            card_index = (int)rand()%8;
            i=i+1;
        } while (
          card_value(game->players[player].cards[card_index], *game)==0
           && i<25); //we limit the number of loops in case the ai
                     //does not have strong cards. It is faster than
                     //checking what cards were already checked each time.
        while (game->players[player].cards[card_index].type==VOIDCARD)
        { // we pick a card slot not empty
            card_index=(card_index+1)%8;
        }
        play_card(game, player, card_index);
    } else {
    play_card(game, player,
    ai_card_played (game, player, ai_win (game, player)));
    }
}

bool ai_win (Game* game, size_t player)
{
    bool wins = false;
    for (int i=0; i<8; i++)
    {
        if (card_wins(game, player, i)==true)
        {
            wins = true;
        }
    } //we check if any card in the hand can win
    return wins;
}

bool card_wins(Game* game, size_t player, int i)
{
    switch (game->trick_cut) // the "best" color are changed if the trick was not cut
    {
      case 1: // trick was cut, we only have to check for trumps
        if ((int)game->pli[game->trick_leader_position].type
             == (int)game->active_trump) { // leader played trump
          if (
            (int)game->players[player].cards[i].type
            == (int)game->active_trump) {
                // player played trump, we need to compare the values
            if (
              card_value(game->players[player].cards[i], *game)
              > card_value(game->pli[game->trick_leader_position], *game)) {
                   // a better card is played
              return true;
            } else {
              if (
                card_value(game->players[player].cards[i], *game)
                < card_value(game->pli[game->trick_leader_position], *game)) {
                    // a worst card is played
                return false;
              } else { // player and leader played 7 and 8 trump
                if (game->players[player].cards[i].value
                    > game->pli[game->trick_leader_position].value) {
                  return true;
                } else {
                  return false;
                }
              }
            }
          } else { // player didn't play trump, can't win against a trump
            return false;
          }
        } else { // leader didn't play trump, player cut
          return true;
        }
        break;
      case 0: // trick wasn't cut, leader has played the
          //right color (the opposite is impossible here)
        if (
          game->players[player].cards[i].type
          == game->trick_color) {
              // player played the right color, we need to compare values
          if (
            card_value(game->players[player].cards[i], *game)
            > card_value(game->pli[game->trick_leader_position], *game)) {
                // a better card is played
            return true;
          } else {
            if (
              card_value(game->players[player].cards[i], *game)
              < card_value(game->pli[game->trick_leader_position], *game)) {
                  // a worst card is played
              return false;
            } else { // player and leader played 7 and 8 right color
              if (game->players[player].cards[i].value
                  > game->pli[game->trick_leader_position].value) {
                return true;
              } else {
                return false;
              }
            }
          }
        } else { // can win against the good color by cutting
          if ((int)game->players[player].cards[i].type == (int)game->active_trump)
          {
              return true;
          } else { //player does not cut, can not win
            return false;
          }
        }
        break;
    }
    return false; //explicit content branching
}

size_t ai_card_played (Game* game, size_t player, bool ai_can_win)
{
    size_t card_index=0;
    switch (ai_can_win)
    { //depending on this, we choose a card by two possible ways:
    case true : //ai can win, we look for the lowest value card so that it does
        for (int i=1; i<8; i++)
        {
            if (card_value(game->players[player].cards[i],*game)
                <card_value(game->players[player].cards[card_index],*game)
                && card_wins(game,player,i)==true)
                { //if card can't win, we don't even consider it.
                    //if it can and have a lowest value, we play it.
                    card_index = i;
                } else {
                    // for 7,8 and sometimes 9, value is 0, we have to make a difference
                    //don't need to check when value is higher, won't change card_index
                    if (card_value(game->players[player].cards[i],*game)
                    == card_value(game->players[player].cards[card_index],*game)
                    && card_wins(game,player,i)==true)
                    {
                        if (game->players[player].cards[i].value
                        <game->players[player].cards[card_index].value)
                        {
                            card_index = i;
                        }//no need to check when value is higher, won't change
                    }   //card_index. No need to check when equal because it means
                }      //both cards can be played, having the smallest value yet.
        }
        break;
    default: //ai can't win, we just look for the lowest value card
        for (int i=1; i<8; i++)
        {
            if (card_value(game->players[player].cards[i],*game)
                <card_value(game->players[player].cards[card_index],*game))
            { //the card has a lower value
                card_index = i;
            } else {
                // for 7,8 and sometimes 9, value is 0, we have to make a difference
                //don't need to check when value is higher, won't change card_index
              if   (card_value(game->players[player].cards[i],*game)
                == card_value(game->players[player].cards[card_index],*game))
                {
                    if (game->players[player].cards[i].value
                        <game->players[player].cards[card_index].value)
                    {
                        card_index = i;
                    } //no need to check when value is higher, won't change
                }    //card_index. No need to check when equal because it means
            }       //both cards can be played, having the smallest value yet.
        }
    }
    return card_index;
}
