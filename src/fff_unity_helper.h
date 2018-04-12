#ifndef fff_unity_helper_H
#define fff_unity_helper_H

/*
    FFF helper macros for Unity.
*/

/*
    Fail if the function was not called the expected number of times.
*/
#define TEST_ASSERT_CALLED_TIMES(times_, function_) FFF_ASSERT_CALLS(function_, times_)

/*
    Fail if the function was not called exactly once.
*/
 #define TEST_ASSERT_CALLED(function_) FFF_ASSERT_CALLED(function_)

/*
    Fail if the function was called 1 or more times.
*/
#define TEST_ASSERT_NOT_CALLED(function_) FFF_ASSERT_NOT_CALLED(function_)

/*
    Fail if the function was not called in this particular order.
*/
#define TEST_ASSERT_CALLED_IN_ORDER(order_, function_)                      \
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void *) function_,                       \
        fff.call_history[order_],                                           \
        "Function " #function_ " not called in order " #order_ )

//BEGIN CC ADDITIONS
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
