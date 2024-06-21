#include <stdio.h>

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

enum jump { V, H }; // vertical or horizontal

int main()
{
	rectangle_t rect = { 0 };
	// width of the building.
	// height of the building.
	scanf("%d%d", &rect.w, &rect.h);
	// maximum number of turns before game over.
	int N;
	scanf("%d", &N);

	point_t jump, last;
	enum jump dir;
	point_t cm;

	scanf("%d%d", &last.x, &last.y);

	// game loop
	while (1) {
		// current distance to the bomb compared to previous distance (COLDER, WARMER, SAME or UNKNOWN)
		char bomb_dir[11];
		scanf("%s", bomb_dir);

		switch (*bomb_dir) {
		case 'W': // WARMER
			if (dir == V)
				rect.h /= 2;
			else
				rect.w /= 2;
			if (dir == V && jump.y > cm.y)
				rect.p0.y = cm.y + 1;
			else if (dir == H && jump.x > cm.x)
				rect.p0.x = cm.x + 1;
			break;
		case 'C': // COLDER
			if (dir == V)
				rect.h /= 2;
			else
				rect.w /= 2;
			if (dir == V && jump.y < cm.y)
				rect.p0.y = cm.y + 1;
			else if (dir == H && jump.x < cm.x)
				rect.p0.x = cm.x + 1;
			break;
		case 'S': // SAME
			if (dir == V) {
				rect.h = 1;
				rect.p0.y = cm.y;
			}
			else {
				rect.w = 1;
				rect.p0.x = cm.x;
			}
			break;
		case 'U': // UNKNOWN
			break;
		}

		// jump favors odd-sized directions first (for a lucky chance of a 'SAME' result) and shorter sides
		dir = rect.w == 1 || (rect.h % 2 && rect.h > 1 && !(rect.w % 2 && rect.h > rect.w)) ||
				(!(rect.h % 2) && !(rect.w % 2) && rect.h <= rect.w) ? V : H;

		// center of mass
		point_t cm = { rect.p0.x + rect.w / 2, rect.p0.y + rect.h / 2 };

		jump.x = dir == V ? last.x : 2 * cm.x - last.x - (!(rect.w % 2) ? 1 : 0);
		jump.y = dir == V ? 2 * cm.y - last.y - (!(rect.h % 2) ? 1 : 0) : last.y;

		printf("%d %d\n", jump.x, jump.y);

		last = jump;
	}
}
