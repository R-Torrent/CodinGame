#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * The Fall - Episode 1
 * Puzzles > Classic Puzzle > Medium
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

enum dir {
	TOP,
	LEFT,
	RIGHT,
	BOTTOM,
	WALL
};

enum dir path[14][3] = {
	{ WALL, WALL, WALL },       // Type  0
	{ BOTTOM, BOTTOM, BOTTOM }, // Type  1
	{ WALL, RIGHT, LEFT },      // Type  2
	{ BOTTOM, WALL, WALL },     // Type  3
	{ LEFT, WALL, BOTTOM },     // Type  4
	{ RIGHT, BOTTOM, WALL },    // Type  5
	{ WALL, RIGHT, LEFT },      // Type  6
	{ BOTTOM, WALL, BOTTOM },   // Type  7
	{ WALL, BOTTOM, BOTTOM },   // Type  8
	{ BOTTOM, BOTTOM, WALL },   // Type  9
	{ LEFT, WALL, WALL },       // Type 10
	{ RIGHT, WALL, WALL },      // Type 11
	{ WALL, WALL, BOTTOM },     // Type 12
	{ WALL, BOTTOM, WALL }      // Type 13
};

int main()
{
    // number of columns.
    int W;
    // number of rows.
    int H;
    scanf("%d%d", &W, &H); fgetc(stdin);
	char *LINE = malloc(H * 201);
    for (int i = 0; i < H; i++) {
        // represents a line in the grid and contains W integers. Each integer represents one room of a given type.
        scanf("%[^\n]", LINE + i * 201); fgetc(stdin);
    }
    // the coordinate along the X axis of the exit (not useful for this first mission, but must be read).
    int EX;
    scanf("%d", &EX);

    // game loop
    while (1) {
        int XI;
        int YI;
        char POS[6];
        scanf("%d%d%s", &XI, &YI, POS);

        // Write an action using printf(). DON'T FORGET THE TRAILING \n
        // To debug: fprintf(stderr, "Debug messages...\n");

		char t[3] = { '\0', '\0', '\0' };
		char *level = LINE + YI * 201;
		for (int i = XI; i; i--)
			level += strcspn(level, " ") + 1;
		int type = atoi(strncpy(t, level, strcspn(level, " ")));
		int XO;
		int YO;
		enum dir entrance = !strcmp(POS, "TOP") ? TOP : !strcmp(POS, "LEFT") ? LEFT : RIGHT;
		switch (path[type][entrance]) {
		case BOTTOM: 
			XO = XI;
			YO = YI + 1;
			break;
		case LEFT:
			XO = XI - 1;
			YO = YI;
			break;
		case RIGHT:
			XO = XI + 1;
			YO = YI;
		};
        // One line containing the X Y coordinates of the room in which you believe Indy will be on the next turn.
        printf("%d %d\n", XO, YO);
    }

	free(LINE);
    return 0;
}
