#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>


static const char ROW_TOP[] = "          %c %c %c\n";
static const char ROW_ODD[] = "        %d %c|%c|%c \n";
static const char ROW_EVN[] = "         --|-|--\n";

static uint8_t const LINE_PATTERNS[][3] = {
        {0, 1, 2},
        {3, 4, 5},
        {6, 7, 8},
        {0, 3, 6},
        {1, 4, 7},
        {2, 5, 8},
        {0, 4, 8},
        {2, 4, 6},
};

static uint8_t const N_PATTERNS = sizeof(LINE_PATTERNS)
                                / sizeof(LINE_PATTERNS[0]);

struct Game {
        char grid[9];
        bool over;
};

static void
render(char const *grid)
{
        printf("\n");
        printf(ROW_TOP, 'a', 'b', 'c');
        for (uint8_t i = 0; i < 3; ++i) {
                printf(ROW_ODD, i + 1,
                                grid[3*i],
                                grid[3*i + 1],
                                grid[3*i + 2]);
                if (i != 2)
                        printf(ROW_EVN);
        }
        printf("\n");
}

static void
initialize(struct Game* restrict game)
{
        for (uint8_t i = 0; i < 9; ++i) {
                game->grid[i] = ' ';
        }
        game->over = false;
}

static void
makeUserTurn(struct Game* game)
{
        char line, row;
        uint8_t tileIndex;
        bool valid = false;
        do {
                puts("Your turn 'X'");
                puts("Please enter line (1,2,3)");
                scanf(" %c", &line);
                puts("Please enter row (a,b,c)");
                scanf(" %c", &row);
                if (line < '1' || line > '3' || row < 'a' || row > 'c') {
                        puts("Invalid input");
                        continue;
                }
                tileIndex = 3 * (line - '1') + (row - 'a');
                if (game->grid[tileIndex] == ' ') {
                        game->grid[tileIndex] = 'X';
                        valid = true;
                }
        } while (!valid);
}

static bool
checkForFullLine(struct Game *const game, char const tag)
{
        for (uint8_t i = 0; i < N_PATTERNS; ++i){
                if (game->grid[LINE_PATTERNS[i][0]] == tag &&
                    game->grid[LINE_PATTERNS[i][1]] == tag &&
                    game->grid[LINE_PATTERNS[i][2]] == tag) {
                        game->over = true;
                        render(game->grid);
                        printf("'%c' WINS !\n", tag);
                }
        }
        return false;
}


statict bool
checkIfGridFull(struct Game* game)
{
        for (int i = 0; i < 9; ++i) {
                if (game->grid[i] == ' ') return false;
        }
        game->over = true;
        render(game->grid);
        puts("Draw");
        return true;
}

static bool
completeLine(struct Game* game, const char tag, const int8_t iteration)
{
        if (game->grid[LINE_PATTERNS[iteration][0]] == tag &&
            game->grid[LINE_PATTERNS[iteration][1]] == tag &&
            game->grid[LINE_PATTERNS[iteration][2]] == ' ') {
                game->grid[LINE_PATTERNS[iteration][2]] = 'O';
                return true;
        } else
        if (game->grid[LINE_PATTERNS[iteration][1]] == tag &&
            game->grid[LINE_PATTERNS[iteration][2]] == tag &&
            game->grid[LINE_PATTERNS[iteration][0]] == ' ') {
                game->grid[LINE_PATTERNS[iteration][0]] = 'O';
                return true;
        } else
        if (game->grid[LINE_PATTERNS[iteration][2]] == tag &&
            game->grid[LINE_PATTERNS[iteration][0]] == tag &&
            game->grid[LINE_PATTERNS[iteration][1]] == ' ') {
                game->grid[LINE_PATTERNS[iteration][1]] = 'O';
                return true;
        }
        return false;
}

static void
makeComputerTurn(struct Game* game)
{
        int8_t moveValueMap[9] = { 0 };
        for (int8_t i = 0; i < N_PATTERNS; ++i) {

                // Secure win
                if (completeLine(game, 'O', i)) return;

                // Prevent loss
                if (completeLine(game, 'X', i)) return;

                // do not consider line with an X
                if (game->grid[LINE_PATTERNS[i][0]] == 'X' ||
                    game->grid[LINE_PATTERNS[i][1]] == 'X' ||
                    game->grid[LINE_PATTERNS[i][2]] == 'X') {
                        --moveValueMap[LINE_PATTERNS[i][0]];
                        --moveValueMap[LINE_PATTERNS[i][1]];
                        --moveValueMap[LINE_PATTERNS[i][2]];
                        continue;
                }

                // determine values for each possible moves
                if (game->grid[LINE_PATTERNS[i][0]] == 'O') {
                        ++moveValueMap[LINE_PATTERNS[i][1]];
                        ++moveValueMap[LINE_PATTERNS[i][2]];
                }
                if (game->grid[LINE_PATTERNS[i][1]] == 'O') {
                        ++moveValueMap[LINE_PATTERNS[i][0]];
                        ++moveValueMap[LINE_PATTERNS[i][2]];
                }
                if (game->grid[LINE_PATTERNS[i][2]] == 'O') {
                        ++moveValueMap[LINE_PATTERNS[i][0]];
                        ++moveValueMap[LINE_PATTERNS[i][1]];
                }
        }
        for (uint8_t i = 0; i < 9; ++i) {
                if (game->grid[i] != ' ') {
                        moveValueMap[i] -= 5;
                }
        }
        int8_t bestMove = -1;
        time_t t;
        srand((unsigned) time(&t));
        uint8_t iterations = rand() % 32;
        for (int8_t i = 0; i < iterations; ++i) {
                if (moveValueMap[i % 9] >= bestMove &&
                    game->grid[i % 9] == ' ') {
                        bestMove = i;
                }
        }
        if (bestMove == -1) {
            for (uint8_t i = 0; i < 9; ++i) {
                if (game->grid[i] == ' ') bestMove = i;
            }
        }
        game->grid[bestMove] = 'O';
}

int main(void)
{
        struct Game game;
        initialize(&game);
        while(!game.over) {
                render(game.grid);
                makeUserTurn(&game);
                checkForFullLine(&game, 'X');
                if (game.over)
                        break;

                checkIfGridFull(&game);
                makeComputerTurn(&game);
                checkForFullLine(&game, 'O');
                if (game.over)
                        break;

                checkIfGridFull(&game);
        }
        puts("Game Over");
        return EXIT_SUCCESS;
}

