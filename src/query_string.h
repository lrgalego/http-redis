#include "hash.h"

static int count_params(char* query_string, int len)
{
  int count = 1;
  while(*query_string)
  { 
    if (*query_string++ == '&')
    {
      count++;
    }
  }
  return count;
}

static Entry new_entry(char* arg)
{
    Entry e;
    e.key = strtok(arg,"=");
    e.value = strtok(NULL,"=");
    dd( "entry: (%s|%s)", e.key, e.value );
    return e;
}

static Hash parse_query_string(char* args, int len)
{
    int args_count = count_params(args,len);
    Entry* entries = (Entry *) malloc(args_count*sizeof(Entry));
    
    int i;
    int index = 0;
    for(i=0; i<args_count; i++)
    {
        char* pos = strchr(args, '&');
        if(pos == NULL)
        {
            index = len;
        }
        else
        {
            index = pos - args;
        }
        dd( "index: %d", index );
        char * arg = malloc((index + 1) * sizeof(char) );
        strncpy(arg, args, index );
        arg[index] = '\0'; 

        entries[i] = new_entry(arg);
        args += index + 1;
        len -= index + 1;
    }
    Hash hash;
    hash.entries = entries;
    hash.size = args_count;
    return hash;
}

static Hash get_params(ngx_http_request_t *r)
{
  char *key = malloc(r->args.len*sizeof(char));
  memcpy(key,r->args.data, r->args.len);
  return parse_query_string(key, r->args.len);
}

static char* command(ngx_http_request_t *r)
{
    char* path = (char *) r->uri.data;
    char* pos = strchr(path, '?');
    int size = pos - path;
    char* command = malloc(size * sizeof(char));
    strncpy(command, path+1, size-1);
    command[size] = '\0';
    return command;
}