#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
 * Paper labyrinth
 * Puzzles > Classic Puzzle > Medium
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
	int xs;
	int ys;
	scanf("%d%d", &xs, &ys);
	int xr;
	int yr;
	scanf("%d%d", &xr, &yr);
	int w;
	int h;
	scanf("%d%d", &w, &h);
	for (int i = 0; i < h; i++) {
		char l[w + 1];
		scanf("%s", l);
	}

	// Write an answer using printf(). DON'T FORGET THE TRAILING \n
	// To debug: fprintf(stderr, "Debug messages...\n");

	printf("go return\n");

	return 0;
}
