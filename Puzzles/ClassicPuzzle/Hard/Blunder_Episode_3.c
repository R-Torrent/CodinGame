#include <math.h>
#include <stdio.h>

/*
 * Blunder - Episode 3
 * Puzzles > Classic Puzzle > Hard
 */

char *answer[] = { "O(1)", "O(log n)", "O(n)", "O(n log n)",
	"O(n^2)", "O(n^2 log n)", "O(n^3)", "O(2^n)" };

// Solved with linear regression, the parameters calculated through
// ordinary least squares (OLS).

int main()
{
	int N;
	scanf("%d", &N);
	float y[999], x[999][8], xt[8][999], xt_x[8][8], inv_xt_x[8][8];
	for (int i = 0; i < N; i++) {
		float n;
		scanf("%f%f", &y[i], &n);
		xt[0][i] = x[i][0] = 1.0;               // 1
		xt[1][i] = x[i][1] = logf(n);           // log(n)
		xt[2][i] = x[i][2] = n;                 // n
		xt[3][i] = x[i][3] = x[i][1] * x[i][2]; // n * log(n)
		xt[4][i] = x[i][4] = x[i][2] * x[i][2]; // n ^ 2
		xt[5][i] = x[i][5] = x[i][1] * x[i][4]; // n ^ 2 * log(n)
		xt[6][i] = x[i][6] = x[i][2] * x[i][4]; // n ^ 3
		xt[7][i] = x[i][7] = powf(2.0F, n);     // 2 ^ n
	}

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++) {
			xt_x[i][j] = 0;
			for (int k = 0; k < 999; k++)
				xt_x[i][j] += xt[i][k] * x[k][j];
		}

	printf("answer\n");

	return 0;
}
