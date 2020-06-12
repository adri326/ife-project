#define MAIN_C
#include "test.h"
#include "rules.h"
#include "game.h"
#include "ai.h"

BEGIN_TEST(all) {
  EXECUTE_TEST(rules, "Checks associated to the rules defined in src/rules.c")
  EXECUTE_TEST(game, "Test the gameplay-related functions")
  EXECUTE_TEST(ai, "Test the AI-related functions")
}
END_TEST()

int main(int argc, char** argv) {
  return !test_all();
}
