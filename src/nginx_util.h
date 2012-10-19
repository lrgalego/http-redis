static void write_header(ngx_http_request_t *r, int status, int len)
{
  r->headers_out.status = status;
  r->headers_out.content_length_n = len;
  ngx_http_send_header(r);

}

static void write_to_buffer(ngx_buf_t* buffer, u_char* msg, int len)
{
 // u_char* msg_copy = (u_char*) malloc(len * sizeof(u_char));
  //memcpy(msg_copy, msg, len);
  buffer->pos = msg;
  buffer->last = msg + len;
  buffer->memory = 1;
  buffer->last_buf = 1;
}

static void not_found(ngx_http_request_t *r, ngx_buf_t* buffer, u_char* msg)
{
    write_to_buffer(buffer, msg, strlen((char*)msg));
    write_header(r,NGX_HTTP_NOT_FOUND,strlen((char*)msg));
}

static ngx_buf_t* create_response_buffer(ngx_http_request_t *r)
{
    return ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
}

static void define_content_type(ngx_http_request_t *r, char* content_type)
{
    r->headers_out.content_type.len = strlen(content_type);
    r->headers_out.content_type.data = (u_char *) content_type;
}
