#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
	int player_idx;
	scanf("%d", &player_idx);
	int nb_games;
	scanf("%d", &nb_games); fgetc(stdin);

	// game loop
	while (1) {
		for (int i = 0; i < 3; i++) {
			char score_info[65];
			scanf("%[^\n]", score_info); fgetc(stdin);
		}
		for (int i = 0; i < nb_games; i++) {
			char gpu[65];
			int reg_0;
			int reg_1;
			int reg_2;
			int reg_3;
			int reg_4;
			int reg_5;
			int reg_6;
			scanf("%s%d%d%d%d%d%d%d", gpu, &reg_0, &reg_1, &reg_2, &reg_3, &reg_4, &reg_5, &reg_6); fgetc(stdin);
		}

		// Write an action using printf(). DON'T FORGET THE TRAILING \n
		// To debug: fprintf(stderr, "Debug messages...\n");

		printf("LEFT\n");
	}

	return 0;
}
