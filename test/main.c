#define MAIN_C
#include "test.h"
#include "rules.h"

BEGIN_TEST(all) {
  EXECUTE_TEST(rules, "Checks associated to the rules defined in src/rules.c")
}
END_TEST()

int main(int argc, char** argv) {
  return !test_all();
}
