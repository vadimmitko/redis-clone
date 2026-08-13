#include <optional>

#include "commands.h"

RespValue cmd_get(std::vector<std::string> &&args, RedisDb &db) {
  if (args.size() != 1) {
    return RespError {  "wrong number of arguments" };
  }

  std::optional<std::string> value_o = db.get_value_by_key(args[0]);

  if (!value_o.has_value()) return Nil{};

  return BulkString { std::move(value_o.value()) };
};
