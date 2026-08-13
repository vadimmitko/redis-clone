#ifndef REDIS_CLONE_COMMANDS_SET_H_
#define REDIS_CLONE_COMMANDS_SET_H_

#include <array>
#include <vector>
#include <string>
#include <vector>

#include "../resp.h"
#include "../db.h"

constexpr long long kMaxExpireSeconds = 9999999999;

inline constexpr std::array<std::string_view, 4> kSetOptions = {
    "EX", "PX", "NX", "XX"
};

constexpr bool isValidSetOption(std::string_view opt) {
  for (const std::string_view& candidate : kSetOptions) {
    if (candidate == opt) return true;
  }
  return false;
}

RespValue cmd_set(std::vector<std::string>&& args, RedisDb& db);

#endif
