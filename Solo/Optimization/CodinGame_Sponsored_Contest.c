// TODO
// Implement general lists
// Cells need a list for occupants instead of a single pointer to character_t
// Scoring not taken into account
// Unknown if there are energizers to eat the ghosts and bonus items in the center of the maze
// Unknown if the maze regenerates or changes once all the dots are eaten
// AI is silly

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * CodinGame Sponsored Contest 
 * Solo > Optimization
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

// Write an action using printf(). DON'T FORGET THE TRAILING \n
// To debug: fprintf(stderr, "Debug messages...\n");

enum type_e {
	UNKNOWN = '.',
	EMPTY   = '_',
	WALL    = '#',
	PLAYER  = '@',
	GHOST   = 'G'
};

typedef enum {
	RIGHT,
	UP,
	DOWN,
	LEFT,
	STILL
} move_t;

typedef struct cell_s {
	int x;
	int y;
	struct cell_s *neighbor[4];
	struct character_s *character;
	char c;
} cell_t;

struct action {
	cell_t *prev;
	cell_t *curr;
	move_t m;
}; 

typedef struct character_s {
	struct action a;
	char c;
} character_t;

int height;        // first_init_input
int width;         // second_init_input
size_t total_size; // width * height
int characters;    // third_init_input
int turn = 0;

void print_maze(char (*)[width], const cell_t (*)[width], const character_t *);
void print_action(const move_t);
void determine_movement(struct action *);
move_t determine_action(const cell_t (*)[width], const character_t *, const character_t *);

int main()
{
	srand(time(NULL));

	scanf("%d", &height);
	scanf("%d", &width);
	total_size = width * height;
	scanf("%d", &characters); fgetc(stdin);
	fprintf(stderr, "height    :%3d\n", height);
	fprintf(stderr, "width     :%3d\n", width);
	fprintf(stderr, "characters:%3d\n", characters);
	fprintf(stderr, "--------------\n");

	cell_t (*const cell)[width] = malloc(total_size * sizeof(cell_t));
	character_t *const character = malloc(characters * sizeof(character_t));
	character_t *const player = &character[characters - 1];
	char (*const maze)[width] = malloc(total_size);

	int i, j;
	for (i = 0; i < characters; i++) {
		character[i].a = (struct action){ NULL, NULL, STILL };
		character[i].c = (i == characters - 1 ? PLAYER : GHOST);
	}
	for (j = 0; j < height; j++)
		for (i = 0; i < width; i++)
			cell[j][i] = (cell_t){ i, j, { NULL }, NULL, UNKNOWN };

	// game loop
	for (turn = 1; ; turn++) {
		char maze_up;    // first_input
		char maze_right; // second_input
		char maze_down;  // third_input
		char maze_left;  // fourth_input
		scanf("%c", &maze_up); fgetc(stdin);
		scanf("%c", &maze_right); fgetc(stdin);
		scanf("%c", &maze_down); fgetc(stdin);
		scanf("%c", &maze_left);
		fprintf(stderr, "***turn #: %d\n", turn);
		fprintf(stderr, "maze_up   : %c\n", maze_up);
		fprintf(stderr, "maze_right: %c\n", maze_right);
		fprintf(stderr, "maze_down : %c\n", maze_down);
		fprintf(stderr, "maze_left : %c\n", maze_left);
		
		// update characters
		int x; // fifth_input
		int y; // sixth_input
		if (turn > 1)
			for (character_t *ch = character; ch - character < characters; ch++) {
				ch->a.curr->character = NULL;
				ch->a.prev = ch->a.curr;
			}
		for (i = 0; i < characters; i++) {
			scanf("%d%d", &x, &y); fgetc(stdin);
			fprintf(stderr, "character[%d] x y: %d %d\n", i, x, y);

			character[i].a.curr = &cell[y][x];
			cell[y][x].character = &character[i];
			cell[y][x].c = EMPTY;
		}
		if (turn > 1)
			for (character_t *ch = character; ch - character < characters; ch++)
				determine_movement(&ch->a);

		// link neighboring cells to the player
		cell_t *c1, *const plc = player->a.curr;
		if ((c1 = &cell[(y - 1) % height][x])->c != WALL) {
			c1->c = maze_up;
			if (maze_up == EMPTY) {
				plc->neighbor[UP] = c1;
				c1->neighbor[DOWN] = plc;
			}
		}
		if ((c1 = &cell[y][(x + 1) % width])->c != WALL) {
			c1->c = maze_right;
			if (maze_right == EMPTY) {
				plc->neighbor[RIGHT] = c1;
				c1->neighbor[LEFT] = plc;
			}
		}
		if ((c1 = &cell[(y + 1) % height][x])->c != WALL) {
			c1->c = maze_down;
			if (maze_down == EMPTY) {
				plc->neighbor[DOWN] = c1;
				c1->neighbor[UP] = plc;
			}
		}
		if ((c1 = &cell[y][(x - 1) % width])->c != WALL) {
			c1->c = maze_left;
			if (maze_left == EMPTY) {
				plc->neighbor[LEFT] = c1;
				c1->neighbor[RIGHT] = plc;
			}
		}

		// link neighboring cells to the moving ghosts
		// (ghosts don't seem to take the "warp tunnels")
		character_t *ch;
		for (ch = character; ch - character < characters - 1; ch++) {
			cell_t *const prev = ch->a.prev;
			cell_t *const curr = ch->a.curr;

			if (!prev || curr == prev)
				continue;
			if (curr->x < prev->x) {
				curr->neighbor[RIGHT] = prev;
				prev->neighbor[LEFT] = curr;
			}
			else if (curr->x > prev->x) {
				curr->neighbor[LEFT] = prev;
				prev->neighbor[RIGHT] = curr;
			}
			else if (curr->y < prev->y) {
				curr->neighbor[DOWN] = prev;
				prev->neighbor[UP] = curr;
			}
			else {
				curr->neighbor[UP] = prev;
				prev->neighbor[DOWN] = curr;
			}
		}

		print_maze(maze, cell, character);

		print_action(determine_action(cell, character, player));
	}

	free(cell);
	free(character);
	free(maze);

	return EXIT_SUCCESS;
}

void print_maze(char maze[][width], const cell_t cell[][width], const character_t character[])
{
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			maze[j][i] = cell[j][i].c;
	for (const character_t *ch = character; ch - character < characters; ch++)
		maze[ch->a.curr->y][ch->a.curr->x] = ch->c;

	for (int j = 0; j < height; j++)
		fprintf(stderr, "%.*s\n", width, maze[j]);
}

void print_action(const move_t m)
{
	char c;
	switch (m) {
	case RIGHT: c = 'A'; break;
	case STILL: c = 'B'; break;
	case UP:    c = 'C'; break;
	case DOWN:  c = 'D'; break;
	case LEFT:  c = 'E';
	}

	printf("%c\n", c);
}

// determínes which movement existed from 'prev' to 'curr'
void determine_movement(struct action *a)
{
	for (a->m = 0; a->m < STILL && a->curr != a->prev->neighbor[a->m]; a->m++)
		;
}

// silly random movement around the maze, turning away from ghosts
move_t determine_action(const cell_t cell[][width], const character_t character[], const character_t *player)
{
	cell_t **const pn0 = player->a.curr->neighbor, **pn = pn0;
	size_t m, n = 0; // available neighbors to move into
	struct action candidate[4];
	while (pn - pn0 < STILL) {
		if (*pn && !(*pn)->character) // linked and free of ghosts
			candidate[n++] = (struct action){ player->a.curr, *pn, pn - pn0 };
		pn++;
	}

	if (!n)
		return STILL;
	if (n == 1) // turn back
		return candidate->m;
	do {
		m = rand() % n;
	} while (candidate[m].curr == player->a.prev); // no backtracing whenever possible;
	return candidate[m].m;
}
