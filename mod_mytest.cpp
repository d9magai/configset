#include <string>
#include <memory>
#include <httpd.h>
#include "http_config.h"
#include <http_protocol.h>
#include <http_log.h>
#include <apr_strings.h>
#include <hiredis/hiredis.h>

extern "C" module AP_MODULE_DECLARE_DATA mytest_module;

APLOG_USE_MODULE(mytest);

/* モジュール設定情報(追加) */
struct mytest_config {
    redisContext *context;
    std::shared_ptr<std::string> ip;
    int port;
};

/* 設定情報の生成・初期化(追加) */
static void *create_per_server_config(apr_pool_t *pool, server_rec *s)
{
    mytest_config *cfg = reinterpret_cast<mytest_config*>(apr_pcalloc(pool, sizeof(mytest_config)));
    // default value
    cfg->ip = std::make_shared<std::string>("127.0.0.1");
    return cfg;
}

/* ハンドラ本体 */
static int mytest_handler(request_rec *r)
{
    if (strcmp(r->handler, "mytest")) {
        return DECLINED;
    }
    /* 設定情報取得(追加) */
    mytest_config *cfg = reinterpret_cast<mytest_config*>(ap_get_module_config(r->server->module_config, &mytest_module));
    r->content_type = "text/html";

    /* 設定を出力(変更) */
    if (!r->header_only) {
        ap_rputs(cfg->ip->c_str(), r);
        /* ログ出力 */
        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "request : %s", r->uri);
    }
    return OK;
}

/* ハンドラ登録 */
static void mytest_register_hooks(apr_pool_t *p)
{
    ap_hook_handler(mytest_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

/* 設定情報読み込み(追加) */
static const char *set_ip_address(cmd_parms *parms, void *mconfig, const char *arg)
{
    mytest_config *cfg = reinterpret_cast<mytest_config*>(ap_get_module_config(parms->server->module_config, &mytest_module));
    cfg->ip = std::make_shared<std::string>(arg);
    return 0;
}

static const char *set_port(cmd_parms *parms, void *in_struct_ptr, const char *arg)
{
    int port;
    if (sscanf(arg, "%d", &port) != 1) {
        return "RedisPort argument must be an integer representing the port number";
    }

    mytest_config *cfg = reinterpret_cast<mytest_config*>(ap_get_module_config(parms->server->module_config, &mytest_module));
    cfg->port = port;
    return NULL;
}

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
        0
        },
    };

/* モジュール・フック定義(create_per_dir_config,mytest_cmds,追加) */
module AP_MODULE_DECLARE_DATA mytest_module =
    {
    STANDARD20_MODULE_STUFF,
    NULL, /* create per-dir    config structures */
    NULL, /* merge  per-dir    config structures */
    create_per_server_config, /* create per-server config structures */
    NULL, /* merge  per-server config structures */
    mytest_cmds, /* table of config file commands       */
    mytest_register_hooks
    /* register hooks                      */
    };

