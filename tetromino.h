#include <stdbool.h>
#include <ncurses.h>
#define TETR_SIZE 4

extern const char L_SHAPE [TETR_SIZE][TETR_SIZE];
struct Tetromino;
struct Board;
struct List;

struct Block
{
    int x;
    int y;
    char repr;
    struct Tetromino *caller;
};

struct Tetromino
{
    // Coordinates of the top left corner
    int x;
    int y;
    char repr;
    int rot; // Rotation from 0 degree angle : 1 = 90 in trigonometric direction, 2 = 180, 3 = 90 in clockwise direction
    struct Block *blocks [TETR_SIZE][TETR_SIZE];
    struct Board* caller;
};

struct Board
{
    int width;
    int height;
    struct Block*** blocks;
    struct List* tetrominos;
    struct Tetromino *current; // The tetromino the player is using
    WINDOW* scr;
};

// Memory management
void init_Tetromino (struct Tetromino** tetr, int x, int y, char repr, const char shape[TETR_SIZE][TETR_SIZE], struct Board* caller);
void del_Tetromino (struct Tetromino* tetr);

void init_Board (struct Board** board, int width, int height, WINDOW* scr);
void del_Board (struct Board* board);

bool add_Tetromino (struct Board* board, int x, int y, int rot, char repr, const char shape[TETR_SIZE][TETR_SIZE]);
bool add_RandomTetromino (struct Board* board);

void flush_Tetromino (struct Tetromino *tetr); // remove tetromino blocks from board->blocks (before moving)
void update_Tetromino (struct Tetromino *tetr); // add tetromino blocks from board->blocks (after moving)
void copy (struct Block *(*arr1) [TETR_SIZE][TETR_SIZE], struct Block *(*arr2) [TETR_SIZE][TETR_SIZE]);

// Shifting
void update_BlockCoord (struct Tetromino *tetr); // update tetromino blocks coordinates according to tetromino's coordinates
bool check_collisions (struct Tetromino *tetr); // true if no collisions detected
bool rotate_Tetromino (struct Tetromino *tetr, int rot); // rot is using the same code as defined in struct Tetromino
bool move_Tetromino (struct Tetromino *tetr, int shift_x, int shift_y); // true if it was able to move, false else
bool down (struct Tetromino *tetr);
bool up (struct Tetromino *tetr);
bool left (struct Tetromino *tetr);
bool right (struct Tetromino *tetr);

// Gameplay
bool is_stuck (struct Tetromino *tetr); // check if tetromino is stuck
bool is_empty (struct Tetromino *tetr); // check if tetromino is empty
bool is_full (struct Block** row, int width); // check if line is full
void delete_line (struct Block** row, int width); // delete row i
bool delete_fullLines (struct Board *board); // routine to be called at the end of main loop, true if lines were deleted
void tidy_Tetrominos (struct Board *board); // remove empty tetrominos from board->tetrominos

// Display
void clear_Tetromino (struct Tetromino *tetr);
void draw_Tetromino (struct Tetromino *tetr);
void display_borders (struct Board *board);
void display_BlockCoord (struct Tetromino *tetr); // Debug
