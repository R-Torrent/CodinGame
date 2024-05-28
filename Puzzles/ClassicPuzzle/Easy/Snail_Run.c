#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Snail Run
 * Puzzles > Classic Puzzle > Easy
 */

int main()
{
	int number_snails;
	scanf("%d", &number_snails);

	int speed_snail[6];
	for (int snail = 0; snail < number_snails; snail++)
		scanf("%d", speed_snail + snail);

	int map_height, map_width;
	scanf("%d%d", &map_height, &map_width);

	char map[6][10], starting_positions[6][2];
	for (int r = 0; r < map_height; r++) {
		char *row = map[r];
		scanf("%[^\n]", row); fgetc(stdin);
		for (char *c = row; *c; c++)
			if (isdigit(*c)) {
				int id = *c - '0' - 1;
				starting_positions[id][0] = r;       // row
				starting_positions[id][1] = c - row; // column
			}
	}

	int time_to_targets[6];
	for (int snail = 0; snail < 6; snail++)
		time_to_targets[snail] = 13;
	for (int r = 0; r < map_height; r++)
		for (int c = 0; c < map_width; c++)
			if (map[r][c] == '#')
				for (int snail = 0; snail < number_snails; snail++) {
					int d = abs(r - starting_positions[snail][0]) +
							abs(c - starting_positions[snail][1]);
					int t = d / speed_snail[snail] +
							(d % speed_snail[snail] != 0);
					if (t < time_to_targets[snail])
						time_to_targets[snail] = t;
				}

	int t = 13, winner;
	for (int snail = 0; snail < number_snails; snail++)
		if (time_to_targets[snail] < t) {
			t = time_to_targets[snail];
			winner = snail;
		}

	printf("%d\n", ++winner);

	return 0;
}
