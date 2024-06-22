#include <stdio.h>
#include <stdlib.h>

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
	point_t p1;
	int w;
	int h;
} rectangle_t;

enum jump { H, V, D }; // horizontal, vertical, diagonal

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

int main()
{
	int building_x, building_y;
	rectangle_t rect = { 0 };
	// width of the building
	// height of the building
	scanf("%d%d", &building_x, &building_y);
	rect.p1 = (point_t){ building_x - 1, building_y - 1};
	rect.w = building_x;
	rect.h = building_y;

	// maximum number of turns before game over
	int N;
	scanf("%d", &N);

	point_t last, jump, jumps[3];
	enum jump dir;

	scanf("%d%d", &jump.x, &jump.y);

	// game loop
	while (1) {
		// current distance to the bomb compared to previous distance (COLDER, WARMER, SAME or UNKNOWN)
		char bomb_dir[11];
		scanf("%s", bomb_dir);

		// trim the rectagle
		int s;
		switch (*bomb_dir) {
		case 'C': case 'W': // COLDER or WARMER
			s = *bomb_dir == 'C' ? -1 : +1;
			if (dir == H) {
				int x = rect.p0.x, lim = rect.p1.x;
				while (s * (abs(jump.x - x) - abs(last.x - x)) >= 0)
					x++;
				rect.p0.x = x;
				for (rect.w = 0; x <= lim && s * (abs(jump.x - x) - abs(last.x - x)) < 0; x++)
					rect.w++;
				rect.p1.x = --x;
			}
			else if (dir == V) {
				int y = rect.p0.y, lim = rect.p1.y;
				while (s * (abs(jump.y - y) - abs(last.y - y)) >= 0)
					y++;
				rect.p0.y = y;
				for (rect.h = 0; y <= lim && s * (abs(jump.y - y) - abs(last.y - y)) < 0; y++)
					rect.h++;
				rect.p1.y = --y;
			}
			break;
		case 'S': // SAME
			if (dir == V) {
				rect.p0.y = rect.p1.y = (jump.y + last.y) / 2;
				rect.h = 1;
			}
			else {
				rect.p0.x = rect.p1.x = (jump.x + last.x) / 2;
				rect.w = 1;
			}
			break;
		case 'U': // UNKNOWN
			break;
		}

		// center of mass of the remaining rectangle
		point_t cm = { rect.p0.x + rect.w / 2, rect.p0.y + rect.h / 2 };

		// three possible jumps
		int target_x = MAX(0, MIN(2 * cm.x - jump.x - (!(rect.w % 2) ? 1 : 0), building_x - 1));
		if (target_x == jump.x)
			target_x = jump.x > 0 ? jump.x - 1 : jump.x < building_x - 1 ? jump.x + 1 : jump.x;
		int target_y = MAX(0, MIN(2 * cm.y - jump.y - (!(rect.h % 2) ? 1 : 0), building_y - 1));
		if (target_y == jump.y)
			target_y = jump.y > 0 ? jump.y - 1 : jump.y < building_y - 1 ? jump.y + 1 : jump.y;
		jumps[H] = (point_t){ target_x, jump.y };
		jumps[V] = (point_t){ jump.x, target_y };
		jumps[D] = (point_t){ rect.p0.x + 2 * rect.w / 3, rect.p0.y + 2 * rect.h / 3 };

		int score_jumps[2];
		int cut_x = (target_x + jump.x) / 2;
		score_jumps[H] = ((rect.p0.x <= cut_x && cut_x < rect.p1.x ||
					cut_x == rect.p1.x && !((target_x + jump.x) % 2)) && target_x != jump.x && rect.w > 1) * rect.h;
		int cut_y = (target_y + jump.y) / 2;
		score_jumps[V] = ((rect.p0.y <= cut_y && cut_y < rect.p1.y ||
					cut_y == rect.p1.y && !((target_y + jump.y) % 2)) && target_y != jump.y && rect.h > 1) * rect.w;

		// jump favors shorter sides
		// diagonal jump occurs only if 'cut' falls outside of rectangle
		dir = !score_jumps[H] && !score_jumps[V] ? D :
				score_jumps[H] > score_jumps[V] ? H :
						score_jumps[H] < score_jumps[V] ? V :
								rect.w % 2 || !(rect.h % 2) ? H : V;

		last = jump;
		jump = jumps[dir];

		printf("%d %d\n", jump.x, jump.y);
	}
}
