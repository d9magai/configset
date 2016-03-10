#include "cached_hiredis.h"

redisContext *getRedisContext(request_rec *r)
{
    mytest_config *conf = reinterpret_cast<mytest_config*>(ap_get_module_config(r->server->module_config, &mytest_module));
    if (conf->context) {
        return conf->context;
    }

    timeval timeout;
    timeout.tv_sec = conf->timeout / 1000;
    timeout.tv_usec = (conf->timeout - (timeout.tv_sec * 1000)) * 1000;
    conf->context = redisConnectWithTimeout(conf->ip->c_str(), conf->port, timeout);
    if((!conf->context) || (conf->context->err != REDIS_OK)) {
        std::stringstream ss;
        ss << "Connection to REDIS failed to " << *(conf->ip) << ":" << conf->port;
        throw d9magai::internal_server_error(ss.str());
    }
    return conf->context;
}


