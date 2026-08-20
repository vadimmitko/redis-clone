#include "commands/rpush.h"
#include "db.h"
#include "resp.h"
#include <variant>
#include <vector>

RespValue cmd_rpush(std::vector<std::string> &&args, RedisDb &db) {
  if (args.size() != 2) return RespError { "wrong number of arguments" };

  RedisObject* l = db.get_mutable(args[0]);

  if (l == nullptr) {
    std::vector<ListObject> l;
    l.push_back(std::move(args[1]));
    
    db.set_value(std::move(args[0]), std::move(l));

    return Integer { 1 };
  }

  auto* list = std::get_if<std::vector<ListObject>>(l);
  if (!list)
      return RespError{"WRONGTYPE Operation against a key holding the wrong kind of value"};
  list->push_back(std::move(args[1]));
  return Integer{static_cast<int64_t>(list->size())};
}
