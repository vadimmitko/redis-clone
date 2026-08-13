#ifndef REDIS_CLONE_COMMANDS_PING_H_
#define REDIS_CLONE_COMMANDS_PING_H_

#include <vector>
#include <string>

#include "../resp.h"
#include "../db.h"

RespValue cmd_ping(std::vector<std::string>&& args, RedisDb& db);

#endif 
