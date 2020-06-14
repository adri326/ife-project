#ifndef LEADERBOARDS_H
#define LEADERBOARDS_H

#include <stdlib.h>

#ifndef LEADERBOARDS_FILE
#define LEADERBOARDS_FILE "./leaderboards.txt"
#endif

struct Score {
  char name[3];
  size_t wins;
  size_t points;
};
typedef struct Score Score;

struct BestScores {
  Score scores[10];
};
typedef struct BestScores BestScores;

BestScores load_best_scores(size_t page);
Score get_score(char name[3]);
void set_score(Score score);
void append_score(Score score);

#endif // LEADERBOARDS_H
