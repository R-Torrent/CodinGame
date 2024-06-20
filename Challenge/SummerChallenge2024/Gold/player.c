#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Summer Challenge 2024
 * Contest
 */

/*
 * Gold League
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
} registers_t;

typedef float array_scores[NUMBER_OPS];

void hurdles(registers_t *, array_scores *);
void archery(registers_t *, array_scores *);
void skating(registers_t *, array_scores *);
void diving(registers_t *, array_scores *);

typedef void (*func_mgame)(registers_t *, array_scores *);

int playable_game(registers_t *, enum games);

#define GAMEOVER "GAME_OVER"
#define GAME_DURATION 100

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) < 0 ? (-a) : (a))
#define SIGN(a) ((a) < 0 ? -1 : (a) > 0 ? 1 : 0)

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
		float game_multiplier[NUMBER_MGAMES];
		for (mg = 0; mg < nb_games; mg++) {
			float average_other = 0.0F, deviation;; 
			for (int i = 1; i < NUMBER_MGAMES; i++)
				average_other += detailed_score[(mg + i) % NUMBER_MGAMES];
			average_other /= (float)(NUMBER_MGAMES - 1);
			deviation = (float)detailed_score[mg] - average_other;
			// linear response, capped below at 1.0
			// dev +10 -> lin response = 0.0 -> 1.0
			// dev -10 -> lin response = 3.0
			game_multiplier[mg] = MAX(-3.0F/20.0F * (deviation - 10.0F), 1.0F);
		}

		registers_t *reg, registers[NUMBER_MGAMES];
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
                        fprintf(stderr,"%s: %d pts. -> %.2f mult.\n",
					played[mg], detailed_score[mg], game_multiplier[mg]);
			for (op = 0; op < NUMBER_OPS; op++)
				fprintf(stderr, "  %s : %+06.2f\n", output[op], indiv[mg][op]);
		}
		array_scores tally = { 0.0F }; // combined scores
		for (mg = 0; mg < nb_games; mg++)
			for (op = 0; op < NUMBER_OPS; op++)
				tally[op] += indiv[mg][op] * game_multiplier[mg];
		fprintf(stderr, "TOTAL SCORE:\n");
		for (op = 0; op < NUMBER_OPS; op++)
			fprintf(stderr, "  %s : %+06.2f\n", output[op], tally[op]);
		enum ops max_op = 0;
		float max_score = 0.0F;
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
void hurdles(registers_t *reg, array_scores *sc)
{
	// reset turn or stunned runner
	if (!strcmp(reg->gpu, GAMEOVER) || reg->reg[player_idx + 3] || !playable_game(reg, HURDLE_RACE)) {
		(*sc)[UP] = 0.0F;    // +2 and jump
		(*sc)[DOWN] = 0.0F;  // +2
		(*sc)[LEFT] = 0.0F;  // +1
		(*sc)[RIGHT] = 0.0F; // +3
		return;
	}

	const short pos = reg->reg[player_idx]; // current position
	short aim;                              // position runner aims at
	for (aim = pos + 1; aim < TRACK_LEN && reg->gpu[aim] == '.'; aim++)
		;
	switch (aim - pos) { // finish line = TRACK_LEN - 1
	case 1: // next to hurdle
		(*sc)[UP] = 12.0F;
		(*sc)[DOWN] = -2.0F;
		(*sc)[LEFT] = -2.0F;
		(*sc)[RIGHT] = -2.0F;
		return;
	case 2: // one space to hurdle or next to finish line
		(*sc)[UP] = aim != TRACK_LEN ? -2.0F : 0.0F;
		(*sc)[DOWN] = aim != TRACK_LEN ? -2.0F : 0.0F;
		(*sc)[LEFT] = aim != TRACK_LEN ? 12.0F : 0.0F;
		(*sc)[RIGHT] = aim != TRACK_LEN ? -2.0F : 0.0F;
		return;
	case 3: // two spaces to hurdle or one space to finish line
		(*sc)[UP] = 10.0F;
		(*sc)[DOWN] = 10.0F;
		(*sc)[LEFT] = 5.0F;
		(*sc)[RIGHT] = aim != TRACK_LEN ? -2.0F : 10.0F;
		return;
	default: // three+ spaces to hurdle or finish line
		(*sc)[UP] = 6.67F;
		(*sc)[DOWN] = 6.67F;
		(*sc)[LEFT] = 3.33F;
		(*sc)[RIGHT] = 10.0F;
		return;
	}
}

/*
short root2(const short S, short x0)
{
	short x1;
	for (int i = 0; x0 && i < 3; i++, x0 = x1)
		x1 = (x0 + S / x0) >> 1; // Heron's method

	return x0;
}
*/

void best_archery_solutions(float *dst, const short x0, const short y0, char *winds, short shots)
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
			float dst2[NUMBER_OPS];
			best_archery_solutions(dst2, x1[op], y1[op], winds + 1, shots);
			dst[op] = MIN(dst2[UP], MIN(dst2[DOWN], MIN(dst2[LEFT], dst2[RIGHT])));
		}
	else
		for (enum ops op = 0; op < NUMBER_OPS; op++)
			dst[op] = sqrtf(x1[op] * x1[op] + y1[op] * y1[op]);
			/* dst[op] = root2(x1[op] * x1[op] + y1[op] * y1[op],
					 (ABS(x1[op]) + ABS(y1[op]))); */
}

#define SHOTS_TO_CONSIDER 3 // NOTE: # of cases to consider grows as 4^(SHOTS_TO_CONSIDER)

// Archery mini-game
void archery(registers_t *reg, array_scores *sc)
{
	if (!strcmp(reg->gpu, GAMEOVER) || !playable_game(reg, ARCHERY)) {
		(*sc)[UP] = 0.0F;
		(*sc)[DOWN] = 0.0F;
		(*sc)[LEFT] = 0.0F;
		(*sc)[RIGHT] = 0.0F;
		return;
	}

	const short remaining = strlen(reg->gpu);
	// ( x0, y0 ): current cursor position
	const short x0 = reg->reg[player_idx << 1];
	const short y0 = reg->reg[(player_idx << 1) + 1];
	// Euclidean: measure of distance to bullseye
	float Euclidean[NUMBER_OPS];
	best_archery_solutions(Euclidean, x0, y0, reg->gpu,
			remaining <= SHOTS_TO_CONSIDER ? remaining : 1);
	for (enum ops op = 0; op < NUMBER_OPS; op++) {
		// lineal response; max = 10, indifference at 10 away (slope -1)
		(*sc)[op] = 10.0F - Euclidean[op];
		// only the last four turns really matter
		(*sc)[op] *= (remaining < 5 ? 4.0F : remaining < 9 ? 3.0F : 1.0F) / 3.0F;
	}
}

// Roller Speed Skating
void skating(registers_t *reg, array_scores *sc)
{
	// reset turn or stunned skater
	if (!strcmp(reg->gpu, GAMEOVER) || reg->reg[player_idx + 3] < 0
			|| !playable_game(reg, ROLLER_SPEED_SKATING)) {
		(*sc)[UP] = 0.0F;
		(*sc)[DOWN] = 0.0F;
		(*sc)[LEFT] = 0.0F;
		(*sc)[RIGHT] = 0.0F;
		return;
	}

	const short travelled[3] = {
		reg->reg[player_idx],           // me
		reg->reg[(player_idx + 1) % 3], // opponent 1
		reg->reg[(player_idx + 2) % 3]  // opponent 2
	};
	const float advantage = (float)SIGN(travelled[0] - MAX(travelled[1], travelled[2]));
	const short risk = reg->reg[player_idx + 3];
	short risk_order; // stores the index in gpu of each operator
	for (enum ops op = 0; op < NUMBER_OPS; op++) {
		risk_order = strchr(reg->gpu, initials[op]) - reg->gpu;
		switch (risk_order) {
		case 0: // player +1 risk -1
			(*sc)[op] = 5.0F + (float)risk;
			break;
		case 1: // player +2
			(*sc)[op] = 10.0F;
			break;
		case 2: // player +2 risk +1
			(*sc)[op] = 8.0F - (float)(risk << 1) - advantage;
			break;
		case 3: // player +3 risk +2
			(*sc)[op] = 5.0F - (float)(risk << 1) - advantage;
			break;
		}
	}
}

// Diving
void diving(registers_t *reg, array_scores *sc)
{
	if (!strcmp(reg->gpu, GAMEOVER) || !playable_game(reg, DIVING)) {
		(*sc)[UP] = 0.0F;
		(*sc)[DOWN] = 0.0F;
		(*sc)[LEFT] = 0.0F;
		(*sc)[RIGHT] = 0.0F;
		return;
	}

	const char correct = *reg->gpu;
	for (enum ops op = 0; op < NUMBER_OPS; op++)
		(*sc)[op] = initials[op] == correct ?
				(float)MIN((7 + reg->reg[player_idx + 3]), 12) : 0.0F;
}

// Arithmetic series SERIES(a, b) = a + (a+1) + (a+2) + ... + (b-1) + b
#define SERIES(a, b) ((((a) + (b)) * ((b) - (a) + 1)) >> 1)

// Checks if there is actually enough time to complete the running mini-game
int playable_game(registers_t *reg, enum games mg)
{
	const short remaining = GAME_DURATION - loop;
	const short current[3] = {
		reg->reg[player_idx],           // me
		reg->reg[(player_idx + 1) % 3], // opponent 1
		reg->reg[(player_idx + 2) % 3]  // opponent 2
	};
	short turns, leader, potential[3], lazy_potential_pos, max_potential_pos;

	switch (mg) {
	case HURDLE_RACE:
		leader = MAX(current[0], MAX(current[1], current[2]));
		for (turns = 0; leader < TRACK_LEN - 1; turns++) {
			short aim;
			for (aim = leader + 1; aim < TRACK_LEN && reg->gpu[aim] == '.'; aim++)
				;
			short diff = aim - leader - 1;
			leader += diff ? MIN(3, diff) : 2;
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
