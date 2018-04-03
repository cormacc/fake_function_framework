#ifndef fff_unity_helper_H
#define fff_unity_helper_H

/*
    FFF helper macros for Unity.
*/

/*
    Fail if the function was not called the expected number of times.
*/
#define TEST_ASSERT_CALLED_TIMES(times_, function_)                         \
    TEST_ASSERT_EQUAL_MESSAGE(times_,                                       \
        function_ ## _fake.call_count,                                      \
        "Function " #function_ " called the incorrect number of times.")
/*
    Fail if the function was not called exactly once.
*/
/* #define TEST_ASSERT_CALLED(function_) TEST_ASSERT_CALLED_TIMES(1, function_) */

/*
    Fail if the function was called 1 or more times.
*/
#define TEST_ASSERT_NOT_CALLED(function_) TEST_ASSERT_CALLED_TIMES(0, function_)

/*
    Fail if the function was not called in this particular order.
*/
#define TEST_ASSERT_CALLED_IN_ORDER(order_, function_)                      \
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void *) function_,                       \
        fff.call_history[order_],                                           \
        "Function " #function_ " not called in order " #order_ )

/*
  Fail if the function was not called with the given parameter values
 */
#include "vaiter64.h"
#define _FFF_NAME(FN) FN##_fake
#define _FFF_CALLS(FN) _FFF_NAME(FN).call_count
#define _FFF_LAST_ARG_VAL(FN, IDX) _FFF_NAME(FN).arg##IDX##_val
#define _FFF_NTH_ARG_VAL(FN, CALL_IDX, ARG_IDX) _FFF_NAME(FN).arg##ARG_IDX##_history[CALL_IDX-1]

#define _FFF_VERIFY_ARG(FN, VAL, ARG_IDX) VAL == _FFF_LAST_ARG_VAL(FN, ARG_IDX)
#define _FFF_ASSERT_ARG(FN, VAL, ARG_IDX) TEST_ASSERT_EQUAL(VAL, _FFF_LAST_ARG_VAL(FN, ARG_IDX));

#define _FFF_VERIFY_HISTORICAL_ARG(FN, CALL_IDX, VAL, ARG_IDX) (VAL == _FFF_NTH_ARG_VAL(FN, CALL_IDX, ARG_IDX))
#define _FFF_ASSERT_HISTORICAL_ARG(FN, CALL_IDX, VAL, ARG_IDX) TEST_ASSERT_EQUAL(VAL, _FFF_NTH_ARG_VAL(FN, CALL_IDX, ARG_IDX));


/**
 * Verifies that the named function was called once and only once, and optionally verifies its arguments
 * @param FN The mocked function name
 * @param ... Expected values of the function arguments
 */
#define TEST_ASSERT_CALLED(FN, ...)                  \
  TEST_ASSERT_CALLED_TIMES(1, FN);                   \
  VA_FIX_IDX_EACH(_FFF_ASSERT_ARG, FN, __VA_ARGS__)

/**
 * Verifies that the named function at least CALL_IDX times, and optionally verifies the arguments for that call
 * @param FN The mocked function name
 * @param CALL_IDX The (1-based) call index
 * @param ... Expected values of the function arguments
 */
#define TEST_ASSERT_NTH_CALL(FN, CALL_IDX, ...)                         \
  TEST_ASSERT_TRUE(CALL_IDX > 0);                                       \
  TEST_ASSERT_TRUE(_FFF_CALLS(FN) >= CALL_IDX);                         \
  VA_2FIX_IDX_EACH(_FFF_ASSERT_HISTORICAL_ARG, FN, CALL_IDX, __VA_ARGS__)

/**
 * Verifies that the named function was called at least once, and optionally verifies the arguments for the final call
 * @param FN The mocked function name
 * @param ... Expected values of the function arguments
 */
#define TEST_ASSERT_LAST_CALL(FN, ...) TEST_ASSERT_NTH_CALL(FN, _FFF_CALLS(FN), __VA_ARGS__)

#define _FFF_AND_VERIFY_NTH_CALL_ARG(FN, CALL_IDX, VAL, ARG_IDX) && _FFF_VERIFY_HISTORICAL_ARG(FN, CALL_IDX, VAL, ARG_IDX)
#define _FFF_VERIFY_NTH_CALL(FN, CALL_IDX, ...)                         \
  ( (CALL_IDX > 0) && (CALL_IDX <= _FFF_CALLS(FN)) VA_2FIX_IDX_EACH(_FFF_AND_VERIFY_NTH_CALL_ARG, FN, CALL_IDX, __VA_ARGS__) )

//This uses GCC compound statement expression:
//https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
#define _FFF_VERIFY_ANY_CALL(FN, ...) ({                          \
  bool verified = false;                                          \
  int call_idx = _FFF_CALLS(FN);                                  \
  while(call_idx && !verified) {                                  \
    verified |= _FFF_VERIFY_NTH_CALL(FN, call_idx, __VA_ARGS__);  \
    call_idx--;                                                   \
  }                                                               \
  verified;                                                       \
    })

#define TEST_ASSERT_ANY_CALL(FN, ...) TEST_ASSERT_TRUE(_FFF_VERIFY_ANY_CALL(FN, __VA_ARGS__))

                                                                      \
  /* #define TEST_ASSERT_ANY_CALL(FN, ...) {                               \ */
  /*                                                                       \ */
  /*  } */

/**
 * Verify that the named function was called COUNT times
 * @param FN The mocked function name
 * @param COUNT The expected number of calls
 * This duplicates the functionality of the existing TEST_ASSERT_CALLED_TIMES, but reverses the parameter order
 * for API consistency with the new variadic macro assertions.
 */
#define TEST_ASSERT_CALLS(FN, COUNT) TEST_ASSERT_EQUAL(COUNT, _FFF_CALLS(FN))

// Should these additions get pushed upstream to the fff project rather than ceedling/fff
#define SET_RETURN_VAL(FN, VAL) FN##_fake.return_val = VAL

/**
 * Set a sequence of consecutive return values for a given mocked function
 * @param FN The function name
 * @param ... The sequence of return values
 * This is a convenient wrapper for existing FFF functionality:
 * @code
 * SET_RETURN_VAL_SEQ(SomeModule_doSomething, 1, 2, 3, 4, 5);
 * @endcode
 * ... expands to ...
 * @code
 * {
 *   typeof(SomeModule_doSomething.return_val) return_seq[] = {1, 2, 3, 4, 5};
 *   SET_RETURN_SEQ(SomeModule_doSomething, return_seq, 5);
 * }
 * @endcode
 */
#define SET_RETURN_VAL_SEQ(FN, ...)                                     \
  typeof(FN##_fake.return_val) FN##_retseq[] = {__VA_ARGS__};           \
  SET_RETURN_SEQ(FN, FN##_retseq, sizeof(FN##_retseq)/sizeof(FN##_retseq[0]))

/**
 * Repeat one or more operations a given number of times
 * @param REP_COUNT The number of repetitions
 * @param ... The action(s) to repeat
 *
 * (This is handy when writing tests, but probably doesn't belong here.)
 */
#define REPEAT(REP_COUNT, ...) {                \
    uint16_t repetitions = REP_COUNT;           \
    while(repetitions--) {                      \
      __VA_ARGS__;                              \
    }                                           \
  }

#endif
