#include "commands/rpush.h"
#include "db.h"
#include "resp.h"
#include <variant>
#include <vector>

RespValue cmd_rpush(std::vector<std::string> &&args, RedisDb &db) {
  if (args.size() < 2) return RespError { "wrong number of arguments" };

  RedisObject* l_ptr = db.get_mutable(args[0]);
  size_t n = args.size();

  if (l_ptr == nullptr) {
    std::vector<ListObject> l;
    l.reserve(n-1);

    for (size_t i = 1; i < n; i++) l.emplace_back(std::move(args[i]));

    db.set_value(std::move(args[0]), std::move(l));
    
    return Integer { static_cast<int64_t>(n-1) };
  }

  auto* list = std::get_if<std::vector<ListObject>>(l_ptr);
  if (!list) return RespError{ "WRONGTYPE Operation against a key holding the wrong kind of value" };

  list->reserve(list->size() + n-1);
  for (size_t i = 1; i < n; i++) list->emplace_back(std::move(args[i]));

  return Integer{ static_cast<int64_t>(list->size()) };
}
