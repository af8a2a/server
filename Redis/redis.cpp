#include "redis.h"
TinyRedis::TinyRedis(EventLoop *loop, const Address &address) : loop_(loop), server_(loop_, address) {}

