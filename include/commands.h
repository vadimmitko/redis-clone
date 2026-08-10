#ifndef REDIS_CLONE_COMMANDS_H_
#define REDIS_CLONE_COMMANDS_H_

#include <variant>
#include <string>
#include <vector>

struct SimpleString { std::string value; };
struct BulkString    { std::string value; };
struct RespError     { std::string message; };
struct Nil {};

using RespValue = std::variant<SimpleString, BulkString, RespError, Nil>;

RespValue cmd_ping(std::vector<std::string>&& args);
RespValue cmd_echo(std::vector<std::string>&& args);

#endif
