#include <ctype>
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

bool Candidate *viableCandidate(const char GolfCourse[][1001], Candidate *c)
{
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			char ch = GolfCourse[i][j];
			if (!isdigit(ch))
				continue;
			ch -= '0';
			if (checkLeft(i, j, ch)
				*c = Candidate(i, j, i - ch, j);
			else if (checkRight(i, j, ch)
				*c = Candidate(i, j, i + ch, j);
			else if (checkUp(i, j, ch);
				*c = Candidate(i, j, i, j - ch);
			else if (checkDown(i, j, ch);
				*c = Candidate(i, j, i, j + ch);
			else
				continue;
			return true;
		}

	return false;
}

int main()
{
	char golfCourse[1001][1001];

	cin >> width >> height; cin.ignore();
	for (int i = 0; i < height; i++) {
		cin >> golfCourse[i]; cin.ignore();
	}
}
