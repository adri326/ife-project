#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdbool.h>

/** Prints where the assert errored out **/
#ifdef __LINE__
  #define ASSERT_LOCATION() printf(" (in %s:%d)", CURR_TEST, __LINE__);
#else
  #define ASSERT_LOCATION() printf(" (in %s)", CURR_TEST);
#endif

/** Asserts that `condition` yields true:
* Prints an error message if `condition` yields false
* Returns `false` if `condition` yields false
**/
#define ASSERT(condition) if (!(condition)) {printf("  Assertion failed!"); ASSERT_LOCATION(); printf("\n"); return false;}

/** Asserts that `condition` yields true:
* Prints an error message if `condition` yields false
* Prints the given value if `condition` yields false
* Returns `false` if `condition` yields false
**/
#define ASSERT_MSG(condition, ...) if (!(condition)) {printf("  Assertion failed"); ASSERT_LOCATION(); printf(": \""); printf(__VA_ARGS__); printf("\"\n"); return false;}

/** Asserts that `a` equals `b`:
* Prints an error message if `a` != `b`
* Returns `false` if `condition` yields false
**/
#define ASSERT_EQ(a, b) ASSERT_MSG((a) == (b))

/** Asserts that `a` equals `b`:
* Prints an error message if `a` != `b`
* Prints the given value if `a` != `b`
* Returns `false` if `condition` yields false
**/
#define ASSERT_EQ_MSG(a, b, ...) ASSERT_MSG((a) == (b), __VA_ARGS__)

/** Asserts that `a` equals `b`:
* Prints an error message if `a` != `b`
* Prints the `a` and `b` if `a` != `b` using `fmt`
* Returns `false` if `condition` yields false
**/
#define ASSERT_EQ_PRI(a, b, fmt) ASSERT_MSG((a) == (b), "Expected " fmt " to be equal to " fmt ".", b, a)

/** Executes the given test:
* Prints the given description if the test yields false
* Sets RES to false if the test yields false
**/
#define EXECUTE_TEST(name, ...) printf("Executing test \"" #name "\" ...\n"); if (!test_##name()) {printf("  ... %s: \"", #name); printf(__VA_ARGS__); printf("\"\n  ... " #name ": error!\n"); RES = false;} else {printf("  ... " #name ": ok!\n");}

/** Executes the given test:
* Prints the given description if the test fails
* Returns false if the test fails
**/
#define ASSERT_TEST(name, ...) if (!name()) {printf("... \""); printf(__VA_ARGS__); printf("\"\n"); return false;}

/** Declares a test body **/
#define BEGIN_TEST(name) bool test_##name() {char CURR_TEST[1024] = #name; bool RES = true;

/** Ends a test body **/
#define END_TEST() return RES;}

/** Declares a test **/
#define DECL_TEST(name) bool test_##name(void);

#endif // TEST_H
