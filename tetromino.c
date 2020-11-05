#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "tetromino.h"
#include "list.h"

#define SHAPE_GEN_SIZE 9

const char L_SHAPE [TETR_SIZE][TETR_SIZE] = { {' ', '*', ' ', ' '},
                                              {' ', '*', ' ', ' '},
                                              {' ', '*', '*', ' '},
                                              {' ', ' ', ' ', ' '} };

const char S_SHAPE [TETR_SIZE][TETR_SIZE] = { {' ', '*', ' ', ' '},
                                              {' ', '*', '*', ' '},
                                              {' ', ' ', '*', ' '},
                                              {' ', ' ', ' ', ' '} };

const char SQUARE_SHAPE [TETR_SIZE][TETR_SIZE] = { {' ', ' ', ' ', ' '},
                                                   {' ', '*', '*', ' '},
                                                   {' ', '*', '*', ' '},
                                                   {' ', ' ', ' ', ' '} };

const char I_SHAPE [TETR_SIZE][TETR_SIZE] = { {' ', '*', ' ', ' '},
                                              {' ', '*', ' ', ' '},
                                              {' ', '*', ' ', ' '},
                                              {' ', '*', ' ', ' '} };


//##########################################################################################
//################################ Memory management #######################################
//##########################################################################################

void init_Tetromino (struct Tetromino **tetr, int x, int y, char repr, const char shape[TETR_SIZE][TETR_SIZE], struct Board *caller)
{
    *tetr = malloc (sizeof(struct Tetromino));
    (**tetr).x = x;
    (**tetr).y = y;
    (**tetr).repr = repr;
    (**tetr).caller = caller;
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            if (shape[i][j] != ' ')
            {
                (**tetr).blocks[i][j] = malloc (sizeof(struct Block));
                (**tetr).blocks[i][j]->caller = *tetr;
                (**tetr).blocks[i][j]->x = x+j;
                (**tetr).blocks[i][j]->y = y+i;
                (**tetr).blocks[i][j]->repr = repr;
            } else {
                (**tetr).blocks[i][j] = NULL;
            }
        }
    }
}

void del_Tetromino (struct Tetromino *tetr)
{
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            if (tetr->blocks[i][j] != NULL)
                free(tetr->blocks[i][j]);
        }
    }
    free(tetr);
}


void init_Board (struct Board** board, int width, int height, WINDOW* scr)
{
    srand( time(NULL) );
    *board = malloc (sizeof(struct Board));
    (**board).width = width;
    (**board).height = height;
    (**board).blocks = malloc (height*sizeof(struct Block**));
    (**board).tetrominos = malloc (sizeof(struct List));
    for (int i=0; i<height; i++)
    {
        (**board).blocks[i] = malloc (width*sizeof(struct Block*));
        for (int j=0; j<width; j++)
        {
            (**board).blocks[i][j] = NULL;
        }
    }
    (**board).scr = scr;
    (**board).current = NULL;
}

void del_Board (struct Board* board)
{
    for (int i=0; i<board->height; i++)
    {
        free(board->blocks[i]);
    }
    free(board->blocks);
}

bool add_Tetromino (struct Board* board, int x, int y, int rot, char repr, const char shape [TETR_SIZE][TETR_SIZE])
{
    struct Tetromino *tetr;
    init_Tetromino (&tetr, x, y, repr, shape, board);
    //if (!rotate_Tetromino (tetr,rot))
    //    return false;
    append (board->tetrominos, tetr);
    update_BlockCoord (tetr);
    board->current = tetr;
    return true;
}

bool add_RandomTetromino (struct Board *board)
{
    int r = rand() % SHAPE_GEN_SIZE;
    switch (r)
    {
        case 0:
            return add_Tetromino (board, 0, 0, 0, 'A', L_SHAPE);
            break;
        case 1:
            return add_Tetromino (board, 0, 0, 1, 'B', L_SHAPE);
            break;
        case 2:
            return add_Tetromino (board, 0, 0, 2, 'C', L_SHAPE);
            break;
        case 3:
            return add_Tetromino (board, 0, 0, 3, 'D', L_SHAPE);
            break;
        case 4:
            return add_Tetromino (board, 0, 0, 0, 'E', S_SHAPE);
            break;
        case 5:
            return add_Tetromino (board, 0, 0, 1, 'F', S_SHAPE);
            break;
        case 6:
            return add_Tetromino (board, 0, 0, 0, 'G', I_SHAPE);
            break;
        case 7:
            return add_Tetromino (board, 0, 0, 1, 'H', I_SHAPE);
            break;
        case 8:
            return add_Tetromino (board, 0, 0, 0, 'I', SQUARE_SHAPE);
            break;
    }
    return false; // should not be called
}

void flush_Tetromino (struct Tetromino *tetr)
{
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            struct Block *block = tetr->blocks[i][j];
            if (block != NULL)
                tetr->caller->blocks[block->y][block->x] = NULL;
        }
    }
}

void update_Tetromino (struct Tetromino *tetr)
{
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            struct Block *block = tetr->blocks[i][j];
            if (block != NULL)
            {
                tetr->caller->blocks[block->y][block->x] = block;
            }
        }
    }
}

void copy (struct Block *(*arr1) [TETR_SIZE][TETR_SIZE], struct Block *(*arr2) [TETR_SIZE][TETR_SIZE])
{
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            (*arr1)[i][j] = (*arr2)[i][j];
        }
    }
}

//##########################################################################################
//###################################### Shifting ##########################################
//##########################################################################################

void update_BlockCoord (struct Tetromino* tetr)
{
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            struct Block* block = tetr->blocks[i][j];
            if (block != NULL)
            {
                block->x = tetr->x+j;
                block->y = tetr->y+i;
            }
        }
    }
}

bool check_collisions (struct Tetromino *tetr)
{
    // Out of borders
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            struct Block* block = tetr->blocks[i][j];
            if (block != NULL
             &&(block->x < 0 || block->x >= tetr->caller->width
                || block->y < 0 || block->y >= tetr->caller->height))
                return false;
        }
    }
    // Collisions with other tetrominos
    for (int k=0; k<tetr->caller->tetrominos->size; k++)
    {
        struct Tetromino *checking = get(tetr->caller->tetrominos, k);
        for (int i1=0; i1<TETR_SIZE; i1++)
        {
            for (int j1=0; j1<TETR_SIZE; j1++)
            {
                for (int i2=0; i2<TETR_SIZE; i2++)
                {
                    for (int j2=0; j2<TETR_SIZE; j2++)
                    {
                        if ( (checking != tetr)
                          && (checking->blocks[i1][j1] != NULL)
                          && (tetr->blocks[i2][j2] != NULL)
                          && (tetr->blocks[i2][j2]->x == checking->blocks[i1][j1]->x)
                          && (tetr->blocks[i2][j2]->y == checking->blocks[i1][j1]->y) )
                            return false;
                    }
                }
            }
        }
    }
    return true;
}

bool rotate_Tetromino (struct Tetromino *tetr, int rot)
{
    struct Block* tmp [TETR_SIZE][TETR_SIZE];
    struct Block* save [TETR_SIZE][TETR_SIZE];
    copy (&tmp, &tetr->blocks);
    copy (&save, &tetr->blocks);
    flush_Tetromino (tetr);
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            if (rot == 0) {
                tmp[i][j] = tetr->blocks[i][j];
            } else if (rot == 1) { // Trigonometric direction
                tmp[i][j] = tetr->blocks[j][TETR_SIZE-1-i];
            } else if (rot == 2) { // 180 degrees
                tmp[i][j] = tetr->blocks[TETR_SIZE-1-i][TETR_SIZE-1-j];
            } else { // Clockwise direction
                tmp[i][j] = tetr->blocks[TETR_SIZE-1-j][i];
            }
        }
    }
    copy (&tetr->blocks, &tmp);
    update_BlockCoord (tetr);
    if (check_collisions(tetr))
    {
        tetr->rot = (tetr->rot + rot)%4;
        update_Tetromino (tetr);
        return true;
    }
    // Else
    copy (&tetr->blocks, &save);
    update_BlockCoord (tetr);
    update_Tetromino (tetr);
    return false;
}

bool move_Tetromino (struct Tetromino *tetr, int shift_x, int shift_y)
{
    flush_Tetromino (tetr);
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            struct Block *block = tetr->blocks[i][j];
            if (block != NULL)
            {
                block->x += shift_x;
                block->y += shift_y;
            }
        }
    }
    if (check_collisions(tetr))
    {
        tetr->x += shift_x;
        tetr->y += shift_y;
        update_Tetromino (tetr);
        return true;
    }
    update_BlockCoord (tetr);
    update_Tetromino (tetr);
    return false;
}

bool down (struct Tetromino *tetr)
{
    move_Tetromino (tetr, 0, 1);
}

bool up (struct Tetromino *tetr)
{
    move_Tetromino (tetr, 0, -1);
}

bool left (struct Tetromino *tetr)
{
    move_Tetromino (tetr, -1, 0);
}

bool right (struct Tetromino *tetr)
{
    move_Tetromino (tetr, 1, 0);
}

//##########################################################################################
//###################################### Gameplay ##########################################
//##########################################################################################

bool is_stuck (struct Tetromino *tetr)
{
    if (down(tetr))
    {
        up(tetr);
        return false;
    }
    return true;
}

bool is_empty (struct Tetromino *tetr)
{
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            if (tetr->blocks[i][j] != NULL)
                return false;
        }
    }
    return true;
}

bool is_full (struct Block** row, int width)
{
    for (int j=0; j<width; j++)
    {
        if (row[j] == NULL)
            return false;
    }
    return true;
}

void delete_line (struct Block** row, int width)
{
    for (int k=0; k<width; k++)
    {
        struct Tetromino* caller = row[k]->caller;
        clear_Tetromino (caller);
        int i = row[k]->y - caller->y;
        int j = row[k]->x - caller->x;
        flush_Tetromino (caller);
        free (caller->blocks[i][j]);
        caller->blocks[i][j] = NULL;
        update_Tetromino (caller);
        draw_Tetromino (caller);
    }
}

// This function is ugly, TODO : write a better code for this function
void down_lines (struct Board *board, int line)
{
    struct List *checked;
    init_List(&checked);
    // Getting tetrominos to change and clearing screen
    for (int i=line; i>=0; i--)
    {
        for (int j=0; j<board->width; j++)
        {
            struct Block *block = board->blocks[i][j];
            if (block != NULL && !is_in(checked, block->caller))
            {
                append(checked, block->caller);
            }
            mvwaddch (board->scr, i+1, j+1, ' ');
        }
    }
    // Changing blocks
    for (int i=line; i>=0; i--)
    {
        for (int j=0; j<board->width; j++)
        {
            struct Block *block = board->blocks[i][j];
            if (block != NULL)
            {
                block->y += 1;
                board->blocks[i][j] = NULL;
                board->blocks[i+1][j] = block;
            }
        }
    }
    // Changing tetrominos
    struct Tetromino* tetr = NULL;
    for (int k=0; k<checked->size; k++)
    {
        tetr = get(checked, k);
        tetr->y += 1;
        draw_Tetromino (tetr);
    }
    destroy (checked);
}

bool delete_fullLines (struct Board *board)
{
    bool ret = false;
    for (int i=0; i<board->height; i++)
    {
        if (is_full(board->blocks[i],board->width))
        {
            delete_line (board->blocks[i], board->width);
            down_lines (board, i-1);
            ret = true;
        }
    }
    return ret;
}

void tidy_Tetrominos (struct Board* board)
{
    for (int k=0; k<board->tetrominos->size; k++)
    {
        struct Tetromino *tetr = get(board->tetrominos, k);
        if (is_empty(tetr))
        {
            pop (board->tetrominos, k);
            del_Tetromino (tetr);
        }
    }
}

//##########################################################################################
//###################################### Display ###########################################
//##########################################################################################

void clear_Tetromino (struct Tetromino *tetr)
{
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            struct Block* block = tetr->blocks[i][j];
            if (block != NULL)
            {
                mvwaddch(tetr->caller->scr, block->y+1, block->x+1, ' '); // +1 because of borders
            }
        }
    }
    move (tetr->caller->height+2, tetr->caller->width+2); // avoid to have cursor on tetromino
}

void draw_Tetromino (struct Tetromino *tetr)
{
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            struct Block* block = tetr->blocks[i][j];
            if (block != NULL)
            {
                mvwaddch(tetr->caller->scr, block->y+1, block->x+1, tetr->repr); // +1 because of borders
            }
        }
    }
    move (tetr->caller->height+2, tetr->caller->width+2);
}

void display_borders (struct Board* board)
{
    for (int x=0; x<board->width+2; x++)
    {
        mvwaddch(board->scr, 0, x, '$');
    }
    for (int x=0; x<board->width+2; x++)
    {
        mvwaddch(board->scr, board->height+1, x, '$');
    }
    for (int y=0; y<board->height+2; y++)
    {
        mvwaddch(board->scr, y, 0, '$');
    }
    for (int y=0; y<board->height+2; y++)
    {
        mvwaddch(board->scr, y, board->width+1, '$');
    }
    move (board->height+2, board->width+2);
}

// Debug
void display_BlockCoord (struct Tetromino *tetr)
{
    for (int i=0; i<TETR_SIZE; i++)
    {
        for (int j=0; j<TETR_SIZE; j++)
        {
            if (tetr->blocks[i][j] != NULL)
                wprintw(tetr->caller->scr, "caller = %c, i=%d, j=%d, x=%d, y=%d\n", tetr->repr, i, j, tetr->blocks[i][j]->x, tetr->blocks[i][j]->y);
        }
    }
    wprintw(tetr->caller->scr, "\n");
}
