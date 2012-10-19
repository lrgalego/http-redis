typedef struct 
{
  char* key;
  char* value;
} Entry;

typedef struct 
{
  Entry* entries;
  int size;
} Hash;

static char* get(Hash hash, char* key)
{
    int i;
    for(i=0; i<hash.size; i++)
    {
        if(!strcmp(key, hash.entries[i].key)){
            return hash.entries[i].value;
        }
    }
    return NULL;
}

static void freeHash(Hash hash)
{
/*
  Wew, I have to study how to free things into nginx

  int i;
  for(i=0; i<hash.size; i++)
  {
    ngx_free(hash.entries[i].key);
    ngx_free(hash.entries[i].value);
  }
  ngx_free(hash.entries);
*/
}