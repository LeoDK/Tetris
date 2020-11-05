#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "tetromino.h"
#include "list.h"

#define REFRESH_PERIOD 500000
#define KEYBOARD_PERIOD 1000

#define BOARD_WIDTH 15
#define BOARD_HEIGHT 20

struct arg_struct {
    struct Board *board;
    bool *play;
};

// main loop simulating gravity and deleting complete rows
void* gravity (void* arguments)
{
    struct arg_struct *args = arguments;
    struct Board *board = args->board;
    bool *play = args->play;

    add_RandomTetromino (board);
    struct Tetromino *current;
    while (*play)
    {
        if (is_stuck(board->current))
        {
            add_RandomTetromino (board);
        } else {
            clear_Tetromino (board->current);
            down (board->current);
        }
        draw_Tetromino (board->current);
        display_borders (board);
        tidy_Tetrominos (board);
        if (delete_fullLines (board))
            add_RandomTetromino (board);
        usleep (REFRESH_PERIOD);
    }
    return NULL;
}

// checking keyboard events
void* keyboard_events (void *arguments)
{
    struct arg_struct *args = arguments;
    struct Board *board = args->board;
    bool *play = args->play;
    int c = 0;

    while (*play)
    {
        c = getch();
        if (c=='q')
        {
            *play = false;
        } else if (c==KEY_LEFT && !is_stuck(board->current)) {
            clear_Tetromino (board->current);
            left (board->current);
            draw_Tetromino (board->current);
        } else if (c==KEY_RIGHT && !is_stuck(board->current)) {
            clear_Tetromino (board->current);
            right (board->current);
            draw_Tetromino (board->current);
        } else if (c==KEY_DOWN) {
            clear_Tetromino (board->current);
            down (board->current);
            draw_Tetromino (board->current);
        } else if (c=='a') {
            clear_Tetromino (board->current);
            rotate_Tetromino (board->current,1);
            draw_Tetromino (board->current);
        } else if (c=='z') {
            clear_Tetromino (board->current);
            rotate_Tetromino (board->current,3);
            draw_Tetromino (board->current);
        }
        usleep (KEYBOARD_PERIOD);
    }
}

int main (void)
{
    initscr();
    keypad(stdscr, true);
    timeout(0);
    struct Board *board;
    init_Board (&board, BOARD_WIDTH, BOARD_HEIGHT, stdscr);

    bool play = true;
    struct arg_struct args;
    args.board = board;
    args.play = &play;
    pthread_t t1;
    pthread_t t2;
    pthread_create (&t1, NULL, gravity, (void *)&args);
    pthread_create (&t2, NULL, keyboard_events, (void *)&args);

    pthread_join (t1, NULL);
    pthread_join (t2, NULL);

    del_Board (board);
    endwin();
    return 0;
}
