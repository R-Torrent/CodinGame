#include <stdlib.h>
#include <stdio.h>

// Greedy algorithm solution:
// https://en.wikipedia.org/wiki/Activity_selection_problem

struct calculation {
	int starting_day;
	int finishing_day;
};

int calc_compare(const struct calculation *c1, const struct calculation *c2)
{
	return c1->finishing_day - c2->finishing_day;
}

int main()
{
	struct calculation requests[100000], *request;
	int N, D, max, f;
	scanf("%d", &N);
	for (request = requests; request - requests < N; request++) {
		scanf("%d%d", &request->starting_day, &D);
		request->finishing_day = request->starting_day + D;
	}

	qsort(requests, N, sizeof(struct calculation),
			(int (*)(const void *, const void *))calc_compare);

	max = 1;
	f = requests[0].finishing_day;
	for (request = requests + 1; request - requests < N; request++)
		if (request->starting_day >= f) {
			f = request->finishing_day;
			max++;
		}

	printf("%d\n", max);

	return 0;
}
