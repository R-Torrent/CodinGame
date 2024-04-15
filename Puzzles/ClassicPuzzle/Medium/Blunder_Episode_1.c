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

char *sdir[] = {
	"NORTH",
	"EAST",
	"SOUTH",
	"WEST"
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
	struct move *prev;
} move_t;

int L;
int C;

void clear_list(move_t **plist_moves)
{
	move_t *prev = (*plist_moves)->prev;
	free(*plist_moves);
	if (*plist_moves = prev)
		clear_list(plist_moves);
}

void print_list(move_t *list_moves)
{
	if (list_moves->prev)
		print_list(list_moves->prev);
	printf("%s\n", sdir[list_moves->d]);
}

bool interact_with_map(char (*map)[C + 1], move_t *move, point_t *teleporters)
{
	switch (map[move->x.i][move->x.j]) {
	case '$':
		return true;
	case 'X':
		map[move->x.i][move->x.j] = ' ';
		break;
	case 'S':
		move->d = SOUTH;
		break;
	case 'E':
		move->d = EAST;
		break;
	case 'N':
		move->d = NORTH;
		break;
	case 'W':
		move->d = WEST;
		break;
	case 'I':
		move->p ^= 1;
		break;
	case 'B':
		move->b = !move->b;
		break;
	case 'T';
	// *****  FINISH THIS!
		break;
	case ' ': default:
		// fall through
	}
	return false;
}

void advance_Blunder(point_t *x2, point_t *x1, enum dir d)
{
	*x2 = *x1
	switch (d) {
	case NORTH:
		x2->i = x1->i - 1;
		break;
	case EAST:
		x2->j = x1->j + 1
		break;
	case WEST:
		x2->j = x1->j - 1;
		break;
	case SOUTH: default:
		x2->i = x1->i + 1;
	}
}

bool is_equal_to(move_t *move1, move_t *move2)
{
	return move1->x.i == move2->x.i
		&& move1->x.j == move2->x.j
		&& move1->d == move2->d
		&& move1->p == move2->p
		&& move1->b == move2->b;
}

bool check_records(move_t *move, move_t **plist_moves) {
{
	move_t *m = *plist_moves;

	while (m) {
		if (is_equal_to(move, m)
			return true;
		m = m->prev;
	}
	m = malloc(sizeof(move_t));
	*m = *move;
	m->prev = *plist_moves;
	*plist_moves = m;
	return false;
}

void change_dir(char (*map)[C + 1], move_t *move)
{
	point_t x2;
	size_t next_pr = 0;

	while (true) {
		advance_Blunder(&x2, &move->x, move->d);
		if (map[x2.i][x2.j] != '#' && (map[x2.i][x2.j] != 'X' || move->b))
			return;
		move->d = priorities[move->p][next_pr++];
	}
}

void find_symbols(char (*map)[C + 1], char symbol, point_t *x, size_t n)
{
	char (*const row0)[C + 1] = map;
	char *m = *map;

	while (n--) {
		while(!(m = strchr(m, symbol)))
			m = ++map;
		x->i = map - row0;
		x++->j = m++ - map;
	}
}

int main()
{
    scanf("%d%d\n", &L, &C);
	char (*map)[C + 1] = malloc(L * (C + 1));
    for (int i = 0; i < L; i++)
        scanf("%[^\n]\n", map[i]);

    // Write an answer using printf(). DON'T FORGET THE TRAILING \n
    // To debug: fprintf(stderr, "Debug messages...\n");

	point_t teleporters[2];
	find_symbols(map, 'T', teleporters, 2);
	move_t move0 = {d = SOUTH}, *list_moves = NULL;
	find_symbols(map, '@', &move0.x, 1);

	while (true) {
		change_dir(map, &move0);
		if (check_records(&move0, &list_moves)) {
			printf("LOOP\n");
			break;
		}
		advance_Blunder(&move0.x, list_moves->x);
		if (interact_with_map(map, &move0, teleporters)) {
			print_list(list_moves);
			break;
		}
	}

	free(map);
	if (list_moves)
		clear_list(&list_moves);
    return 0;
}
