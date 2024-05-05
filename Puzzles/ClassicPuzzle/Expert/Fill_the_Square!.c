#include <stdio.h>

/*
 * Fill the Square!
 * Puzzles > Classic Puzzle > Expert
 */

#define MAP(I, J) ((I) * N + (J))

int minor(int [][225], int, int, int);

int det(int A[][225], int N)
{
	int d = 0;
	for (int j = 0; j < N; j++)
		d += A[0][j] * minor(A, 0, j, N);

	return d;
}

int minor(int A[][225], int i_skip, int j_skip, int N)
{
	int Min[225][225];
	for (int i = 0, i1 = 0; i < N; i++) {
		if (i == i_skip)
			continue;
		for (int j = 0, j1 = 0; j < N; j++) {
			if (j == j_skip)
				continue;
			Min[i1][j1] = A[i][j];
			j1++;
		}
		i1++;
	}

	return ((i_skip + j_skip) % 2 ? -1 : 1) * (--N == 1 ? Min[0][0] : det(Min, N));
}

int main()
{
	int N;
	scanf("%d", &N); fgetc(stdin);

// Problem: {A X + B}(mod 2) = C(mod 2)
// --> X = A^(-1) {C - B}

	int A[225][225], C_B[225];
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
		}
	}

	int X[225], Adj[225][225], N2 = N * N;
	// Adj: Adjunct matrix
	for (int i = 0; i < N2; i++)
		for (int j = 0; j < N2; j++)
			Adj[j][i] = minor(A, i, j, N2);
	for (int i = 0; i < N2; i++) {
		X[i] = 0;
		for (int j = 0; j < N2; j++)
			X[i] += Adj[i][j] * C_B[j];
	}
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			putchar(X[MAP(i, j)] % 2 ? 'X' : '.');
		putchar('\n');
	}

	return 0;
}
