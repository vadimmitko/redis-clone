#include "commands/lrange.h"
#include "db.h"

#include <algorithm>
#include <deque>
#include <ranges>
#include <string>

RespValue cmd_lrange(std::vector<std::string> &&args, RedisDb &db) {
  if (args.size() != 3) return RespError { "wrong number of arguments" };

  std::optional<std::reference_wrapper<const RedisObject>> val_o = db.get_value_by_key(std::move(args[0]));

  if (!val_o.has_value()) return Array { std::ranges::subrange<std::deque<std::string>::const_iterator>{} };

  auto* list = std::get_if<std::deque<std::string>>(&val_o.value().get());
  if (list == nullptr)
    return RespError{"WRONGTYPE Operation against a key holding the wrong kind of value"};
  int size = static_cast<int>(list->size());

  // process indexes

  int start = std::stoi(std::move(args[1]));
  if (start < 0) start = std::max(0, size + start);

  int end = std::min(std::stoi(std::move(args[2])), size-1);
  if (end < 0) end = std::max(0, size + end);

  if (start >= list->size() || start > end) return Array { std::ranges::subrange<std::deque<std::string>::const_iterator>{} };
 
  return Array { std::ranges::subrange(list->begin() + start, list->begin() + end + 1) };
}
