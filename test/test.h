#ifndef TEST_H
#define TEST_H

#include <stdbool.h>
#include <stdio.h>

#ifdef NO_TEST_DEPTH
#define TEST_DEPTH 0
#else
#ifdef MAIN_C
size_t TEST_DEPTH = 0;
#else
extern size_t TEST_DEPTH;
#endif
#endif

#define TEST_ENV

/** Prints where the assert errored out **/
#ifdef __LINE__
#define ASSERT_LOCATION() printf(" (in %s:%d)", CURR_TEST, __LINE__);
#else
#define ASSERT_LOCATION() printf(" (in %s)", CURR_TEST);
#endif

#ifdef NO_TEST_DEPTH
// Empty implementations
#define ASSERT_INDENT()
#define INC_TEST_DEPTH()
#define DEC_TEST_DEPTH()
#else
/** Indents the logs by TEST_DEPTH **/
#define ASSERT_INDENT()                                                        \
  for (size_t indent_count = 0; indent_count < TEST_DEPTH; indent_count++) {   \
    printf("  ");                                                              \
  }
/** Increments TEST_DEPTH **/
#define INC_TEST_DEPTH() TEST_DEPTH++;
/** Decrements TEST_DEPTH **/
#define DEC_TEST_DEPTH() TEST_DEPTH--;
#endif

/** Asserts that `condition` yields true:
 * Prints an error message if `condition` yields false
 * Returns `false` if `condition` yields false
 **/
#define ASSERT(condition)                                                      \
  if (!(condition)) {                                                          \
    ASSERT_INDENT();                                                           \
    printf("Assertion failed!");                                               \
    ASSERT_LOCATION();                                                         \
    printf("\n");                                                              \
    return false;                                                              \
  }

/** Asserts that `condition` yields true:
 * Prints an error message if `condition` yields false
 * Prints the given value if `condition` yields false
 * Returns `false` if `condition` yields false
 **/
#define ASSERT_MSG(condition, ...)                                             \
  if (!(condition)) {                                                          \
    ASSERT_INDENT();                                                           \
    printf("Assertion failed");                                                \
    ASSERT_LOCATION();                                                         \
    printf(": \"");                                                            \
    printf(__VA_ARGS__);                                                       \
    printf("\"\n");                                                            \
    return false;                                                              \
  }

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
#define ASSERT_EQ_PRI(a, b, fmt)                                               \
  ASSERT_MSG((a) == (b), "Expected " fmt " to be equal to " fmt ".", b, a)

/** Executes the given test:
 * Prints the given description if the test yields false
 * Sets RES to false if the test yields false
 **/
#define EXECUTE_TEST(name, ...)                                                \
  {                                                                            \
    ASSERT_INDENT();                                                           \
    printf("Executing test \"" #name "\" ...\n");                              \
    INC_TEST_DEPTH();                                                          \
    if (!test_##name()) {                                                      \
      ASSERT_INDENT();                                                         \
      printf("... %s: \"", #name);                                             \
      printf(__VA_ARGS__);                                                     \
      printf("\"\n");                                                          \
      ASSERT_INDENT();                                                         \
      printf("... " #name ": error!\n");                                       \
      RES = false;                                                             \
    } else {                                                                   \
      ASSERT_INDENT();                                                         \
      printf("... " #name ": ok!\n");                                          \
    }                                                                          \
    DEC_TEST_DEPTH();                                                          \
  }

/** Declares a test body **/
#define BEGIN_TEST(name)                                                       \
  bool test_##name() {                                                         \
    char CURR_TEST[1024] = #name;                                              \
    bool RES = true;

/** Ends a test body **/
#define END_TEST()                                                             \
  return RES;                                                                  \
  }

/** Declares a test **/
#define DECL_TEST(name) bool test_##name(void);

#endif // TEST_H
