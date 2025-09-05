#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * CodinGame Sponsored Contest 
 * Solo > Optimization
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

typedef enum {
	RIGHT,
	STILL,
	UP,
	DOWN,
	LEFT
} move_t;

typedef struct {
	int x;
	int y;
	char c;
} character_t;

int main()
{
	int first_init_input;
	scanf("%d", &first_init_input);
	int second_init_input;
	scanf("%d", &second_init_input);
	int third_init_input;
	scanf("%d", &third_init_input); fgetc(stdin);
	const size_t size = first_init_input * second_init_input;
	fprintf(stderr, "1: %d\n", first_init_input);
	fprintf(stderr, "2: %d\n", second_init_input);
	fprintf(stderr, "3: %d\n", third_init_input);
	fprintf(stderr, "-----\n");
	character_t *character = malloc(third_init_input * sizeof(character_t));
	char (*map)[second_init_input] = malloc(size);
	int i, j;
	for (i = 0; i < third_init_input - 1; i++)
		character[i].c = 'C';
	character[i].c = '@';
	// game loop
	for (int turn = 1; ; turn++) {
		char first_input;
		scanf("%c", &first_input); fgetc(stdin);
		char second_input;
		scanf("%c", &second_input); fgetc(stdin);
		char third_input;
		scanf("%c", &third_input); fgetc(stdin);
		char fourth_input;
		scanf("%c", &fourth_input);
		fprintf(stderr, "turn #: %d\n", turn);
		fprintf(stderr, "1: %c\n", first_input);
		fprintf(stderr, "2: %c\n", second_input);
		fprintf(stderr, "3: %c\n", third_input);
		fprintf(stderr, "4: %c\n", fourth_input);

		for (i = 0; i < third_init_input; i++) {
			int fifth_input;
			int sixth_input;
			scanf("%d%d", &fifth_input, &sixth_input); fgetc(stdin);
			fprintf(stderr, "5 6: %d %d\n", fifth_input, sixth_input);
			character[i].x = fifth_input;
			character[i].y = sixth_input;
		}

		memset(map, '.', size);
		for (character_t *ch = character; ch - character < third_init_input; ch++)
			map[ch->y][ch->x] = ch->c;
		for (j = 0; j < first_init_input; j++)
			fprintf(stderr, "%.*s\n", second_init_input, map[j]);

		// Write an action using printf(). DON'T FORGET THE TRAILING \n
		// To debug: fprintf(stderr, "Debug messages...\n");

		printf("%c\n", (turn % 2 ? LEFT : RIGHT) + 'A');
	}
	free(character);
	free(map);
	return 0;
}
