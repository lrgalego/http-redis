#ifndef DDEBUG
#define DDEBUG 1
#endif
#include "ddebug.h"

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <hiredis/hiredis.h>


static char *ngx_restfull_redis(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void *ngx_restfull_redis_create_loc_conf(ngx_conf_t *cf);

static ngx_command_t  ngx_restfull_redis_commands[] = {

  { ngx_string("restfull_redis"),
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


//static u_char  buffer[] = "glu glu";

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


static ngx_int_t ngx_restfull_redis_handler(ngx_http_request_t *r)
{
  ngx_buf_t    *b;
  ngx_chain_t   out;

  r->headers_out.content_type.len = sizeof("text/html") - 1;
  r->headers_out.content_type.data = (u_char *) "text/html";

  b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

  //TODO: deal with allocation problem - return 500

  out.buf = b;
  out.next = NULL;


  ngx_restfull_redis_loc_conf *cfg;
  cfg = ngx_http_conf_get_module_loc_conf(r, ngx_restfull_redis_module);

  //redisReply *reply;

  redisContext *c = redisConnect("127.0.0.1", 6379);

  if (c->err) {
    printf("Error: %s\n", c->errstr);
    // handle error
  }

  redisReply *reply = redisCommand(c, "GET foo");
  
  u_char* result =  (u_char*) reply->str;

  b->pos = result;
  b->last = result + sizeof(result) -1;
  b->memory = 1;
  b->last_buf = 1;

  r->headers_out.status = NGX_HTTP_OK;
  r->headers_out.content_length_n = sizeof(result)-1;
  ngx_http_send_header(r);

  return ngx_http_output_filter(r, &out);
}


static char *ngx_restfull_redis(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
  ngx_http_core_loc_conf_t  *clcf;

  clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
  clcf->handler = ngx_restfull_redis_handler;

  return NGX_CONF_OK;
}
