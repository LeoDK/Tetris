#include <stdbool.h>

struct Tetromino;

struct List
{
    struct Elem *first;
    struct Elem *last;
    int size;
};

struct Elem
{
    struct Tetromino *tetr;
    struct Elem *next;
};

void init_List (struct List **l);
struct Tetromino* get (struct List *l, int i);
void append (struct List *l, struct Tetromino *tetr);
void pop (struct List *l, int index);
void destroy (struct List *l);
bool is_in (struct List *l, struct Tetromino *tetr);
