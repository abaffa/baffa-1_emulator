typedef struct linked_list
{
    void* info;
    struct linked_list *next;
} Linked_List;


Linked_List* ll_create (void);

Linked_List* ll_insert (Linked_List* ll, void* info);

Linked_List* ll_insert_sorted (Linked_List* ll, void* info, int (*cmp)(void *, void *));

Linked_List* ll_remove_first (Linked_List* ll);

Linked_List* ll_remove (Linked_List* ll, void* info, int (*cmp)(void *, void *));

void ll_print (Linked_List* ll, void (*print)(void *));

Linked_List* ll_find (Linked_List* ll, void* info, int (*cmp)(void *, void *));

int ll_equal (Linked_List* ll1, Linked_List* ll2, int (*cmp)(void *, void *));

int ll_empty (Linked_List* ll);

void ll_free (Linked_List* ll, void (* ffree)(void *));

int ll_size(Linked_List* ll);