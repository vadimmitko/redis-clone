#ifndef REDIS_CLONE_RESP_H_
#define REDIS_CLONE_RESP_H_

#include <cstdint>
#include <vector>
#include <string>
#include <variant>
#include <optional>

constexpr size_t kMaxMultibulkLen = 1024 * 1024;
constexpr size_t kMaxBulkLen = 512 * 1024 * 1024;

struct SimpleString { std::string value; };
struct Integer { int64_t n; };
struct BulkString    { std::string value; };
struct RespError     { std::string message; };
struct Nil {};
using RespValue = std::variant<Integer, SimpleString, BulkString, RespError, Nil>;

struct ParsedCommand {
  std::string name;
  std::vector<std::string> args;
};

std::optional<std::vector<std::string>> parse_tokens(std::string_view buffer);

std::optional<ParsedCommand> parse_command(std::string_view buffer);

std::string serialize(const RespValue& v);

#endif
