#ifndef REDIS_CLONE_COMMANDS_RPUSH_H_
#define REDIS_CLONE_COMMANDS_RPUSH_H_

#include <vector>
#include <string>

#include "../resp.h"
#include "../db.h"

RespValue cmd_rpush(std::vector<std::string>&& args, RedisDb& db);

#endif


