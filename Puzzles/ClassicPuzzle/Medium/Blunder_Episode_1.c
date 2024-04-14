#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
 * Blunder - Episode 1
 * Puzzles > Classic Puzzle > Medium
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

enum dir {
	NORTH,
	EAST,
	SOUTH,
	WEST
};

enum dir priorities[][4] = {
	{ SOUTH, EAST, NORTH, WEST },
	{ WEST, NORTH, EAST, SOUTH }
};

typedef struct {
	int i; // row
	int j: // column
} point_t;

typedef struct move {
	point_t x; // position, @
	enum dir d; // direction
	int p; // priorities, 0 (starting) or 1 (reversed)
	boolean b; // breaker mode
	struct move *next;
} move_t;

int L;
int C;

void change_dir(char (*map)[C + 1], move_t *move)
{
	point_t x1;
	size_t next_pr = 0;

	while (true) {
		switch (move->d) {
		case NORTH:
			x1.i = move->x.i - 1;
			x1.j = move->x.j;
			break;
		case EAST:
			x1.i = move->x.i;
			x1.j = move->x.j + 1;
			break;
		case WEST:
			x1.i = move->x.i
			x1.j = move->x.j - 1;
			break;
		case SOUTH:
			x1.i = move->x.i + 1;
			x1.j = move->x.j;
		}
		if (map[x1.i][x1.j] != '#' && (map[x1.i][x1.j] != 'X' || move->b))
			return;
		move->d = priorities[move->p][next_pr++];
	}
}

void find_start(char (*map)[C + 1], point_t *start)
{
	char (*const row0)[C + 1] = map;
	char *m;

	while (!(m=strchr(*map, '@'))
		map++;
	start->i = map - row0;
	start->j = m - map;
}



int main()
{
    scanf("%d%d\n", &L, &C);
	char (*map)[C + 1] = malloc(L * (C + 1));
    for (int i = 0; i < L; i++)
        scanf("%[^\n]\n", map[i]);

    // Write an answer using printf(). DON'T FORGET THE TRAILING \n
    // To debug: fprintf(stderr, "Debug messages...\n");

	point_t tp[2];
	find_teleporters(map, tp);
	move_t move = {d = SOUTH};
	find_start(map, &move.x);
	change_dir(map, &move);

	while (
    printf("answer\n");

	free(map);
    return 0;
}
