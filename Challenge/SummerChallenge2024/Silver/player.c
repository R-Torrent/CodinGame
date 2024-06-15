#include <stdio.h>
#include <string.h>

/*
 * Summer Challenge 2024
 * Contest
 */

/*
 * Silver League
 */

#define TURN    \
X(UP, U)        \
X(DOWN, D)      \
X(LEFT, L)      \
X(RIGHT, R)     \
X(NUMBER_OPS, N) // 4

#define X(a, b) #a,
char *output[] = {
	TURN
};
#undef X

#define X(a, b) a,
enum ops {
	TURN
};
#undef X

#define X(a, b) (#b)[0],
char initials[] = {
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

#define GAMEOVER "GAME_OVER"

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

		fprintf(stderr, "** Turn #%03d **\n", loop++);

		func_mgame mgames[] = { hurdles, archery, skating, diving };
		array_scores indiv[NUMBER_MGAMES]; // 4 mini-game scores
		enum games mg;
		enum ops op;
		for (mg = 0; mg < nb_games; mg++) {
			mgames[mg](registers + mg, indiv + mg);
			fprintf(stderr, "Mini-game: %s\n", played[mg]);
			for (op = 0; op < NUMBER_OPS; op++)
				fprintf(stderr, "  %s : %+03hd\n", output[op], indiv[mg][op]);
		}
		array_scores *sc, tally = { 0 }; // combined scores
		for (sc = indiv; sc - indiv < nb_games; sc++)
			for (op = 0; op < NUMBER_OPS; op++)
				tally[op] += (*sc)[op];
		fprintf(stderr, "TOTAL SCORE:\n");
		for (op = 0; op < NUMBER_OPS; op++)
			fprintf(stderr, "  %s : %+03hd\n", output[op], tally[op]);
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

	short pos = reg->reg[player_idx]; // current position
	short aim;                        // position runner aims at
	for (aim = pos + 1; aim < TRACK_LEN && reg->gpu[aim] == '.'; aim++)
		;
	switch (aim - pos) { // finish line = TRACK_LEN - 1
	case 1: // next to hurdle
		(*sc)[UP] = 12;
		(*sc)[DOWN] = -2;
		(*sc)[LEFT] = -2;
		(*sc)[RIGHT] = -2;
		return;
	case 2: // one space to hurdle or next to finish line
		(*sc)[UP] = aim != TRACK_LEN ? -2 : 0;
		(*sc)[DOWN] = aim != TRACK_LEN ? -2 : 0;
		(*sc)[LEFT] = aim != TRACK_LEN ? 12 : 0;
		(*sc)[RIGHT] = aim != TRACK_LEN ? -2 : 0;
		return;
	case 3: // two spaces to hurdle or one space to finish line
		(*sc)[UP] = 10;
		(*sc)[DOWN] = 10;
		(*sc)[LEFT] = 5;
		(*sc)[RIGHT] = aim != TRACK_LEN ? -2 : 10;
		return;
	default: // three+ spaces to hurdle or finish line
		(*sc)[UP] = 7;
		(*sc)[DOWN] = 7;
		(*sc)[LEFT] = 3;
		(*sc)[RIGHT] = 10;
		return;
	}
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) < 0 ? (-a) : (a))
#define SIGN(a) ((a) < 0 ? -1 : (a) > 0 ? 1 : 0)

short root2(short S, short x0)
{
	short x1;
	for (int i = 0; x0 && i < 3; i++, x0 = x1)
		x1 = (x0 + S / x0) >> 1; // Heron's method

	return x0;
}

// Archery mini-game
void archery(register_t *reg, array_scores *sc)
{
	if (!strcmp(reg->gpu, GAMEOVER)) {
		(*sc)[UP] = 0;
		(*sc)[DOWN] = 0;
		(*sc)[LEFT] = 0;
		(*sc)[RIGHT] = 0;
		return;
	}

	short wind = reg->gpu[0] - '0';
	// ( x0, y0 ): current cursor position
	// ( x1, y1 ): next cursor position, for each direction
	short x0 = reg->reg[player_idx << 1], x1[NUMBER_MGAMES];
	short y0 = reg->reg[(player_idx << 1) + 1], y1[NUMBER_MGAMES];
	x1[UP] = x0, y1[UP] = MAX(y0 - wind, -20);
	x1[DOWN] = x0; y1[DOWN] = MIN(y0 + wind, 20);
	x1[LEFT] = MAX(x0 - wind, -20), y1[LEFT] = y0;
	x1[RIGHT] = MIN(x0 + wind, 20), y1[RIGHT] = y0;
	// euclid: measure of distance to bullseye
	short euclid;
	for (enum ops op = 0; op < NUMBER_OPS; op++) {
		euclid = root2(x1[op] * x1[op] + y1[op] * y1[op], (ABS(x1[op]) + ABS(y1[op])));
		// lineal response; max = 10, indifference at 15 away
		(*sc)[op] = 10 - (10 * euclid / 15);
	}
}

// Roller Speed Skating
void skating(register_t *reg, array_scores *sc)
{
	// reset turn or stunned skater
	if (!strcmp(reg->gpu, GAMEOVER) || reg->reg[player_idx + 3] < 0) {
		(*sc)[UP] = 0;
		(*sc)[DOWN] = 0;
		(*sc)[LEFT] = 0;
		(*sc)[RIGHT] = 0;
		return;
	}

	short travelled[3] = {
		reg->reg[player_idx],           // me
		reg->reg[(player_idx + 1) % 3], // opponent 1
		reg->reg[(player_idx + 2) % 3]  // opponent 2
	};
	short advantage = SIGN(travelled[0] - MAX(travelled[1], travelled[2]));
	short risk = reg->reg[player_idx + 3];
	short risk_order; // stores the index in gpu of each operator
	for (enum ops op = 0; op < NUMBER_OPS; op++) {
		risk_order = strchr(reg->gpu, initials[op]) - reg->gpu;
		switch (risk_order) {
		case 0: // player +1 risk -1
			(*sc)[op] = 5 + risk;
			break;
		case 1: // player +2
			(*sc)[op] = 10;
			break;
		case 2: // player +2 risk +1
			(*sc)[op] = 8 - (risk << 1) - advantage;
			break;
		case 3: // player +3 risk +2
			(*sc)[op] = 5 - (risk << 1) - advantage;
			break;
		}
	}
}

// Diving
void diving(register_t *reg, array_scores *sc)
{
	if (!strcmp(reg->gpu, GAMEOVER)) {
		(*sc)[UP] = 0;
		(*sc)[DOWN] = 0;
		(*sc)[LEFT] = 0;
		(*sc)[RIGHT] = 0;
		return;
	}

	char correct = *reg->gpu;
	for (enum ops op = 0; op < NUMBER_OPS; op++)
		(*sc)[op] = initials[op] == correct ? (6 + reg->reg[player_idx + 3]) : 0;
}
