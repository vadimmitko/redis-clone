#ifndef REDIS_CLONE_COMMANDS_GET_H_
#define REDIS_CLONE_COMMANDS_GET_H_

#include <vector>
#include <string>

#include "../resp.h"
#include "../db.h"

RespValue cmd_get(std::vector<std::string>&& args, RedisDb& db);

#endif
