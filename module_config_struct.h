#pragma once

#include <httpd.h>
#include "http_config.h"
#include <http_protocol.h>
#include <hiredis/hiredis.h>

/* モジュール設定情報(追加) */
struct mytest_config {
    redisContext *context;
    std::shared_ptr<std::string> ip;
    int port;
    int timeout;
};

