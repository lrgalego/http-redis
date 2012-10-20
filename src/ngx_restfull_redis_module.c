#ifndef DDEBUG
#define DDEBUG 1
#endif
#include "ddebug.h"

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <hiredis/hiredis.h>
#include <string.h>
#include <math.h>

#include "query_string.h"
#include "nginx_util.h"


static char *ngx_restfull_redis(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void *ngx_restfull_redis_create_loc_conf(ngx_conf_t *cf);

static int key_not_found(redisReply *reply);

static ngx_command_t  ngx_restfull_redis_commands[] = {
{ 
    ngx_string("restfull_redis"),
    NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
    ngx_restfull_redis,
    0,
    0,
    NULL },

    ngx_null_command
};

typedef struct{
  char * server;
  ngx_uint_t port;
} ngx_restfull_redis_loc_conf;

static ngx_http_module_t  ngx_restfull_redis_module_ctx = {
  NULL,                          /* preconfiguration */
  NULL,                          /* postconfiguration */

  NULL,                          /* create main configuration */
  NULL,                          /* init main configuration */

  NULL,                          /* create server configuration */
  NULL,                          /* merge server configuration */

  ngx_restfull_redis_create_loc_conf,                          /* create location configuration */
  NULL                           /* merge location configuration */
};


ngx_module_t ngx_restfull_redis_module = {
  NGX_MODULE_V1,
  &ngx_restfull_redis_module_ctx, /* module context */
  ngx_restfull_redis_commands,   /* module directives */
  NGX_HTTP_MODULE,               /* module type */
  NULL,                          /* init master */
  NULL,                          /* init module */
  NULL,                          /* init process */
  NULL,                          /* init thread */
  NULL,                          /* exit thread */
  NULL,                          /* exit process */
  NULL,                          /* exit master */
  NGX_MODULE_V1_PADDING
};

static void *
ngx_restfull_redis_create_loc_conf(ngx_conf_t *cf)
{
    ngx_restfull_redis_loc_conf  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_restfull_redis_loc_conf));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }
    conf->server = "localhost";
    conf->port = 11211;
    return conf;
}

static int number_of_digits(double n)
{
  return log10(n) + 1;
}

static ngx_int_t ngx_restfull_redis_handler(ngx_http_request_t *r)
{
  
  //TODO: deal with allocation problem - return 500
  ngx_chain_t   out;
  out.buf = create_response_buffer(r);;
  out.next = NULL;

  dd("command => %s", command(r));

  char* redis_command = command(r);
  define_content_type(r, "application/json; charset=utf-8");

  redisContext *c = redisConnect("127.0.0.1", 6379);

  if (c->err) {
    dd("Error: %s\n", c->errstr);
    write_to_buffer(out.buf, (u_char*) "Can't connect to redis", strlen("Can't connect to redis"));
  }

  Hash* params = get_params(r);

  redisReply *reply;

  if(!strcmp(redis_command, "get"))
  {
    reply = redisCommand(c,"GET %s", get(params, "key"));

    dd("Redis reply status: %d", reply->type);
    if(key_not_found(reply))
    {
      not_found(r, out.buf, (u_char*)"not found");
      return ngx_http_output_filter(r, &out);
    }
    dd("reply: %s", reply->str);

    u_char * result = copy(r->pool, reply->str, reply->len);
    write_to_buffer(out.buf, result, reply->len);
    write_header(r, NGX_HTTP_OK, reply->len);

  } else if (!strcmp(redis_command, "set"))
  {
    reply = redisCommand(c,"SET %s %s", get(params, "key"), get(params, "value"));

    dd("Redis reply status: %d", reply->type);
    if(key_not_found(reply))
    {
      not_found(r, out.buf, (u_char*)"not found");
      return ngx_http_output_filter(r, &out);
    }

    dd("Reply set %s -- %d", reply->str, reply->len);
    write_to_buffer(out.buf, (u_char*) reply->str, reply->len);
    write_header(r, NGX_HTTP_OK, reply->len);
  } else if(!strcmp(redis_command, "incr"))
  {
    reply = redisCommand(c,"INCR %s", get(params, "key"));

    dd("Redis reply status: %d", reply->type);

    int len = number_of_digits(reply->integer);

    dd("Reply INCR -- %d - len %d", (int)reply->integer, len);
    u_char* result = ngx_pcalloc(r->pool, sizeof(char)*len);
    sprintf((char*)result,"%d",(int)reply->integer);
    write_to_buffer(out.buf, result, len);
    write_header(r, NGX_HTTP_OK, len);
  }

  freeReplyObject(reply);
  ngx_free(params);
  redisFree(c);
  freeHash(params);
    
  return ngx_http_output_filter(r, &out);
}

static int key_not_found(redisReply *reply)
{
  return reply->type == REDIS_REPLY_NIL;
}


static char *ngx_restfull_redis(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
  ngx_http_core_loc_conf_t  *clcf;

  clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
  clcf->handler = ngx_restfull_redis_handler;

  return NGX_CONF_OK;
}
