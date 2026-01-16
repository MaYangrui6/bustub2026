//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// check_util.h
//
// Identification: src/include/common/util/check_util.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cassert>
#include <stdexcept>

namespace bustub {

/**
 * TCHECK_WITH macro for conditional checking with custom exception type.
 * This macro checks a condition and throws a custom exception if the condition is false.
 * @param condition The condition to check
 * @param exception_type The type of exception to throw
 * @param message The error message to throw if the condition is false (optional, defaults to empty)
 */
#define TCHECK_WITH(condition, exception_type, ...)                    \
  do {                                                                 \
    if (!(condition)) {                                                \
      const char *msg[] = {__VA_ARGS__};                               \
      if (sizeof(msg) > 0 && msg[0] != nullptr) {                      \
        throw exception_type(std::string("TCHECK failed: ") + msg[0]); \
      } else {                                                         \
        throw exception_type("TCHECK failed");                         \
      }                                                                \
    }                                                                  \
  } while (0)

/**
 * TCHECK macro for conditional checking.
 * This macro checks a condition and aborts the program if the condition is false.
 * @param condition The condition to check
 * @param message The error message to display if the condition is false (optional, defaults to "TCHECK failed")
 */
#define TCHECK(condition, ...)                                                    \
  do {                                                                            \
    if (!(condition)) {                                                           \
      const char *messages[] = {__VA_ARGS__};                                     \
      if (sizeof(messages) / sizeof(messages[0]) > 0 && messages[0] != nullptr) { \
        assert(false && messages[0]);                                             \
      } else {                                                                    \
        assert(false && "TCHECK failed");                                         \
      }                                                                           \
    }                                                                             \
  } while (0)

}  // namespace bustub
