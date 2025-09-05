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

enum type {
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
	struct caracter_s *character;
	char c;
} cell_t;

typedef struct character_s {
	cell_t *prev_location;
	cell_t *curr_location;
	char c;
} character_t;

int height;     // first_init_input
int width;      // second_init_input
size_t size;    // width * height
int characters; // third_init_input

void print_map(char (*)[width], const cell_t (*)[width], const character_t *);

void print_command(const move_t);

int main()
{
	srand(time(NULL));

	scanf("%d", &height);
	scanf("%d", &width);
	size = width * height;
	scanf("%d", &characters); fgetc(stdin);
	fprintf(stderr, "height    :%3d\n", height);
	fprintf(stderr, "width     :%3d\n", width);
	fprintf(stderr, "characters:%3d\n", characters);
	fprintf(stderr, "--------------\n");

	cell_t (*const cell)[width] = malloc(size * sizeof(cell_t));
	character_t *const character = malloc(characters * sizeof(character_t));
	character_t *const player = &character[characters - 1];
	char (*const map)[width] = malloc(size);

	int i, j;
	for (i = 0; i < characters; i++) {
		character[i].prev_location = NULL;
		character[i].curr_location = NULL;
		character[i].c = (i == characters - 1 ? PLAYER : GHOST);
	}
	for (j = 0; j < height; j++)
		for (i = 0; i < width; i++)
			cell[j][i] = (cell_t){ i, j, { NULL }, NULL, UNKNOWN };

	// game loop
	for (int turn = 1; ; turn++) {
		char map_up;    // first_input
		char map_right; // second_input
		char map_down;  // third_input
		char map_left;  // fourth_input
		scanf("%c", &map_up); fgetc(stdin);
		scanf("%c", &map_right); fgetc(stdin);
		scanf("%c", &map_down); fgetc(stdin);
		scanf("%c", &map_left);
		fprintf(stderr, "***turn #: %d\n", turn);
		fprintf(stderr, "map_up   : %c\n", map_up);
		fprintf(stderr, "map_right: %c\n", map_right);
		fprintf(stderr, "map_down : %c\n", map_down);
		fprintf(stderr, "map_left : %c\n", map_left);
		
		int x; // fifth_input
		int y; // sixth_input
		for (character_t *ch = character; ch - character < characters; ch++)
			if (ch->curr_location) {
				ch->prev_location = ch->curr_location;
				ch->curr_location->character = NULL;
			}
		for (i = 0; i < characters; i++) {
			scanf("%d%d", &x, &y); fgetc(stdin);
			fprintf(stderr, "character[%d] x y: %d %d\n", i, x, y);

			character[i].curr_location = &cell[y][x];
			character[i].curr_location->character = &character[i];
			cell[y][x].c = EMPTY;
		}

		// neighbor cells for the player
		cell_t *c1, *const clp = player->curr_location;;
		if (y > 0 && (c1 = &cell[y - 1][x])->c != WALL) {
			c1->c = map_up;
			if (map_up == EMPTY) {
				clp->neighbor[UP] = c1;
				c1->neighbor[DOWN] = clp;
			}
		}
		if (x < width - 1 && (c1 = &cell[y][x + 1])->c != WALL) {
			c1->c = map_right;
			if (map_right == EMPTY) {
				clp->neighbor[RIGHT] = c1;
				c1->neighbor[LEFT] = clp;
			}
		}
		if (y < height - 1 && (c1 = &cell[y + 1][x])->c != WALL) {
			c1->c = map_down;
			if (map_down == EMPTY) {
				clp->neighbor[DOWN] = c1;
				c1->neighbor[UP] = clp;
			}
		}
		if (x > 0 && (c1 = &cell[y][x - 1])->c != WALL) {
			c1->c = map_left;
			if (map_left == EMPTY) {
				clp->neighbor[LEFT] = c1;
				c1->neighbor[RIGHT] = clp;
			}
		}

		// neighbor cell for the moving ghosts
		for (character_t *ch = character; ch - character < characters - 1; ch++) {
			cell_t *const prev = ch->prev_location;
			cell_t *const curr = ch->curr_location;

			if (!prev || curr == prev)
				continue;
			if (curr->y == prev->y) {
				if (curr->x < prev->x) {
					curr->neighbor[RIGHT] = prev;
					prev->neighbor[LEFT] = curr;
				}
				else {
					curr->neighbor[LEFT] = prev;
					prev->neighbor[RIGHT] = curr;
				}
			}
			else {
				if (curr->y < prev->y) {
					curr->neighbor[DOWN] = prev;
					prev->neighbor[UP] = curr;
				}
				else {
					curr->neighbor[UP] = prev;
					prev->neighbor[DOWN] = curr;
				}
			}
		}

		print_map(map, cell, character);

		// silly random movement around the maze
		int r = 0;
		for (i = 0; i < 4; i++)
			if (clp->neighbor[i] && !clp->neighbor[i]->character)
				r++;
		if (r == 1) // dead end
			r = STILL;
		else
			do {
				r = rand() % 4;
				switch (r) {
				case 0: c1 = clp->neighbor[RIGHT]; r = RIGHT; break;
				case 1: c1 = clp->neighbor[UP]; r = UP; break;
				case 2: c1 = clp->neighbor[DOWN]; r = DOWN; break;
				case 3: c1 = clp->neighbor[LEFT]; r = LEFT; break;
				}
			} while (!c1 || c1 == player->prev_location);
		print_command(r);
	}

	free(cell);
	free(character);
	free(map);

	return 0;
}

void print_map(char map[][width], const cell_t cell[][width], const character_t character[])
{
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			map[j][i] = cell[j][i].c;
	for (const character_t *ch = character; ch - character < characters; ch++)
		map[ch->curr_location->y][ch->curr_location->x] = ch->c;

	for (int j = 0; j < height; j++)
		fprintf(stderr, "%.*s\n", width, map[j]);
}

void print_command(const move_t m)
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
