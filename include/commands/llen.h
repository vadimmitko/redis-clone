#ifndef REDIS_CLONE_COMMANDS_LLEN_H_
#define REDIS_CLONE_COMMANDS_LLEN_H_

#include <vector>
#include <string>

#include "../resp.h"
#include "../db.h"

RespValue cmd_llen(std::vector<std::string>&& args, RedisDb& db);

#endif

