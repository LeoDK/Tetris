#include "list.h"
#include <stdlib.h>
#include <stdio.h>

void init_List (struct List **l)
{
    *l = malloc (sizeof(struct List));
    (**l).first = NULL;
    (**l).last = NULL;
    (**l).size = 0;
}

struct Tetromino* get (struct List* l, int i)
{
    if (i>=l->size)
        return NULL;
    struct Elem* ret = l->first;
    for (int j=1; j<=i; j++)
    {
        ret = ret->next;
    }
    return ret->tetr;
}

void append (struct List *l, struct Tetromino *tetr)
{
    struct Elem* toadd = malloc (sizeof(struct Elem));
    if (l->size == 0)
        l->first = toadd;
    toadd->tetr = tetr;
    toadd->next = NULL;
    if (l->last != NULL)
        l->last->next = toadd;
    l->last = toadd;
    l->size += 1;
}

void pop (struct List *l, int index)
{
    struct Elem *prev = NULL;
    struct Elem *to_remove = l->first;
    for (int i=0; i<index; i++)
    {
        prev = to_remove;
        to_remove = to_remove->next;
    }
    if (prev == NULL) // first elem
    {
        l->first = to_remove->next;
    } else {
        prev->next = to_remove->next;
    }
    free(to_remove);
    l->size -= 1;
}

void destroy (struct List *l)
{
    struct Elem *curr = l->first;
    struct Elem *next = NULL;
    while (curr != NULL)
    {
        next = curr->next;
        free(curr);
        curr = next;
    }
    free(l);
}

bool is_in (struct List *l, struct Tetromino *tetr)
{
    struct Elem *curr = l->first;
    while (curr != NULL)
    {
        if (curr->tetr == tetr)
            return true;
        curr = curr->next;
    }
    return false;
}
