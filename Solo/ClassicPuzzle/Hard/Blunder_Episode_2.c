#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Blunder - Episode 2
 * Puzzles > Classic Puzzle > Hard
 */

typedef struct room {
	int n; // room number (room N == E)
	int m; // money
	bool v; // visited
	struct room *d1; // door 1
	struct room *d2; // door 2
} room_t;

int main()
{
	int N;
	scanf("%d\n", &N);

	room_t building[10000];
	for (room_t *room = building; room - building < N; room++) {
		char door1[5], door2[5];
		scanf("%d%d%s %[^\n]\n", &room->n, &room->m, door1, door2);
		room->v = false;
		room->d1 = strcmp(door1, "E") ? building + atoi(door1) : building + N;
		room->d2 = strcmp(door2, "E") ? building + atoi(door2) : building + N;
	}
	building[N] = (room_t){ .n = N, .v = true };

	for (room_t *room = building; !building->v; room++) {
		if (room->n == N)
			room = building;
		if (room->v)
			continue;
		if (room->d1->v && room->d2->v) {
			int m1 = room->d1->m;
			int m2 = room->d2->m;
			room->m += m1 > m2 ? m1 : m2;
			room->v = true;
		}
	}

	printf("%d\n", building->m);

	return 0;
}
