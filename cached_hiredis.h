#pragma once

#include <sstream>
#include <memory>
#include <hiredis/hiredis.h>
#include "module_config_struct.h"
#include "serverexception.h"

extern "C" module AP_MODULE_DECLARE_DATA mytest_module;

redisContext *getRedisContext(request_rec *r);

