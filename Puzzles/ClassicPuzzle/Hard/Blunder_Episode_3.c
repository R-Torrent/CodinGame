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

float determinant(int, float [][8]);
float minor(int, float [][8], int, int);

int main()
{
	int N;
	scanf("%d", &N);

	float y[999], x[999][8], xt[8][999], xt_x[8][8], xt_y[8], b[8];
	for (int i = 0; i < N; i++) {
		float n;
		scanf("%f%f", &n, &y[i]);
		xt[0][i] = x[i][0] = 1.0F;              // 1
		xt[1][i] = x[i][1] = logf(n);           // log(n)
		xt[2][i] = x[i][2] = n;                 // n
		xt[3][i] = x[i][3] = x[i][1] * x[i][2]; // n * log(n)
		xt[4][i] = x[i][4] = x[i][2] * x[i][2]; // n ^ 2
		xt[5][i] = x[i][5] = x[i][1] * x[i][4]; // n ^ 2 * log(n)
		xt[6][i] = x[i][6] = x[i][2] * x[i][4]; // n ^ 3
		xt[7][i] = x[i][7] = powf(2.0F, n);     // 2 ^ n
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			xt_x[i][j] = 0.0F;
			for (int k = 0; k < N; k++)
				xt_x[i][j] += xt[i][k] * x[k][j];
		}
		xt_y[i] = 0.0F;
		for (int j = 0; j < N; j++)
			xt_y[i] += xt[i][j] * y[j];
	}
	float det_xt_x = determinant(8, xt_x);
	for (int i = 0; i < 8; i++) {
		b[i] = 0.0F;
		for (int j = 0; j < 8; j++)
			b[i] += ((i + j) % 2 ? -1 : 1) * minor(8, xt_x, j, i)
					* xt_y[j] / det_xt_x;
	}

	printf("%s\n", answer[0]);

	return 0;
}

// Laplace expansion
float determinant(int n, float matrix[][8])
{
	float d = 0.0F;

	for (int j = 0; j < n; j++)
		d += (j % 2 ? -1 : 1) * matrix[0][j] * minor(n, matrix, 0, j);

	return d;
}

float minor(int n, float matrix[][8], int i, int j)
{
	float m[8][8];

	if (n > 1) {
		for (int k = 0, km = 0; k < n; k++) {
			if (k == i)
				continue;
			for (int l = 0, lm = 0; l < n; l++) {
				if (l == j)
					continue;
				m[km][lm] = matrix[k][l];
				lm++;
			}
			km++;
		}

		return determinant(n - 1, m);
	}

	return 1.0F;
}
