#include <stdio.h>

/*
 * Summer Challenge 2024
 * Contest
 */

/*
 * Bronze League
 */

#define TURN \
X(UP)        \
X(DOWN)      \
X(LEFT)      \
X(RIGHT)     \
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

int player_idx, loop;

int main()
{
	int nb_games; // nb_games = 4
	scanf("%d%d", &player_idx, &nb_games);

	// game loop
	while (1) {
		char score_info[3][65];
		for (int i = 0; i < 3; i++)
			scanf("\n%[^\n]\n", score_info[i]);
		register_t *reg, registers[NUMBER_MGAMES];
		for (reg = registers; reg - registers < nb_games; reg++) {
			scanf("%s", reg->gpu);
			for (int i = 0; i < 7; i++)
				scanf("%d", reg->reg + i);
		};

		fprintf(stderr, "** Turn #%03d **\n", ++loop);

		func_mgame mgames[] = { hurdles, archery, skating, diving };
		array_scores indiv[NUMBER_MGAMES]; // 4 mini-game scores
		enum games mg;
		enum ops op;
		for (mg = 0; mg < nb_games; mg++) {
			mgames[mg](registers + mg, indiv + mg);
			fprintf(stderr, "Mini-game: %s\n", played[mg]); 
			for (op = 0; op < NUMBER_OPS; op++)
				fprintf(stderr, "  %s : %02d\n", output[op], indiv[mg][op]);
		}
		array_scores *sc, tally = { 0 }; // combined scores
		for (sc = indiv; sc - indiv < nb_games; sc++)
			for (op = 0; op < NUMBER_OPS; op++)
				tally[op] += (*sc)[op];
		fprintf(stderr, "TOTAL SCORE:\n"); 
		for (op = 0; op < NUMBER_OPS; op++)
			fprintf(stderr, "  %s : %02d\n", output[op], tally[op]);
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

#define GAMEOVER = "GAME_OVER"

#define TRACK_LEN 30

// Hurdle Race mini-game
void hurdles(register_t *reg, array_scores *sc)
{
	// reset turn or stunned runner
	if (!strcmp(reg->gpu, GAMEOVER) || reg->reg[player_idx + 3]) {
		(*sc)[UP] = 0;    // +2 and jump
		(*sc)[DOWN] = 0;  // +2
		(*sc)[LEFT] = 0;  // +1
		(*sc)[RIGHT] = 0; // +3
		return;
	}

	int pos = reg->reg[player_idx]; // current position
	int aim;                        // position runner aims at
	for (aim = pos; aim < TRACK_LEN && reg->gpu[aim] == '.'; aim++)
		;
	switch (aim - pos) {
	case 1: // next to hurdle or finish line
		(*sc)[UP] = aim != TRACK_LEN ? 10 : 0;
		(*sc)[DOWN] = 0;
		(*sc)[LEFT] = 0;
		(*sc)[RIGHT] = 0;
		return;
	case 2: // one space to hurdle or finish line
		(*sc)[UP] = aim != TRACK_LEN ? 0 : 10;
		(*sc)[DOWN] = aim != TRACK_LEN ? 0 : 10;
		(*sc)[LEFT] = aim != TRACK_LEN ? 10 : 5;
		(*sc)[RIGHT] = aim != TRACK_LEN ? 0 : 10;
		return;
	case 3: // two spaces to hurdle ot finish line
		(*sc)[UP] = aim != TRACK_LEN ? 10 : 7;
		(*sc)[DOWN] = aim != TRACK_LEN ? 10 : 7;
		(*sc)[LEFT] = aim != TRACK_LEN ? 5 : 3;
		(*sc)[RIGHT] = aim != TRACK_LEN ? 0 : 10;
		return;
	default: // three+ spaces to hurdle or final line
		(*sc)[UP] = 7;
		(*sc)[DOWN] = 7;
		(*sc)[LEFT] = 3;
		(*sc)[RIGHT] = 10;
		return;
	}
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
