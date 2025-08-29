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

struct Candidate {
	pair<int, int> from;
	pair<int, int> to;

	Candidate(int a, int b, int c, int d): from(pair(a, b)), to(pair(b, c)) { }
};

int width;
int height;

bool checkLeft(char golfCourse[][1001], int row, int col, int col1)
{
	if (col1 < 0)
		return false;

	char ch;
	for (int c = col; c >= col1; c--)
        if ((ch = golfCourse[row][c]) != '.' && (ch != 'X' || c == col1))
			return false;
	return true;
}

bool checkRight(char golfCourse[][1001], int row, int col, int col1)
{
	if (col1 >= width)
		return false;

	char ch;
	for (int c = col; c <= col1; c++)
        if ((ch = golfCourse[row][c]) != '.' && (ch != 'X' || c == col1))
			return false;
	return true;
}

bool checkUp(char golfCourse[][1001], int row, int col, int row1)
{
	if (row1 < 0)
		return false;

	char ch;
	for (int r = row; r >= row1; r--)
        if ((ch = golfCourse[r][col]) != '.' && (ch != 'X' || r == row1))
			return false;
	return true;
}

bool checkDown(char golfCourse[][1001], int row, int col, int row1)
{
	if (row1 >= height)
		return false;

	char ch;
	for (int r = row; r <= row1; r++)
        if ((ch = golfCourse[r][col]) != '.' && (ch != 'X' || r == row1))
			return false;
	return true;
}

bool viableCandidate(char golfCourse[][1001], Candidate *c)
{
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			char count = golfCourse[i][j];
			if (!isdigit(count))
				continue;
			count -= '0';
			if (checkLeft(golfCourse, i, j, count))
				*c = Candidate(i, j, i - count, j);
			else if (checkRight(golfCourse, i, j, count))
				*c = Candidate(i, j, i + count, j);
			else if (checkUp(golfCourse, i, j, count))
				*c = Candidate(i, j, i, j - count);
			else if (checkDown(golfCourse, i, j, count))
				*c = Candidate(i, j, i, j + count);
			else
				continue;
			return true;
		}

	return false;
}

int main()
{
	char golfCourse[1000][1001];

	cin >> width >> height; cin.ignore();
	for (int i = 0; i < height; i++) {
		cin >> golfCourse[i]; cin.ignore();
	}
}
