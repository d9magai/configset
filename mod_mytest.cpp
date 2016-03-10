#include <sstream>
#include <memory>
#include <httpd.h>
#include "http_config.h"
#include <http_protocol.h>
#include <http_log.h>
#include <apr_strings.h>
#include <hiredis/hiredis.h>
#include "serverexception.h"
#include "module_declare_data.h"
#include "module_config_struct.h"
#include "cached_hiredis.h"

extern "C" module AP_MODULE_DECLARE_DATA mytest_module;

APLOG_USE_MODULE(mytest);

/* 設定情報の生成・初期化(追加) */
static void *create_per_server_config(apr_pool_t *pool, server_rec *s)
{
    mytest_config *cfg = reinterpret_cast<mytest_config*>(apr_pcalloc(pool, sizeof(mytest_config)));
    // default value
    cfg->ip = std::make_shared<std::string>("127.0.0.1");
    cfg->port = 6379;
    cfg->timeout = 1500;
    return cfg;
}

/* ハンドラ本体 */
static int mytest_handler(request_rec *r)
{
    if (strcmp(r->handler, "mytest")) {
        return DECLINED;
    }

    try {
        r->content_type = "text/html";
        /* 設定を出力(変更) */
        if (!r->header_only) {
            /* ログ出力 */
            redisReply *resp = reinterpret_cast<redisReply*>(redisCommand(getRedisContext(r), "GET 1"));
            ap_rputs(resp->str, r);
            ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "request : %s", r->uri);
        }
    } catch (d9magai::internal_server_error& e) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, APLOG_MODULE_INDEX, r, e.what());
        return HTTP_INTERNAL_SERVER_ERROR;
    } catch (std::exception& e) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, APLOG_MODULE_INDEX, r, e.what());
        return HTTP_INTERNAL_SERVER_ERROR;
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
    if (strlen(arg) == 0) {
        return "RedisIPAddress argument must be a string representing a server address";
    }

    mytest_config *cfg = reinterpret_cast<mytest_config*>(ap_get_module_config(parms->server->module_config, &mytest_module));
    cfg->ip = std::make_shared<std::string>(arg);
    return NULL;
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

static const char * set_timeout(cmd_parms *parms, void *in_struct_ptr, const char *arg)
{
    int timeout;
    if (sscanf(arg, "%d", &timeout) != 1) {
        return "RedisTimeout argument must be an integer representing the timeout setting for a connection";
    }

    mytest_config *cfg = reinterpret_cast<mytest_config*>(ap_get_module_config(parms->server->module_config, &mytest_module));
    cfg->timeout = timeout;
    return NULL;
}

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


