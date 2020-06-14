#include "leaderboards.h"
#include <stdio.h>

BestScores load_best_scores(size_t page) {
  BestScores res;
  for (size_t n = 0; n < 10; n++) {
    res.scores[n].name[0] = 0;
    res.scores[n].wins = 0;
    res.scores[n].points = 0;
  }

  FILE* file = fopen(LEADERBOARDS_FILE, "r");
  int line = 0;
  if (file != NULL) {
    do {
      line++;
      char name[3];
      size_t wins;
      size_t points;
      if (fscanf(file, "%c%c%c %zu %zu\n", &name[0], &name[1], &name[2], &wins, &points) != 5) {
        printf("Error reading line %d. Skipping!\n", line);
        continue;
      }
      for (size_t n = 0; n < 10; n++) {
        if (res.scores[n].wins < wins) {
          for (size_t o = 8; o < 10; o--) {
            res.scores[o + 1].wins = res.scores[o].wins;
            res.scores[o + 1].points = res.scores[o].points;
            res.scores[o + 1].name[0] = res.scores[o].name[0];
            res.scores[o + 1].name[1] = res.scores[o].name[1];
            res.scores[o + 1].name[2] = res.scores[o].name[2];
          }
          res.scores[n].wins = wins;
          res.scores[n].points = points;
          // The compiler is gonna unfold it anyways
          res.scores[n].name[0] = name[0];
          res.scores[n].name[1] = name[1];
          res.scores[n].name[2] = name[2];
          break;
        }
      }
    } while (!feof(file));
    fclose(file);
  } else {
    printf("Error opening file!\n");
  }
  return res;
}

Score get_score(char name[3]) {
  Score res = {
    .name = {0, 0, 0},
    .points = 0,
    .wins = 0
  };
  FILE* file = fopen(LEADERBOARDS_FILE, "r");
  int line = 0;
  if (file != NULL) {
    do {
      line++;
      char n_name[3];
      size_t wins;
      size_t points;
      if (fscanf(file, "%c%c%c %zu %zu\n", &n_name[0], &n_name[1], &n_name[2], &wins, &points) != 5) {
        printf("Error reading line %d. Skipping!\n", line);
        continue;
      }
      if (name[0] == n_name[0] && name[1] == n_name[1] && name[2] == n_name[2]) {
        res.wins = wins;
        res.points = points;
      }
    } while (!feof(file));
    fclose(file);
  } else {
    printf("Error opening file!\n");
  }

  return res;
}

void set_score(Score score) {
  FILE* file = fopen(LEADERBOARDS_FILE, "r");
  FILE* replica = fopen(LEADERBOARDS_FILE "~", "w");
  int line = 0;

  if (file != NULL && replica != NULL) {
    do {
      line++;
      char name[3];
      size_t wins;
      size_t points;
      if (fscanf(file, "%c%c%c %zu %zu\n", &name[0], &name[1], &name[2], &wins, &points) != 5) {
        printf("Error reading line %d. Skipping!\n", line);
        continue;
      }
      if (name[0] != score.name[0] || name[1] != score.name[1] || name[2] != score.name[2]) {
        fprintf(replica, "%c%c%c %zu %zu\n", name[0], name[1], name[2], wins, points);
      }
    } while (!feof(file));
    fclose(file);
    fclose(replica);
    append_score(score);
  } else {
    printf("Error opening file!\n");
  }
}

void append_score(Score score) {
  FILE* file = fopen(LEADERBOARDS_FILE, "w");
  FILE* replica = fopen(LEADERBOARDS_FILE "~", "r");
  int line = 0;

  if (file != NULL && replica != NULL) {
    do {
      line++;
      char name[3];
      size_t wins;
      size_t points;
      if (fscanf(replica, "%c%c%c %zu %zu\n", &name[0], &name[1], &name[2], &wins, &points) != 5) {
        printf("Error reading line %d. Skipping!\n", line);
        continue;
      }
      fprintf(file, "%c%c%c %zu %zu\n", name[0], name[1], name[2], wins, points);
    } while (!feof(replica));
    fprintf(file, "%c%c%c %zu %zu\n", score.name[0], score.name[1], score.name[2], score.wins, score.points);
  } else {
    printf("Error opening file!\n");
  }
}
