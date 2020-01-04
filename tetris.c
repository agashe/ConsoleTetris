/**
 * Tetris 0.5 (6/9/2019)
 * Auther: Mohamed Yousef (modi401@hotmail.com)
 * License: Open Source.
 * 
 * ToDo:
 *  - Control speed according to score.
 *  - Highest Score.
 *  - Add Load / Save option.
 *  - Fixing or writing new code for COLLISION !!!
 */

/* ************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

#pragma comment(lib, "user32.lib")

/* ************************************** */

#define ROW 20
#define COL 30

#define UP    1
#define RIGHT 2
#define LEFT  3
#define DOWN  4

#define BOTTOM 5
#define ORIGIN 0
#define COMPLETE 28
#define PERIOD 250
#define LOSE 6
#define ENTER 10

typedef struct {
    int x;
    int y;
} peace;

typedef struct  {
    peace peaces[4];
    char shape;
    char status;
    char direction;
} block;

enum {y, x};
enum {r, l, d};
enum {true, false};
enum {I, O, T, S, L, J, Z};

char blocks_yx[7][4][2] = {
    {{0, 15}, {1, 15}, {2, 15}, {3, 15}}, // I
    {{0, 15}, {1, 15}, {0, 16}, {1, 16}}, // O
    {{0, 15}, {1, 15}, {2, 15}, {1, 16}}, // T
    {{0, 14}, {0, 15}, {1, 14}, {1, 13}}, // S
    {{0, 15}, {1, 15}, {2, 15}, {2, 16}}, // L
    {{0, 15}, {1, 15}, {2, 15}, {2, 14}}, // J
    {{0, 14}, {0, 13}, {1, 14}, {1, 15}}  // Z
};

char game_status = false;
int  game_score  = 0;

/* ************************************** */

char welcome_message(){
    char answer = 0;

    puts(" ######### ######### ######### ########  ######### ######### ");
    puts(" ######### ##        ######### ###   ###    ###    ##        ");
    puts("    ###    #########    ###    ########     ###    ######### ");
    puts("    ###    ##           ###    ###   ###    ###           ## ");
    puts("    ###    #########    ###    ###   ### ######### ######### \n");
    puts("             2019 (C) CREATED BY MOHAMED YOUSEF              \n");
    puts("                   press ENTER to start                      ");

    // Check user's input.
    scanf("%c", &answer);
    
    if (answer == ENTER) {
        game_status = true;
        return true;
    } else {
        return false;
    }
}

void init_map(char map[ROW][COL]){
    int i, j;

    for (i = 0;i < ROW;i++) {
        for (j = 0;j < COL;j++) {
            if (i == (ROW-1)) {
                map[i][j] = '#';
            } else {
                if (j == 0 || j == (COL-1)) {
                    map[i][j] = '#';
                } else {
                    map[i][j] = ' ';
                }
            }
        }
    }
}

char check_collision(char map[ROW][COL], block *main_block) {
    char i, j, max[3] = {0}, flag = 0, x = 0, y = 0;

    // First: check the outer borders.
    for (i = 0;i < 4;i++) {    
        if ((main_block->peaces[i].x + 1) == (COL-1) && main_block->direction == RIGHT) {
            return false;
        }        
        else if ((main_block->peaces[i].x - 1) == 0 && main_block->direction == LEFT) {
            return false;
        }
        else if ((main_block->peaces[i].y + 1) == (ROW-1) && main_block->direction == DOWN) {
            return BOTTOM;
        }
    }

    //Second: check the surronded blockes.

    //1. get the max peace in each direction.
    for (j = 0;j < 3;j++) {
        if (main_block->peaces[j].x < main_block->peaces[j+1].x) {
            max[r] = j+1;
            max[l] = j;
        }
        // if (main_block->peaces[j].x > main_block->peaces[j+1].x) {
        //     max[l] = j+1;
        // }
        if (main_block->peaces[j].y < main_block->peaces[j+1].y) {
            max[d] = j+1;
        }
    }

    /*
     * save y/x for down direction, beacuse it needs more conditions
     * this will enhance readability.
     */
    y = main_block->peaces[max[d]].y;
    x = main_block->peaces[max[d]].x;
    
    //2. check that no peaces around the block, to prevent collision.
    if (map[main_block->peaces[max[r]].y][main_block->peaces[max[r]].x+1] == '#' && main_block->direction == RIGHT) {
        return false;
    }        
    // else if (map[main_block->peaces[max[l]].y][main_block->peaces[max[l]].x-1] == '#' && main_block->direction == LEFT) {
    //     return false;
    // }
    else if ((map[y+1][x] == '#' ||
             (map[y][x+1] == '#' && map[y+1][x+1] == '#') ||
             (map[y][x-1] == '#' && map[y+1][x-1] == '#')) &&
             main_block->direction == DOWN) {

                return (main_block->peaces[ORIGIN].y == 1)? LOSE : BOTTOM;
    }

    return true;
}

void check_line(char map[ROW][COL]) {
    int i, j, k, count, rows = 0;

    // First: delete completed rows.
    for (i = ROW-2;i > 0;i--) {
        count = 0;
        for (j = 1;j < COL-1;j++) {
            if (map[i][j] == '#') {
                count++;
            }
        }

        if (count == COMPLETE) {
            for (k = 1;k < COL-1;k++) {
                map[i][k] = ' ';
            }

            // Get number of rows to be shifted.
            rows++;
        }
    }

    // Second: Shift all remaining peaces by "rows" steps down.
    for (j = 0;j < rows;j++) {
        for (i = ROW-2;i > 0;i--) {
            for (k = 1;k < COL-1;k++) {
                map[i][k] = map[i - 1][k];
            }
        }
    }

    // Third: Increase the score.
    if (rows > 0) {
        game_score += (rows * 100);
    }
}

void create_block(char map[ROW][COL], block *main_block){
    char i, random_block = 0;

    srand(time(0));
    random_block = rand() % 7;

    for (i = 0;i < 4;i++) {
        main_block->peaces[i].y = blocks_yx[random_block][i][y];
        main_block->peaces[i].x = blocks_yx[random_block][i][x];
    }

    main_block->shape = random_block;
    main_block->status = 1;
}

void move_block(char map[ROW][COL], block *main_block) {
    char i, j, flag = check_collision(map, main_block);

    if (flag == LOSE) {
        game_status = false;
        return;
    }

    if (flag == BOTTOM) {
        check_line(map);
        create_block(map, main_block);
    }

    for (i = 0;i < 4 && flag == true;i++) {
        map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

        switch (main_block->direction) {
            case RIGHT: 
                main_block->peaces[i].x += 1;
            break;
            case LEFT:
                main_block->peaces[i].x -= 1;
            break;
            case DOWN:
                main_block->peaces[i].y += 1;
            break;
        }

        map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
    }

    /* Magic !! */
    for (i = 0;i < 4 && flag == true;i++) {
        map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';
        map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
    }
}

void rotate_block(char map[ROW][COL], block *main_block){
    char i;

    if (main_block->shape == O)
        return;

    switch (main_block->shape) {
        case I:
            if (main_block->status == 1) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    main_block->peaces[i].y = main_block->peaces[ORIGIN].y;
                    main_block->peaces[i].x = main_block->peaces[ORIGIN].x + i;

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 2;
            } 
            else if (main_block->status == 2) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    main_block->peaces[i].y = main_block->peaces[ORIGIN].y + i;
                    main_block->peaces[i].x = main_block->peaces[ORIGIN].x;

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 1;
            }
        break;
        case T:
            if (main_block->status == 1) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    if (i == 3) {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 1;
                    } else {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x + i;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 2;
            } 
            else if (main_block->status == 2) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    if (i == 3) {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 1;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x - 1;
                    } else {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + i;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 3;
            }
            else if (main_block->status == 3) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    if (i == 3) {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y - 1;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x + 1;
                    } else {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x + i;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 4;
            }
            else if (main_block->status == 4) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    if (i == 3) {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 1;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x + 1;
                    } else {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + i;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 1;
            }
        break;
        case S:
            if (main_block->status == 1) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    switch (i) {
                        case 1:
                            main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 1;
                            main_block->peaces[i].x = main_block->peaces[ORIGIN].x;
                        break;
                        case 2:
                            main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 1;
                            main_block->peaces[i].x = main_block->peaces[ORIGIN].x + 1;
                        break;
                        case 3:
                            main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 2;
                            main_block->peaces[i].x = main_block->peaces[ORIGIN].x + 1;
                        break;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 2;
            } 
            else if (main_block->status == 2) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    switch (i) {
                        case 1:
                            main_block->peaces[i].y = main_block->peaces[ORIGIN].y;
                            main_block->peaces[i].x = main_block->peaces[ORIGIN].x + 1;
                        break;
                        case 2:
                            main_block->peaces[i].y = main_block->peaces[ORIGIN].y - 1;
                            main_block->peaces[i].x = main_block->peaces[ORIGIN].x + 1;
                        break;
                        case 3:
                            main_block->peaces[i].y = main_block->peaces[ORIGIN].y - 1;
                            main_block->peaces[i].x = main_block->peaces[ORIGIN].x + 2;
                        break;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 1;
            }
        break;
        case Z:
            if (main_block->status == 1) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    switch (i) {
                        case 1:
                            main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 1;
                            main_block->peaces[i].x = main_block->peaces[ORIGIN].x;
                        break;
                        case 2:
                            main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 1;
                            main_block->peaces[i].x = main_block->peaces[ORIGIN].x - 1;
                        break;
                        case 3:
                            main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 2;
                            main_block->peaces[i].x = main_block->peaces[ORIGIN].x - 1;
                        break;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 2;
            } 
            else if (main_block->status == 2) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    switch (i) {
                        case 1:
                            main_block->peaces[i].y = main_block->peaces[ORIGIN].y;
                            main_block->peaces[i].x = main_block->peaces[ORIGIN].x - 1;
                        break;
                        case 2:
                            main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 1;
                            main_block->peaces[i].x = main_block->peaces[ORIGIN].x;
                        break;
                        case 3:
                            main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 1;
                            main_block->peaces[i].x = main_block->peaces[ORIGIN].x + 1;
                        break;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 1;
            }
        break;
        case L:
            if (main_block->status == 1) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    if (i == 3) {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 1;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x;
                    } else {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x + i;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 2;
            } 
            else if (main_block->status == 2) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    if (i == 3) {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x - 1;
                    } else {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + i;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 3;
            }
            else if (main_block->status == 3) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    if (i == 3) {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y - 1;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x + 2;
                    } else {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x + i;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 4;
            }
            else if (main_block->status == 4) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    if (i == 3) {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 2;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x + 1;
                    } else {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + i;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 1;
            }
        break;
        case J:
            if (main_block->status == 1) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    if (i == 3) {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y - 1;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x;
                    } else {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x + i;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 2;
            } 
            else if (main_block->status == 2) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    if (i == 3) {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x + 1;
                    } else {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + i;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 3;
            }
            else if (main_block->status == 3) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    if (i == 3) {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 1;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x + 2;
                    } else {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x + i;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 4;
            }
            else if (main_block->status == 4) {
                for (i = 1;i < 4;i++) {
                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = ' ';

                    if (i == 3) {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + 2;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x - 1;
                    } else {
                        main_block->peaces[i].y = main_block->peaces[ORIGIN].y + i;
                        main_block->peaces[i].x = main_block->peaces[ORIGIN].x;
                    }

                    map[main_block->peaces[i].y][main_block->peaces[i].x]  = '#';
                }

                main_block->status = 1;
            }
        break;
    }
}

void render(char map[ROW][COL]) {
    int i, j;

    for (i = 0;i < ROW;i++) {
        for (j = 0;j < COL;j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}

void clear(char map[ROW][COL]) {
    int i, j;

    for (i = 0;i < ROW;i++) {
        for (j = 0;j < COL;j++) {
            map[i][j] = ' ';
        }
    }
}

void init_tetris(char map[ROW][COL], block *main_block){
    init_map(map);
    create_block(map, main_block);
    render(map);
}

short get_key(int key){
    return (GetAsyncKeyState(key) & 0x8000 != 0);
}

void main(){
    char map[ROW][COL];
    block main_block;

    // Initialization
    if (welcome_message() == true)
        init_tetris(&map, &main_block);

    // Game Loop
    while (game_status == true) {
        // Directions Control
        if (get_key(VK_UP)) 
            rotate_block(&map, &main_block);
        else if (get_key(VK_RIGHT)) 
            main_block.direction = RIGHT;
        else if (get_key(VK_LEFT)) 
            main_block.direction = LEFT;
        else if (get_key(VK_DOWN))
            main_block.direction = DOWN;
        else
            main_block.direction = DOWN;
        
        // Auto Moving
        move_block(&map, &main_block); 
        Sleep(PERIOD);
        
        // Rendering
        system("@cls||clear");
        render(&map);

        // Show the Score
        printf("\n SCORE: %d \n", game_score);
    }

    // When the loop stops then..
    printf("you lose!");
}
