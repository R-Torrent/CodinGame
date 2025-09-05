#include <stdlib.h>
#include <stdio.h>

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
 * fract_digits will store the number of fractional digits
 * rep_digits will store the number of repeating digits
 */
	fract_t *fract0 = malloc((N - 1) * sizeof(fract_t)), *fract;
	int fract_digits, rep_digits, i;
	div_t op;
	for(op = div(1, N), fract = fract0, fract_digits = 0, rep_digits = 0;
			op.rem && !rep_digits; fract++, fract_digits++) {
		fract->rem = op.rem;
		op = div(10 * fract->rem, N);
		fract->dig = op.quot + '0';

		i = 0;
		do // check for remainder repetition
			if ((fract - i)->rem == op.rem)
				rep_digits = i + 1;
		while (!rep_digits && i++ <= fract_digits);
	}

	printf("0.");
	for (i = 0, fract = fract0; i < fract_digits; i++, fract++) {
		if (rep_digits && (i == fract_digits - rep_digits))
			putchar('(');
		putchar(fract->dig);
	}
	if (rep_digits)
		putchar(')');

	free(fract0);
	return 0;
}
