#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
 * Shadows of the Knight - Episode 2
 * Puzzles > Classic Puzzle > Expert
 */

typedef struct {
	int x;
	int y;
} point_t;

typedef struct {
	point_t p0;
	int w;
	int h;
} rectangle_t;

enum jump { V, H }; // Vertical or horizontal

int main()
{
	rectangle_t rect = { 0 };
	// width of the building.
	// height of the building.
	scanf("%d%d", &rect.w, &rect.h);
	// maximum number of turns before game over.
	int N;
	scanf("%d", &N);

	point_t last, jump;
	scanf("%d%d", &last.x, &last.y);

	// game loop
	while (1) {
		// Current distance to the bomb compared to previous distance (COLDER, WARMER, SAME or UNKNOWN)
		char bomb_dir[11];
		scanf("%s", bomb_dir);

		int gap = dir == V ? jump.y - last.y : jump.x - last.x;
		int cut = (dir == V ? last.y : last.x) + gap / 2 + gap % 2;
		if (*bomb_dir != 'U')
			if (*bomb_dir != 'S') {
				if (dir == V)
					rect.h = (gap > 0 ? 1 : -1) * (cut - rect.p0.y);
				else if (dir == H)
					rect.w = (gap > 0 ? 1 : -1) * (cut - rect.p0.x);
				if ((gap > 0 && *bomb_dir == 'W')) || (gap < 0 && *bomb_dir == 'C')))
					if (dir == V)
						rect.p0.y = cut + 1;
					else
						rect.p0.x = cut + 1;
				if (*bomb_dir == 'W')
					last = jump;
			}
			else {
				if (dir == V) {
					rect.h = 1;
					rect.p0.y = cut;
				}
				else {
					rect.w = 1;
					rect.p0.x = cut;
				}
			last =


			printf("0 0\n");
	}

	return 0;
}
