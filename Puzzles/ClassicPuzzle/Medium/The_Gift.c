#include <stdio.h>
#include <stdlib.h>

/*
 * The Gift
 * Puzzles > Classic Puzzle > Medium
 */

int compare(const void *a, const void *b) {
	return *(int *)a - *(int *)b;
}

int main()
{
	int N, C;
	scanf("%d%d", &N, &C);
	int B[2000], Sol[2000];
	for (int i = 0; i < N; i++)
		scanf("%d", B + i);

	while (C) {
		int C0 = C;
		for (int i = 0; C && i < N; i++)
			if (B[i]) {
				B[i]--;
				Sol[i]++;
				C--;
			}
		if (C0 == C)
			break;
	}

	if (C)
		printf("IMPOSSIBLE\n");
	else {
		qsort(Sol, N, sizeof(int), compare);
		for (int i = 0; i < N; i++)
			printf("%d\n", Sol[i]);
	}

	return 0;
}
