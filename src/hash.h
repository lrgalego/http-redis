typedef struct 
{
  char* key;
  char* value;
} Entry;

typedef struct 
{
  Entry* entries;
  int size;
  char* (*get)( const void *, char* );
} Hash;

static char* get( const void* self, char* key);

Hash* new_hash( void* (*allocator)(size_t))
{
  Hash *h = (Hash*) allocator(sizeof(Hash));
  h->get = &get;
  return h;
}

static char* get( const void* self, char* key)
{
    Hash* hash = (Hash*)self;
    int i;
    for(i=0; i<hash->size; i++)
    {
        if(!strcmp(key, hash->entries[i].key)){
            return hash->entries[i].value;
        }
    }
    return NULL;
}
