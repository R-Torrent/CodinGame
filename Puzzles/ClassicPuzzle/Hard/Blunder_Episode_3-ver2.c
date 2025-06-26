// Second attempt on this hard problem: 100% score

#include <math.h>
#include <stdio.h>

/*
 * Blunder - Episode 3
 * Puzzles > Classic Puzzle > Hard
 */

char *answer[] = { "O(1)", "O(log n)", "O(n)", "O(n log n)",
	"O(n^2)", "O(n^2 log n)", "O(n^3)", "O(2^n)" };

/*
 * Solved with linear regression, the parameters calculated through
 * ordinary least squares (OLS).
 *
 * Each of the eight models is plotted against the execution time
 * (in microseconds). For the correct model, the resulting graph
 * should be a straight line passing through the origin (except for
 * case O(1), whose points should ideally fall on the same spot).
 *
 * This program picks the model with the smallest sum of squared
 * residuals.
 *
 * https://en.wikipedia.org/wiki/Simple_linear_regression#Simple_linear_regression_without_the_intercept_term_(single_regressor)
 */

int main()
{
	int N;
	scanf("%d", &N);

	double y[999], x[999][8];
	int found_inf = 0;
	for (int i = 0; i < N; i++) {
		double n;
		scanf("%lf%lf", &n, &y[i]);
		x[i][0] = 1.0;               // 1
		x[i][1] = log(n);            // log(n)
		x[i][2] = n;                 // n
		x[i][3] = x[i][1] * x[i][2]; // n * log(n)
		x[i][4] = x[i][2] * x[i][2]; // n ^ 2
		x[i][5] = x[i][1] * x[i][4]; // n ^ 2 * log(n)
		x[i][6] = x[i][2] * x[i][4]; // n ^ 3
		if (!found_inf) {
			x[i][7] = pow(2.0, n);  // 2 ^ n
			if (isinf(x[i][7]) || n > 50.0)
				found_inf = 1;
		}
	}

	double sx2[8] = {0.0}, sxy[8] = {0.0};
	for (int j = 0; j < (found_inf ? 7 : 8); j++)
		for (int i = 0; i < N; i++) {
			sx2[j] += x[i][j] * x[i][j];
			sxy[j] += x[i][j] * y[i];
		}
	double q[8] = {0.0};
	for (int j = 0; j < (found_inf ? 7 : 8); j++) {
		double b = sxy[j] / sx2[j];
		for (int i = 0; i < N; i++) {
			double ei = y[i] - b * x[i][j];
			q[j] += ei * ei;
		}
	}
	int best_fit;
	double q_min = INFINITY;
	for (int j = 0; j < (found_inf ? 7 : 8); j++)
		if (q[j] < q_min) {
			best_fit = j;
			q_min = q[j];
		}

	printf("%s", answer[best_fit]);
	return 0;
}
