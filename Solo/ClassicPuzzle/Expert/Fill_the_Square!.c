#include <stdio.h>
#include <string.h>

/*
 * Fill the Square!
 * Puzzles > Classic Puzzle > Expert
 */

#define MAP(I, J) ((I) * N + (J))

int main()
{
	int N;
	scanf("%d", &N); fgetc(stdin);

// Problem: {A X + B}(mod 2) = C(mod 2)
// --> X = A^(-1) {C - B}

	int A[225][450], C_B[225];
	memset(A, 0, 101250 * sizeof(int));
	for (int i = 0; i < N; i++) {
		char ROW[16];
		scanf("%[^\n]", ROW); fgetc(stdin);
		for (int j = 0; j < N; j++) {
			C_B[MAP(i, j)] = ROW[j] == '*' ? 0 : 1;
			A[MAP(i, j)][MAP(i, j)] = 1;
			if (i > 0)
				A[MAP(i, j)][MAP(i - 1, j)] = 1;
			if (j > 0)
				A[MAP(i, j)][MAP(i, j - 1)] = 1;
			if (i < N - 1)
				A[MAP(i, j)][MAP(i + 1, j)] = 1;
			if (j < N - 1)
				A[MAP(i, j)][MAP(i, j + 1)] = 1;
			A[MAP(i, j)][225 + MAP(i, j)] = 1;
		}
	}

// A^(-1) by Gaussian elimination
	int X[225], N2 = N * N;
	for (int i1 = 0; i1 < N2; i1++) {
		if (!A[i1][i1]) {
			int i2;
			for (i2 = i1 + 1; !A[i2][i1] && i2 < N2; i2++)
				;
			for (int j = 0; j < N2; j++) {
				A[i1][j] ^= A[i2][j];
				A[i1][225 + j] ^= A[i2][225 + j];
			}
		}
		for (int i = 0; i < N2; i++)
			if (i != i1 && A[i][i1])
				for (int j = 0; j < N2; j++) {
					A[i][j] ^= A[i1][j];
					A[i][225 + j] ^= A[i1][225 + j];
				}
	}
	for (int i = 0; i < N2; i++) {
		X[i] = 0;
		for (int j = 0; j < N2; j++)
			X[i] ^= A[i][225 + j] * C_B[j];
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			putchar(X[MAP(i, j)] ? 'X' : '.');
		putchar('\n');
	}

	return 0;
}
