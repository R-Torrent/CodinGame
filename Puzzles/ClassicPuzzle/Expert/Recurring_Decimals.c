#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
 * Recurring Decimals
 * Puzzles > Classic Puzzle > Expert
 */

typedef struct {
	int rem;
	char dig;
} fract_t;

int main()
{
	int N;
	scanf("%d", &N);

/*
 * Fractional digits
 * Stored in fract_t array pointed by fract
 * Remainder in fract->rem
 * First digit corresponding to the remainder in fract->dig
 *
 * 10·rem[i] = dig[i]·divisor + rem[i+1]
 *
 * frcds will store the number of fractional digits
 * repds will store the number of repeating digits
 */
	fract_t *fract0 = malloc((N - 1) * sizeof(fract_t)), *fract;
	int frcds = 0, repds = 0;
	div_t op;
	for(op = div(1, N), fract = fract0; op.rem; fract++) {
		fract->rem = op.rem;
		op = div(10 * fract->rem, N);
	}

	free(fract0);
	return 0;
}
