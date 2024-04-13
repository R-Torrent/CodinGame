#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Stock Exchange Losses
 * Puzzles > Classic Puzzle > Medium
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
    int n, loss = 0;
    scanf("%d", &n);
	int *v = malloc(n * sizeof(int));
    for (int *w = v; w < v + n; w++)
        scanf("%d", w);

    // Write an answer using printf(). DON'T FORGET THE TRAILING \n
    // To debug: fprintf(stderr, "Debug messages...\n");

	int *min_thereafter = malloc(n * sizeof(int));
	int minimum = INT_MAX;
	for (int i = n - 1; i >=0; i--) {
		if (v[i] < minimum)
			minimum = v[i];
		min_thereafter[i] = minimum;
	}
	int l;
	for (int i = 0; i < n - 1; i++)
		if ((l = min_thereafter[i] - v[i]) < loss)
			loss = l;

    printf("%d\n", loss);

	free(v);
	free(min_thereafter);
    return 0;
}
