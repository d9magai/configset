#include <httpd.h>
#include "http_config.h"
#include <http_protocol.h>
#include <http_log.h>
#include <apr_strings.h>

extern "C" module AP_MODULE_DECLARE_DATA mytest_module;

APLOG_USE_MODULE (mytest);

/* モジュール設定情報(追加) */
struct mytest_config {
  char  * message;
}mytest_config;

/* 設定情報の生成・初期化(追加) */
static void * create_per_dir_config (apr_pool_t *pool, char *arg)
{
  void * buf = apr_pcalloc(pool, sizeof(mytest_config));
  struct mytest_config *cfg = (struct mytest_config*)buf;
  // default value
  cfg->message    = "The sample page by mod_mytest.c";
  return buf;
}

/* ハンドラ本体 */
static int mytest_handler(request_rec *r)
{
    if (strcmp(r->handler, "mytest")) {
        return DECLINED;
    }
    /* 設定情報取得(追加) */
    struct mytest_config *cfg = (struct mytest_config*)ap_get_module_config(r->per_dir_config, &mytest_module);
    r->content_type = "text/html";      

    /* 設定を出力(変更) */
    if (!r->header_only) {
        ap_rputs(cfg->message, r);
        /* ログ出力 */
        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, "request : %s",r->uri);
    }
    return OK;
}

/* ハンドラ登録 */
static void mytest_register_hooks(apr_pool_t *p)
{
    ap_hook_handler(mytest_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

/* 設定情報読み込み(追加) */
static const char *cmd_mytest_message (cmd_parms *parms, void *mconfig, char *arg){
  struct mytest_config *cfg = (struct mytest_config*)mconfig;
  cfg->message = apr_pstrdup(parms->pool,arg);
  return 0;
}

/* 設定情報フック定義(追加) */
static const command_rec mytest_cmds[] = {
  {
    "MytestMessage",                    /* ディレクティブ名 */
    cmd_mytest_message,  /* コールバック関数 */
    0,                                  /* 0で良い */
    OR_ALL,                             /* ディレクティブが現れるべき位置(何処でもの意) */
    TAKE1,                           /* 引数の数(一つの意) */
    "Custom message."                   /* エラーメッセージ(特に気にしなくても良い) */
  },
  {0},
};

/* モジュール・フック定義(create_per_dir_config,mytest_cmds,追加) */
module AP_MODULE_DECLARE_DATA mytest_module = {
    STANDARD20_MODULE_STUFF, 
    create_per_dir_config, /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    mytest_cmds,           /* table of config file commands       */
    mytest_register_hooks  /* register hooks                      */
};

