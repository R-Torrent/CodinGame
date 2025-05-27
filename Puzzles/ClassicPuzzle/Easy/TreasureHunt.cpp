#include <iostream>
#include <string>

/*
 * Treasure hunt
 * Puzzles > Classic Puzzle > Easy
 */

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int h;
int w;

struct Cell {

	int r;
	int c;
	int gold;
	bool visited;

	Cell(int r, int c): r(r), c(c), gold(0), visited(false) { }

};

struct CellMap {

	Cell map[][w];

	CellMap() { map = new Cell[h * w]; }

	CellMap(const CellMap& other): CellMap() {
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				
			
	}

}


int check_neigbors(Cell (*const map)[w], Cell& current)
{
	current.visited = true;
	int local_gold = current.gold;

	if (current.r > 1 && !map[r - 1][current.c].visited) {
		Cell (*const map)[w] = new Cell[
		check_neighbors(map, map[r - 1][current.c]
}

int main()
{
	cin >> h >> w; cin.ignore();
	Cell (*const map)[w] = new Cell[h * w];
	Cell *current;

	for (int i = 0; i < h; i++) {
		string row;

		getline(cin, row);
		for (Cell *cell = map[i]; cell - map[i] < w; cell++) {
			*cell = Cell(i, j);
			char c;

			switch (c = row.at(j)) {
			case 'X': current = cell; // fall through
			case '#': cell->visited = true; break;
			case ' ': break;
			default: cell->gold = c - '0';
			}
		}
	}

	int amount = 0;

	check_neigbors(map, *current);


// Write an answer using cout. DON'T FORGET THE "<< endl"
// To debug: cerr << "Debug messages..." << endl;

	cout << "answer" << endl;

	delete[] map;
}
