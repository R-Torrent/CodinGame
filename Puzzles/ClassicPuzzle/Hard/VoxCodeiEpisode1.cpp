#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

#define MAXHEIGHT 20

struct Grid {
	int gameTurn; // number of bombs left
	int bombs;
	string mapRow[MAXHEIGHT];
	vector<reference_wrapper<Grid>> neighbors;
	bool discovered; // see DFS below

	Grid(): gameTurn(0), discovered(false) { }

	static int width; // width of the firewall grid
	static int height; // height of the firewall grid
};

// https://en.wikipedia.org/wiki/Depth-first_search
// recursive implementation
void DFS(Grid& grid)
{
	grid.discovered = true;
	for (Grid& neighbor : grid.neighbors)
		if (!neighbor.discovered)
			DFS(neighbor);
}

int main()
{  
	cin >> Grid::width >> Grid::height; cin.ignore();
	Grid grid0;
	for (int i = 0; i < Grid::height; i++) 
		getline(cin, grid0.mapRow[i]); // one line of the firewall grid

	int rounds; // number of rounds left before the end of the game
	cin >> rounds >> grid0.bombs; cin.ignore();

	// game loop
	while (true) {

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;

		cout << "3 0" << endl;
	}
}
