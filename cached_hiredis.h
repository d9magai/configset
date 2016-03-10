#pragma once

#include <sstream>
#include <memory>
#include <httpd.h>
#include "http_config.h"
#include <http_protocol.h>
#include <hiredis/hiredis.h>
#include "module_config_struct.h"
#include "serverexception.h"
#include "module_declare_data.h"

extern "C" module AP_MODULE_DECLARE_DATA mytest_module;

redisContext *getRedisContext(request_rec *r);

