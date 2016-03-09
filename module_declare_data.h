#pragma once

#include <httpd.h>
#include "http_config.h"
#include <http_protocol.h>

extern "C" module AP_MODULE_DECLARE_DATA mytest_module;

/* 設定情報の生成・初期化(追加) */
static void *create_per_server_config(apr_pool_t *pool, server_rec *s);

/* ハンドラ登録 */
static void mytest_register_hooks(apr_pool_t *p);

/* 設定情報読み込み(追加) */
static const char *set_ip_address(cmd_parms *parms, void *mconfig, const char *arg);

static const char *set_port(cmd_parms *parms, void *in_struct_ptr, const char *arg);

static const char *set_timeout(cmd_parms *parms, void *in_struct_ptr, const char *arg);

/* 設定情報フック定義(追加) */
static const command_rec mytest_cmds[] =
    {
        {
        "RedisIPAddress", set_ip_address, 0, RSRC_CONF, TAKE1, "The address of the REDIS server."
        },
        {
        "RedisPort", set_port, 0, RSRC_CONF, TAKE1, "The port number of the REDIS server."
        },
        {
        "RedisTimeout", set_timeout, 0, RSRC_CONF, TAKE1, "The timeout for connections to the REDIS server"
        },
        {
        0
        },
    };

/* モジュール・フック定義 */
module AP_MODULE_DECLARE_DATA mytest_module =
    {
    STANDARD20_MODULE_STUFF,
    NULL,                     /* create per-dir    config structures */
    NULL,                     /* merge  per-dir    config structures */
    create_per_server_config, /* create per-server config structures */
    NULL,                     /* merge  per-server config structures */
    mytest_cmds,              /* table of config file commands       */
    mytest_register_hooks     /* register hooks                      */
    };

