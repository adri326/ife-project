#define MAIN_C
#include "test.h"
#include "rules.h"

BEGIN_TEST(all) {
  EXECUTE_TEST(turn_points, "Test the turn_points() function");
  EXECUTE_TEST(contract_check, "Test the contract_check() function");
  EXECUTE_TEST(card_value, "Test that card_value() returns the right card value");
}
END_TEST()

int main(int argc, char** argv) {
  return !test_all();
}
