#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

#define MAXHEIGHT 20

struct Grid {
	int gameTurn;
	int bombs; // number of bombs left
	string mapRow[MAXHEIGHT];
	vector<reference_wrapper<Grid>> children;
	bool discovered; // see DFS below

	Grid(): gameTurn(0), discovered(false) { }

	void spawnChildren()
	{
		Grid nextTurn(*this);
		nextTurn.gameTurn--;
		queue<pair<int, int>> explosions;
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				if (mapRow[i][j] == 'P') // fork-bomb placed
					nextTurn.mapRow[i][j] = 'C';  // fork-bomb charging
				else if (mapRow[i][j] == 'C')
					explosions.push(pair(i, j));
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				if (nodesHit(i, j)) {
					Grid child(nextTurn);
					child.mapRow[i][j] = 'P';
					child.bombs--;
					children.push_back(*grids.find(child.add()));
				}
	}

	string serialize() const
	{
		string temp = "";
		for (const auto& s : mapRow) {
			int len = s.length();
			temp += to_string(len) + "~" + s;
		}

		return temp;
	}

	string add() const
	{
		const string str = serialize();
		grids[str] = *this;

		return str;
	}

	static int width; // width of the firewall grid
	static int height; // height of the firewall grid
	static map<string, Grid> grids; // collection of all grids
};

// https://en.wikipedia.org/wiki/Depth-first_search
// recursive implementation
void DFS(Grid& grid)
{
	grid.discovered = true;
	for (Grid& neighbor : grid.children)
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
	grid0.add();

	// game loop
	while (true) {

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;

		cout << "3 0" << endl;
	}
}
