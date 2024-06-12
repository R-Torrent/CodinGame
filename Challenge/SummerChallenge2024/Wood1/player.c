#include <stdio.h>

/*
 * Summer Challenge 2024
 * Contest
 */

/*
 * Wood 1 League
 */

#define TURN \
X(LEFT, 1)   \
X(DOWN, 2)   \
X(RIGHT, 3)  \
X(UP, 2)

#define X(a, b) #a,
char *output[] = {
	TURN
};
#undef X

#define X(a, b) a,
enum op {
	TURN
};
#undef X

#define TRACK_LEN 30

typedef struct {
	char gpu[65];
	int reg[7];
} register_t;

int main()
{
	int player_idx, nb_games; // nb_games = 4
	scanf("%d%d", &player_idx, &nb_games);

	// game loop
	while (1) {
		char score_info[3][65];
		for (int i = 0; i < 3; i++)
			scanf("\n%[^\n]\n", score_info[i]);
		register_t *reg, registers[4];
		for (reg = registers; reg - registers < nb_games; reg++) {
			scanf("%s", reg->gpu);
			for (int i = 0; i < 7; i++)
				scanf("%d", reg->reg + i);
		};

		// Four simultaneous Hurdle Race mini-games
		int min_stretch = 5, stretch;
		for (reg = registers; reg - registers < nb_games; reg++) {
			int pos = reg->reg[player_idx]; // current position
			int aim;                        // position runner aims at
			for (aim = pos; aim < TRACK_LEN && reg->gpu[aim] == '.'; aim++)
				;
			// final sprint OR reset turn, 'GAME_OVER'
			if ((aim == TRACK_LEN) || !(stretch = aim - pos))
				continue;
			else if (stretch < min_stretch)
				min_stretch = stretch;
		}

		char *order;
		switch (min_stretch) {
			case 1: order = output[UP]; break;
			case 2: order = output[LEFT]; break;
			case 3: order = output[DOWN]; break;
			default: order = output[RIGHT];
		}
		printf("%s\n", order);
	}

	return 0;
}
