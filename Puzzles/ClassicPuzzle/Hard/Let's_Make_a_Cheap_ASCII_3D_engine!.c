#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Let's make a cheap ASCII 3D engine!
 * Puzzles > Classic Puzzle > Hard
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

enum tile {
	H, // h-wall
	V, // v-wall
	E  // empty
};

char display[] = { [H] = '.', [V] = ',', [E] = ' ' };

// Horizontal wall segments: y = k for l0 <= x <= l1
// Vertical wall segments: x = k for l0 <= y <= l1
typedef struct s_wall {
	enum tile     t;
	int           k;
	int           l0;
	int           l1;
	struct s_wall *next;
} wall_t;

typedef struct {
	enum tile  t;
	double     D;
	double     D1;
} frame_col_t;

void read_room(wall_t **, char (*)[21], int);
void trace_rays(frame_col_t *, wall_t *, int, int, int);
void construct_frame(char (*)[62], frame_col_t *);
void display_frame(char (*)[62]);
void clear_list(wall_t **);

int main()
{
	int X, Y, A, N;
	scanf("%d%d%d%d\n", &X, &Y, &A, &N);

	wall_t *list = NULL;
	char room[20][21];
	frame_col_t fcol[61];
	char frame[15][62];

	read_room(&list, room, N);

	trace_rays(fcol, list, X, Y, A);

	construct_frame(frame, fcol);

	display_frame(frame);

	clear_list(&list);

    return 0;
}

void add_wall(wall_t **plist, wall_t *new)
{
	new->next = *plist;
	*plist = new;
}

void read_room(wall_t **plist, char (*room)[21], int N)
{
	char *w;
	wall_t *segment;

	for (int r = 0; r < N; r++) { // horizontal walls
		scanf("%[^\n]\n", room[r]);
		char row[21];
		for (char *str = strcpy(row, room[r]); (w = strtok(str, ".")); str = NULL) {
			int h0 = (w - row) * 100, h1 = h0 + strlen(w) * 100 - 1;
			int v0 = r * 100, v1 = (r + 1) * 100 - 1;
			segment = malloc(sizeof(wall_t));
			*segment = (wall_t){ H, v0, h0, h1 }; // top surface
			add_wall(plist, segment);
			segment = malloc(sizeof(wall_t));
			*segment = (wall_t){ H, v1, h0, h1 }; // bottom surface
			add_wall(plist, segment);
		}
	}
	for (int c = 0; c < N; c++) { // vertical walls
		char col[21];
		int r;
		for (r = 0; r < N; r++)
			col[r] = room[r][c];
		col[N] = '\0';
		for (char *str = col; (w = strtok(str, ".")); str = NULL) {
			int v0 = (w - col) * 100, v1 = v0 + strlen(w) * 100 - 1;
			int h0 = c * 100, h1 = (c + 1) * 100 - 1;
			segment = malloc(sizeof(wall_t));
			*segment = (wall_t){ V, h0, v0, v1 }; // left surface
			add_wall(plist, segment);
			segment = malloc(sizeof(wall_t));
			*segment = (wall_t){ V, h1, v0, v1 }; // right surface
			add_wall(plist, segment);
		}
	}
}

const double d_to_r = 3.14159265 / 180;

void trace_rays(frame_col_t *fcol, wall_t *list, int X, int Y, int A)
{
	for (int a = -30; a < 31; a++, fcol++) {
		double angle = (A + a) * d_to_r;
		double angle1 = (A - a) * d_to_r;
		double candidate;
		*fcol = (frame_col_t){ .D = 20 * 100 * 1.5 };
		for (wall_t *segment = list; segment; segment = segment->next) {
			double diff_X, diff_Y, intersection;
			if (segment->t == H) {
				diff_Y = segment->k - Y;
				intersection = X + diff_Y / tan(angle);
				diff_X = intersection - X;
			}
			else {
				diff_X = segment->k - X;
				intersection = Y + diff_X * tan(angle);
				diff_Y = intersection - Y;
			}
			if (fabs(atan2(diff_Y, diff_X) / d_to_r - (A + a) % 360) > 10)
				continue;
			if (segment->l0 <= intersection && intersection <= segment->l1
				&& (candidate = hypot(diff_X, diff_Y)) < fcol->D)
				*fcol = (frame_col_t){ segment->t, candidate, candidate * cos(angle1) };
		}
	}
}

void construct_frame(char (*frame)[62], frame_col_t *fcol)
{
	for (int i = 0; i < 61; i++, fcol++) {
		int H = (int)round(1500 / fcol->D1);
		for (int j = -7; j < 8; j++)
			frame[j + 7][i] = abs(j) <= H ? display[fcol->t] : display[E];
	}
	for (int j = 0; j < 15; j++)
		frame[j][61] = '\0';
}

void display_frame(char (*frame)[62])
{
	for (int j = 0; j < 15; j++)
		puts(*frame++);
}

void clear_list(wall_t **plist)
{
	wall_t *next;

	while (*plist)
	{
		next = (*plist)->next;
		free(*plist);
		*plist = next;
	}
}
