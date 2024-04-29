#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Paper Labyrinth
 * Puzzles > Classic Puzzle > Medium
 */

#define Node(X, Y) ((X) + w * (Y))

int main()
{
	int xs, ys;
	scanf("%d%d", &xs, &ys);
	int xr, yr;
	scanf("%d%d", &xr, &yr);
	int w, h;
	scanf("%d%d", &w, &h);
	const int N = h * w;
	char (*maze)[h] = malloc(N);
	char (*dist)[h * w] = malloc(N * N);
	for (int y = 0; y < h; y++) {
		char l[w + 1];
		scanf("%s", l);
		for (int x = 0; x < w; x++)
			maze[x][y] = isdigit(l[x]) ? l[x] - '0' : l[x] - 'a' + 10;
	}

	// Floyd-Warshall algorithm
	// [https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm]
	memset(dist, CHAR_MAX, N * N);
	for (int y = 0; y < h; y++)
	for (int x = 0; x < w; x++) {
		dist[Node(x, y)][Node(x, y)] = 0;
		if (!(maze[x][y] & 1) && y < h - 1)
			dist[Node(x, y)][Node(x, y + 1)] = 1;
		if (!(maze[x][y] & 2) && x > 0)
			dist[Node(x, y)][Node(x - 1, y)] = 1;
		if (!(maze[x][y] & 4) && y > 0)
			dist[Node(x, y)][Node(x, y - 1)] = 1;
		if (!(maze[x][y] & 8) && x < w - 1)
			dist[Node(x, y)][Node(x + 1, y)] = 1;
	}
	for (int yk = 0; yk < h; yk++)
	for (int xk = 0; xk < w; xk++)
		for (int yi = 0; yi < h; yi++)
		for (int xi = 0; xi < w; xi++)
			for (int yj = 0; yj < h; yj++)
			for (int xj = 0; xj < w; xj++) {
				int d = dist[Node(xi, yi)][Node(xk, yk)]
					+ dist[Node(xk, yk)][Node(xj, yj)];
				if (dist[Node(xi, yi)][Node(xj, yj)] > d)
					dist[Node(xi, yi)][Node(xj, yj)] = d;
			}

	printf("%hhd %hhd\n",
		dist[Node(xs, ys)][Node(xr, yr)], dist[Node(xr, yr)][Node (xs, ys)]);

	free(maze);
	free(dist);

	return 0;
}
