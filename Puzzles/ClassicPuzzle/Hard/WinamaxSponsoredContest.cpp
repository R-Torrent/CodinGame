#include <cctype>
#include <iostream>

/*
 * Winamax Sponsored Contest
 * Puzzles > Classic Puzzle > Hard
 */

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int width;
int height;

void duplicateCourse(char (&copy)[1000][1001], const char (&original)[1000][1001])
{
	for (int i = 0; i < height; i++)
		for (int j = 0; j <= width; j++)
			copy[i][j] = original[i][j];
}

void backtrack(char (&)[1000][1001]);

void hitLeft(char (&golfCourse)[1000][1001], const int row, const int col, int count)
{
	const int col1 = col - count;
	if (col1 < 0)
		return;

	int c;
	for (c = col - 1; c >= col1; c--) {
        const char ch = golfCourse[row][c];
        if (!(ch == '.' || (ch == 'X' && c != col1) || (ch == 'H')))
			return;
	}
	char next[1000][1001];
	duplicateCourse(next, golfCourse);
	for (c = col; c > col1 && next[row][c] != 'H'; c--)
		next[row][c] = '<';
	if (next[row][c] == 'H')
		count = 1;
	next[row][c] = '0' + count - 1;
	backtrack(next);
}

void hitRight(char (&golfCourse)[1000][1001], const int row, const int col, int count)
{
	const int col1 = col + count;
	if (col1 >= width)
		return;

	int c;
	for (c = col + 1; c <= col1; c++) {
		const char ch = golfCourse[row][c];
        if (!(ch == '.' || (ch == 'X' && c != col1) || (ch == 'H')))
			return;
	}
	char next[1000][1001];
	duplicateCourse(next, golfCourse);
	for (c = col; c < col1 && next[row][c] != 'H'; c++)
		next[row][c] = '>';
	if (next[row][c] == 'H')
		count = 1;
	next[row][c] = '0' + count - 1;
	backtrack(next);
}

void hitUp(char (&golfCourse)[1000][1001], const int row, const int col, int count)
{
	const int row1 = row - count;
	if (row1 < 0)
		return;

	int r;
	for (r = row - 1; r >= row1; r--) {
    	const char ch = golfCourse[r][col];
    	if (!(ch == '.' || (ch == 'X' && r != row1) || (ch == 'H')))
			return;
	}
	char next[1000][1001];
	duplicateCourse(next, golfCourse);
	for (r = row; r > row1 && next[r][col] != 'H'; r--)
		next[r][col] = '^';
	if (next[r][col] == 'H')
		count = 1;
	next[r][col] = '0' + count - 1;
	backtrack(next);
}

void hitDown(char (&golfCourse)[1000][1001], const int row, const int col, int count)
{
	const int row1 = row + count;
	if (row1 >= height)
		return;

	int r;    
	for (r = row + 1; r <= row1; r++) {
		const char ch = golfCourse[r][col];
    	if (!(ch == '.' || (ch == 'X' && r != row1) || (ch == 'H')))
			return;
	}
	char next[1000][1001];
	duplicateCourse(next, golfCourse);
	for (r = row; r < row1 && next[r][col] != 'H'; r++)
		next[r][col] = 'v';
	if (next[r][col] == 'H')
		count = 1;
	next[r][col] = '0' + count - 1;
	backtrack(next);
}

// https://en.wikipedia.org/wiki/Backtracking
void backtrack(char (&candidate)[1000][1001])
{
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
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
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++)
			if (candidate[i][j] == '0')
				candidate[i][j] = '.';
		cout << candidate[i] << endl;
	}
}

int main()
{
	char golfCourse[1000][1001];

	cin >> width >> height; cin.ignore();
	for (int i = 0; i < height; i++) {
		cin >> golfCourse[i]; cin.ignore();
	}

	backtrack(golfCourse);
}
