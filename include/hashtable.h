typedef struct Ht_item Ht_item;
typedef struct HashTable HashTable;


Ht_item* ht_create_item(char* key, void* value);

HashTable* ht_create();

void ht_free_item(Ht_item* item, void(*ffree)(void *));

void ht_free_table(HashTable* table, void(*ffree)(void *));

void ht_handle_collision(HashTable* table, unsigned long index, Ht_item* item);

int ht_insert(HashTable* table, char* key, void* value);

void* ht_search(HashTable* table, char* key);

//void ht_print_search(HashTable* table, char* key);

//void ht_print(HashTable* table);