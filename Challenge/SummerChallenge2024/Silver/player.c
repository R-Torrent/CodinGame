#include <stdio.h>
#include <stdlib.h>
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

int playable_game(register_t *, enum games);

#define GAMEOVER "GAME_OVER"
#define GAME_DURATION 100

int player_idx, loop;

int main()
{
	int nb_games; // nb_games = 4
	scanf("%d%d", &player_idx, &nb_games);

	// game loop
	for (loop = 0; ; loop++) {
		char score_info[3][65];
		int own_score, detailed_score[NUMBER_MGAMES];
		int own_medals[NUMBER_MGAMES][3]; // own medals
		for (int i = 0; i < 3; i++)
			scanf("\n%[^\n]\n", score_info[i]);
		own_score = atoi(strtok(score_info[player_idx], " "));
		for (int i = 0, *pm = own_medals[0]; i < NUMBER_MGAMES * 3; i++)
			*pm++ = atoi(strtok(NULL, " "));
		enum games mg;
		for (mg = 0; mg < nb_games; mg++)
			detailed_score[mg] = 3 * own_medals[mg][0] + own_medals[mg][1];

		register_t *reg, registers[NUMBER_MGAMES];
		for (reg = registers; reg - registers < nb_games; reg++) {
			scanf("%s", reg->gpu);
			for (int i = 0; i < 7; i++)
				scanf("%d", reg->reg + i);
		};

		fprintf(stderr, "** Turn #%03d **\n", loop);

		func_mgame mgames[] = { hurdles, archery, skating, diving };
		array_scores indiv[NUMBER_MGAMES]; // 4 mini-game scores
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
		enum ops max_op = 0;
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
	if (!strcmp(reg->gpu, GAMEOVER) || reg->reg[player_idx + 3] || !playable_game(reg, HURDLE_RACE)) {
		(*sc)[UP] = 0;    // +2 and jump
		(*sc)[DOWN] = 0;  // +2
		(*sc)[LEFT] = 0;  // +1
		(*sc)[RIGHT] = 0; // +3
		return;
	}

	const short pos = reg->reg[player_idx]; // current position
	short aim;                              // position runner aims at
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

short root2(const short S, short x0)
{
	short x1;
	for (int i = 0; x0 && i < 3; i++, x0 = x1)
		x1 = (x0 + S / x0) >> 1; // Heron's method

	return x0;
}

void best_archery_solutions(short *dst, const short x0, const short y0, char *winds, short shots)
{
	// ( x1, y1 ): next cursor position, for each direction
	short x1[NUMBER_OPS], y1[NUMBER_OPS];
	const short wind =  *winds - '0';

	x1[UP] = x0, y1[UP] = MAX(y0 - wind, -20);
	x1[DOWN] = x0; y1[DOWN] = MIN(y0 + wind, 20);
	x1[LEFT] = MAX(x0 - wind, -20), y1[LEFT] = y0;
	x1[RIGHT] = MIN(x0 + wind, 20), y1[RIGHT] = y0;
	if (--shots)
		for (enum ops op = 0; op < NUMBER_OPS; op++) {
			short dst2[NUMBER_OPS];
			best_archery_solutions(dst2, x1[op], y1[op], winds + 1, shots);
			dst[op] = MIN(dst2[UP], MIN(dst2[DOWN], MIN(dst2[LEFT], dst2[RIGHT])));
		}
	else
		for (enum ops op = 0; op < NUMBER_OPS; op++)
			 dst[op] = root2(x1[op] * x1[op] + y1[op] * y1[op],
					 (ABS(x1[op]) + ABS(y1[op])));
}

#define SHOTS_TO_CONSIDER 5 // NOTE: # of cases to consider grows as 4^(SHOTS_TO_CONSIDER)

// Archery mini-game
void archery(register_t *reg, array_scores *sc)
{
	if (!strcmp(reg->gpu, GAMEOVER) || !playable_game(reg, ARCHERY)) {
		(*sc)[UP] = 0;
		(*sc)[DOWN] = 0;
		(*sc)[LEFT] = 0;
		(*sc)[RIGHT] = 0;
		return;
	}

	const short remaining = strlen(reg->gpu);
	// ( x0, y0 ): current cursor position
	const short x0 = reg->reg[player_idx << 1];
	const short y0 = reg->reg[(player_idx << 1) + 1];
	// Euclidean: measure of distance to bullseye
	short Euclidean[NUMBER_OPS];
	best_archery_solutions(Euclidean, x0, y0, reg->gpu, MIN(SHOTS_TO_CONSIDER, remaining));
	for (enum ops op = 0; op < NUMBER_OPS; op++) {
		// lineal response; max = 10, indifference at 15 away
		(*sc)[op] = 10 - (10 * Euclidean[op] / 15);
		// only the last six turns really matter
		(*sc)[op] *= remaining < 7 ? 4 : remaining < 11 ? 3 : 1;
                (*sc)[op] /= 3;
	}
}

// Roller Speed Skating
void skating(register_t *reg, array_scores *sc)
{
	// reset turn or stunned skater
	if (!strcmp(reg->gpu, GAMEOVER) || reg->reg[player_idx + 3] < 0
			|| !playable_game(reg, ROLLER_SPEED_SKATING)) {
		(*sc)[UP] = 0;
		(*sc)[DOWN] = 0;
		(*sc)[LEFT] = 0;
		(*sc)[RIGHT] = 0;
		return;
	}

	const short travelled[3] = {
		reg->reg[player_idx],           // me
		reg->reg[(player_idx + 1) % 3], // opponent 1
		reg->reg[(player_idx + 2) % 3]  // opponent 2
	};
	const short advantage = SIGN(travelled[0] - MAX(travelled[1], travelled[2]));
	const short risk = reg->reg[player_idx + 3];
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
	if (!strcmp(reg->gpu, GAMEOVER) || !playable_game(reg, DIVING)) {
		(*sc)[UP] = 0;
		(*sc)[DOWN] = 0;
		(*sc)[LEFT] = 0;
		(*sc)[RIGHT] = 0;
		return;
	}

	const char correct = *reg->gpu;
	for (enum ops op = 0; op < NUMBER_OPS; op++)
		(*sc)[op] = initials[op] == correct ? MIN((7 + reg->reg[player_idx + 3]), 12) : 0;
}

// Arithmetic series SERIES(a, b) = a + (a+1) + (a+2) + ... + (b-1) + b
#define SERIES(a, b) ((((a) + (b)) * ((b) - (a) + 1)) >> 1)

// Checks if there is actually enough time to complete the running mini-game
int playable_game(register_t *reg, enum games mg)
{
	const short remaining = GAME_DURATION - loop;
	short current[3] = {
		reg->reg[player_idx],           // me
		reg->reg[(player_idx + 1) % 3], // opponent 1
		reg->reg[(player_idx + 2) % 3]  // opponent 2
	};
	short turns, potential[3], lazy_potential_pos, max_potential_pos;

	switch (mg) {
	case HURDLE_RACE:
		for (turns = 0; *current < TRACK_LEN - 1; turns++) {
			short aim;
			for (aim = *current + 1; aim < TRACK_LEN && reg->gpu[aim] == '.'; aim++)
				;
			short diff = aim - *current - 1;
			*current += diff ? MIN(3, diff) : 2;
		}

		return turns <= remaining;
	case ARCHERY: return strlen(reg->gpu) <= remaining;
	case ROLLER_SPEED_SKATING:
		turns = reg->reg[6];
		for (int i = 0; i < 3; i++)
			potential[i] = current[i] + 3 * turns;
		// without advancing a single space, opponents moving by 3 every time
		lazy_potential_pos = 1 + (potential[1] >= current[0]) + (potential[2] >= current[0]);
		// potentially sprinting by 3 spaces, opponents none
		max_potential_pos = 1 + (current[1] >= potential[0]) + (current[2] >= potential[0]);
		// (to bolster competitiveness, ties are considered a lost place)

		return turns <= remaining && lazy_potential_pos != 1 && max_potential_pos != 3;
	case DIVING: default:
		turns = strlen(reg->gpu);
		for (int i = 0; i < 3; i++) {
			short combo = reg->reg[(player_idx + i) % 3 + 3] + 1;
			potential[i] = current[i] + SERIES(combo, combo + turns - 1);
		}
		// without scoring a single extra point, opponents scoring every time
		lazy_potential_pos = 1 + (potential[1] >= current[0]) + (potential[2] >= current[0]);
		// potentially earning all the possible points, opponents none
		max_potential_pos = 1 + (current[1] >= potential[0]) + (current[2] >= potential[0]);
		// (to bolster competitiveness, ties are considered a lost place)

		return turns <= remaining && lazy_potential_pos != 1 && max_potential_pos != 3;
	}
}
