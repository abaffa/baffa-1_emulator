#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

#define CAPACITY 50000 // Size of the Hash Table

// Define the Hash Table Item here
struct Ht_item {
	char* key;
	void* value;
};


typedef struct LinkedList LinkedList;

// Define the Linkedlist here
struct LinkedList {
	Ht_item* item;
	LinkedList* next;
};

// Define the Hash Table here
struct HashTable {
	// Contains an array of pointers
	// to items
	Ht_item** items;
	LinkedList** overflow_buckets;
	int size;
	int count;
};



//##########################################

static LinkedList* allocate_list() {
	// Allocates memory for a Linkedlist pointer
	LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
	return list;
}

static LinkedList* linkedlist_insert(LinkedList* list, Ht_item* item) {
	// Inserts the item onto the Linked List
	if (!list) {
		LinkedList* head = allocate_list();
		head->item = item;
		head->next = NULL;
		list = head;
		return list;
	}

	else if (list->next == NULL) {
		LinkedList* node = allocate_list();
		node->item = item;
		node->next = NULL;
		list->next = node;
		return list;
	}

	LinkedList* temp = list;
	while (temp->next != NULL) {
		temp = temp->next;
	}

	LinkedList* node = allocate_list();
	node->item = item;
	node->next = NULL;
	temp->next = node;

	return list;
}

static Ht_item* linkedlist_remove(LinkedList* list, void(*ffree)(void *)) {
	// Removes the head from the linked list
	// and returns the item of the popped element
	if (!list)
		return NULL;
	if (!list->next)
		return NULL;
	LinkedList* node = list->next;
	LinkedList* temp = list;
	temp->next = NULL;
	list = node;
	Ht_item* it = NULL;
	memcpy(temp->item, it, sizeof(Ht_item));
	free(temp->item->key);
	ffree(temp->item->value);
	free(temp->item);
	free(temp);
	return it;
}

static void free_linkedlist(LinkedList* list, void(*ffree)(void *)) {
	LinkedList* temp = list;
	while (list) {
		temp = list;
		list = list->next;
		free(temp->item->key);
		free(temp->item->value);
		free(temp->item);
		free(temp);
	}
}

static LinkedList** create_overflow_buckets(HashTable* table) {
	// Create the overflow buckets; an array of linkedlists
	LinkedList** buckets = (LinkedList**)calloc(table->size, sizeof(LinkedList*));
	for (int i = 0; i < table->size; i++)
		buckets[i] = NULL;
	return buckets;
}

static void free_overflow_buckets(HashTable* table, void(*ffree)(void *)) {
	// Free all the overflow bucket lists
	LinkedList** buckets = table->overflow_buckets;
	for (int i = 0; i < table->size; i++)
		free_linkedlist(buckets[i], free);
	free(buckets);
}



//########################################################

unsigned long hash_function(char* str) {
	unsigned long i = 0;
	for (int j = 0; str[j]; j++)
		i += str[j];
	return i % CAPACITY;
}

Ht_item* ht_create_item(char* key, void* value) {
	// Creates a pointer to a new hash table item
	Ht_item* item = (Ht_item*)malloc(sizeof(Ht_item));
	item->key = (char*)malloc(strlen(key) + 1);

	strcpy(item->key, key);
	item->value = value;

	return item;
}


HashTable* ht_create() {
	// Creates a new HashTable
	HashTable* table = (HashTable*)malloc(sizeof(HashTable));
	table->size = CAPACITY;
	table->count = 0;
	table->items = (Ht_item**)calloc(table->size, sizeof(Ht_item*));
	for (int i = 0; i < table->size; i++)
		table->items[i] = NULL;
	table->overflow_buckets = create_overflow_buckets(table);

	return table;
}

void ht_free_item(Ht_item* item, void(*ffree)(void *)) {
	// Frees an item
	free(item->key);
	ffree(item->value);
	free(item);
}

void ht_free_table(HashTable* table, void(*ffree)(void *)) {
	// Frees the table
	for (int i = 0; i < table->size; i++) {
		Ht_item* item = table->items[i];
		if (item != NULL)
			ht_free_item(item, ffree);
	}

	free_overflow_buckets(table, free);
	free(table->items);
	free(table);
}

void ht_handle_collision(HashTable* table, unsigned long index, Ht_item* item) {
	LinkedList* head = table->overflow_buckets[index];

	if (head == NULL) {
		// We need to create the list
		head = allocate_list();
		head->item = item;
		head->next = NULL;
		table->overflow_buckets[index] = head;
		return;
	}
	else {
		// Insert to the list
		table->overflow_buckets[index] = linkedlist_insert(head, item);
		return;
	}
}

int ht_insert(HashTable* table, char* key, void* value) {

	// Create the item
	Ht_item* item = ht_create_item(key, value);

	// Compute the index
	unsigned long index = hash_function(key);

	Ht_item* current_item = table->items[index];

	if (current_item == NULL) {
		// Key does not exist.
		if (table->count == table->size) {
			// Hash Table Full
			printf("Insert Error: Hash Table is full\n");
			// Remove the create item
			//ht_free_item(item);
			return 0;
		}

		// Insert directly
		table->items[index] = item;
		table->count++;
	}

	// Scenario 1: We only need to update value
	else if (strcmp(current_item->key, key) == 0) {
		table->items[index]->value = value;
	}

	// Scenario 2: Collision
	else {
		ht_handle_collision(table, index, item);
	}

	return 1;
}

void* ht_search(HashTable* table, char* key) {
	// Searches the key in the hashtable
	// and returns NULL if it doesn't exist
	int index = hash_function(key);
	Ht_item* item = table->items[index];
	LinkedList* head = table->overflow_buckets[index];

	// Ensure that we move to items which are not NULL
	while (item != NULL) {
		if (strcmp(item->key, key) == 0)
			return item->value;
		if (head == NULL)
			return NULL;
		item = head->item;
		head = head->next;
	}
	return NULL;
}

/*
void ht_print_search(HashTable* table, char* key) {
	char* val;
	if ((val = ht_search(table, key)) == NULL) {
		printf("%s does not exist\n", key);
		return;
	}
	else {
		printf("Key:%s, Value:%s\n", key, val);
	}
}

void ht_print(HashTable* table) {
	printf("\n-------------------\n");
	for (int i = 0; i < table->size; i++) {
		if (table->items[i]) {
			printf("Index:%d, Key:%s, Value:%s", i, table->items[i]->key, table->items[i]->value);
			if (table->overflow_buckets[i]) {
				printf(" => Overflow Bucket => ");
				LinkedList* head = table->overflow_buckets[i];
				while (head) {
					printf("Key:%s, Value:%s ", head->item->key, head->item->value);
					head = head->next;
				}
			}
			printf("\n");
		}
	}
	printf("-------------------\n");
}

*/