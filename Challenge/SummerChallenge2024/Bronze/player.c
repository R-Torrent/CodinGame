#include <stdio.h>

/*
 * Summer Challenge 2024
 * Contest
 */

/*
 * Bronze League
 */

#define TURN \
X(LEFT)      \
X(DOWN)      \
X(RIGHT)     \
X(UP)        \
X(NUMBER_OPS) // 4

#define X(a) #a,
char *output[] = {
	TURN
};
#undef X

#define X(a) a,
enum ops {
	TURN
};
#undef X

#define MGAME           \
Y(HURDLE_RACE)          \
Y(ARCHERY)              \
Y(ROLLER_SPEED_SKATING) \
Y(DIVING)               \
Y(NUMBER_MGAMES) // 4

#define Y(a) #a,
char *played[] = {
	MGAME
};
#undef Y

#define Y(a) a,
enum games {
	MGAME
};
#undef Y

#define TRACK_LEN 30

typedef struct {
	char gpu[65];
	int reg[7];
} register_t;

typedef short array_scores[NUMBER_OPS];

void hurdles(register_t *, array_scores *);
void archery(register_t *, array_scores *);
void skating(register_t *, array_scores *);
void diving(register_t *, array_scores *);

typedef void (*func_mgame)(register_t *, array_scores *);

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

		func_mgame mgames[] = { hurdles, archery, skating, diving };
		array_scores indiv[NUMBER_MGAMES]; // 4 mini-game scores
		enum games mg;
		for (mg = 0; mg < nb_games; mg++)
			mgames[mg](registers + mg, indiv + mg);
		array_scores *sc, tally = { 0 }; // combined scores
		enum ops op;
		for (sc = indiv; sc - indiv < nb_games; sc++)
			for (op = 0; op < NUMBER_OPS; op++)
				tally[op] += (*sc)[op];
		enum ops max_op;
		short max_score = 0;
		for (op = 0; op < NUMBER_OPS; op++)
			if (tally[op] > max_score) {
				max_score = tally[op];
				max_op = op;
			}

		printf("%s\n", output[max_op]);
	}

	return 0;
}

// Hurdle Race mini-game
void hurdles(register_t *reg, array_scores *sc)
{
}

// Archery mini-game
void archery(register_t *reg, array_scores *sc)
{
}

// Roller Speed Skating
void skating(register_t *reg, array_scores *sc)
{
}

// Diving
void diving(register_t *reg, array_scores *sc)
{
}

/*
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
	}
*/
