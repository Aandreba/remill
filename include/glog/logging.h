#pragma once

#include <iostream>

namespace remill::petit {

struct LogMessageVoidify {
  // This has to be an operator with a precedence lower than << but
  // higher than ?:
  constexpr void operator&(std::ostream &) const noexcept {}
};

static std::ostream &INFO_LOG = std::cerr;
static std::ostream &WARNING_LOG = std::cerr;
static std::ostream &ERROR_LOG = std::cerr;
static std::ostream &FATAL_LOG = std::cerr;

};  // namespace remill::petit

#define LOG(severity) (::remill::petit::severity##_LOG)
#define LOG_IF(severity, condition) \
  static_cast<void>(0), \
      !(condition) ? (void) 0 \
                   : ::remill::petit::LogMessageVoidify() & LOG(severity)
#define CHECK(condition) \
  LOG_IF(FATAL, !(condition)) << "Check failed: " #condition " "

#define CHECK_EQ(lhs, rhs) CHECK((lhs) == (rhs))
#define CHECK_LT(lhs, rhs) CHECK((lhs) < (rhs))
#define CHECK_LE(lhs, rhs) CHECK((lhs) <= (rhs))
#define CHECK_NOTNULL(value) CHECK((value) != nullptr)

#ifndef NDEBUG
#  define DLOG(severity) LOG(severity)
#  define DCHECK_EQ(lhs, rhs) CHECK_EQ(lhs, rhs)
#else
#  define DLOG(severity) LOG_IF(severity, false)
#  define DCHECK_EQ(lhs, rhs) CHECK(true)
#endif
