#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>

/*
 * Winamax Sponsored Contest
 * Puzzles > Classic Puzzle > Hard
 */

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

#define MAXHEIGHT 20

int width;
int height;

void printSolution(string (&solution)[MAXHEIGHT])
{
	for (int i = 0; i < height; i++) {
		replace(solution[i].begin(), solution[i].end(), '0', '.');
		replace(solution[i].begin(), solution[i].end(), 'X', '.');
		cout << solution[i] << endl;
	}

	exit(0);
}

void duplicateCourse(string (&copy)[MAXHEIGHT], const string (&original)[MAXHEIGHT])
{
	for (int i = 0; i < height; i++)
		copy[i] = original[i];
}

void backtrack(string (&)[MAXHEIGHT]);

void hitLeft(string (&golfCourse)[MAXHEIGHT], const int row, const int col,
		int count)
{
	const int col1 = col - count;
	if (col1 < 0)
		return;

	int c;
	for (c = col - 1; c >= col1; c--) {
        const char ch = golfCourse[row][c];
        if (!(ch == '.' || (ch == 'X' && c != col1) || (ch == 'H' && c == col1)))
			return;
	}
	string next[MAXHEIGHT];
	duplicateCourse(next, golfCourse);
	for (c = col; c > col1 && next[row][c] != 'H'; c--)
		next[row][c] = '<';
	if (next[row][c] == 'H')
		count = 1;
	next[row][c] = '0' + count - 1;
	backtrack(next);
}

void hitRight(string (&golfCourse)[MAXHEIGHT], const int row, const int col,
		int count)
{
	const int col1 = col + count;
	if (col1 >= width)
		return;

	int c;
	for (c = col + 1; c <= col1; c++) {
		const char ch = golfCourse[row][c];
        if (!(ch == '.' || (ch == 'X' && c != col1) || (ch == 'H' && c == col1)))
			return;
	}
	string next[MAXHEIGHT];
	duplicateCourse(next, golfCourse);
	for (c = col; c < col1 && next[row][c] != 'H'; c++)
		next[row][c] = '>';
	if (next[row][c] == 'H')
		count = 1;
	next[row][c] = '0' + count - 1;
	backtrack(next);
}

void hitUp(string (&golfCourse)[MAXHEIGHT], const int row, const int col,
		int count)
{
	const int row1 = row - count;
	if (row1 < 0)
		return;

	int r;
	for (r = row - 1; r >= row1; r--) {
    	const char ch = golfCourse[r][col];
    	if (!(ch == '.' || (ch == 'X' && r != row1) || (ch == 'H' && r == row1)))
			return;
	}
	string next[MAXHEIGHT];
	duplicateCourse(next, golfCourse);
	for (r = row; r > row1 && next[r][col] != 'H'; r--)
		next[r][col] = '^';
	if (next[r][col] == 'H')
		count = 1;
	next[r][col] = '0' + count - 1;
	backtrack(next);
}

void hitDown(string (&golfCourse)[MAXHEIGHT], const int row, const int col,
		int count)
{
	const int row1 = row + count;
	if (row1 >= height)
		return;

	int r;    
	for (r = row + 1; r <= row1; r++) {
		const char ch = golfCourse[r][col];
    	if (!(ch == '.' || (ch == 'X' && r != row1) || (ch == 'H' && r == row1)))
			return;
	}
	string next[MAXHEIGHT];
	duplicateCourse(next, golfCourse);
	for (r = row; r < row1 && next[r][col] != 'H'; r++)
		next[r][col] = 'v';
	if (next[r][col] == 'H')
		count = 1;
	next[r][col] = '0' + count - 1;
	backtrack(next);
}

// https://en.wikipedia.org/wiki/Backtracking
void backtrack(string (&candidate)[MAXHEIGHT])
{
	int i, j;
	int holesLeft = 0, ballsInPlay = 0;

	for (i = 0; i < height; i++) {
		holesLeft += count(candidate[i].begin(), candidate[i].end(), 'H');
		ballsInPlay += count_if(candidate[i].begin(), candidate[i].end(),
				[](char c) { return '1' <= c && c <= '9'; });
	}
	if (holesLeft != ballsInPlay) // reject
		return;
	if (!holesLeft) //accept
		printSolution(candidate);
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++) {
			char count = candidate[i][j];
			if (isdigit(count) && count != '0') {
				count -= '0';
				hitLeft(candidate, i, j, count);
				hitRight(candidate, i, j, count);
				hitUp(candidate, i, j, count);
				hitDown(candidate, i, j, count);
				return;
			}
		}
}

int main()
{
	string golfCourse[MAXHEIGHT];

	cin >> width >> height; cin.ignore();
	for (int i = 0; i < height; i++) {
		cin >> golfCourse[i]; cin.ignore();
	}

	backtrack(golfCourse);
}
