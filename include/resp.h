#ifndef REDIS_CLONE_RESP_H_
#define REDIS_CLONE_RESP_H_

#include <deque>
#include <ranges>
#include <cstdint>
#include <vector>
#include <string>
#include <variant>
#include <optional>


constexpr size_t kMaxMultibulkLen = 1024 * 1024;
constexpr size_t kMaxBulkLen = 512 * 1024 * 1024;

struct SimpleString { const std::string value; };
struct Integer { const int64_t n; };
struct Array { std::ranges::subrange<std::deque<std::string>::const_iterator> v; };
struct BulkString    { const std::string value; };
struct RespError     { const std::string message; };
struct Nil {};
using RespValue = std::variant<Integer, Array, SimpleString, BulkString, RespError, Nil>;

struct ParsedCommand {
  std::string name;
  std::vector<std::string> args;
};

std::optional<std::vector<std::string>> parse_tokens(std::string_view buffer);

std::optional<ParsedCommand> parse_command(std::string_view buffer);

std::string serialize(const RespValue& v);

#endif
