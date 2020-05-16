#include "test.h"
#include "rules.h"

BEGIN_TEST(all) {
  EXECUTE_TEST(turn_points, "Test the turn_points() function");
}
END_TEST()

int main(int argc, char** argv) {
  return !test_all();
}
