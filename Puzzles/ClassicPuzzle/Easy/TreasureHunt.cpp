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

#define INDEX(A, B) ( (A) * w + (B) )

struct Cell {

	int r;
	int c;
	int gold;
	bool visited;

	Cell(int r = 0, int c = 0): r(r), c(c), gold(0), visited(false) { }

};

struct CellMap {

	Cell *map;

	CellMap() { map = new Cell[h * w]; }

	CellMap(const CellMap& other): CellMap() {
		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
				map[INDEX(i, j)] = other.map[INDEX(i, j)];
	}

	~CellMap() { delete[] map; }

};

int checkNeighbors(CellMap& cellMap, Cell& current)
{
	current.visited = true;

	int up, dw, lf, rt;
	up = dw = lf = rt = 0;

	if (current.r > 0 && !cellMap.map[INDEX(current.r - 1, current.c)].visited) {
		CellMap next(cellMap);
		up = checkNeighbors(next, next.map[INDEX(current.r - 1, current.c)]);
	}
	if (current.r < h - 1 && !cellMap.map[INDEX(current.r + 1, current.c)].visited) {
		CellMap next(cellMap);
		dw = checkNeighbors(next, next.map[INDEX(current.r + 1, current.c)]);
	}
	if (current.c > 0 && !cellMap.map[INDEX(current.r, current.c - 1)].visited) {
		CellMap next(cellMap);
		lf = checkNeighbors(next, next.map[INDEX(current.r, current.c - 1)]);
	}
	if (current.c < w - 1 && !cellMap.map[INDEX(current.r, current.c + 1)].visited) {
		CellMap next(cellMap);
		rt = checkNeighbors(next, next.map[INDEX(current.r, current.c + 1)]);
	}

	if (up >= dw && up >= lf && up >= rt)
		return current.gold + up;
	if (dw >= up && dw >= lf && dw >= rt)
		return current.gold + dw;
	if (lf >= up && lf >= dw && lf >= rt)
		return current.gold + lf;
	return current.gold + rt;
}

int main()
{
	cin >> h >> w; cin.ignore();

	CellMap cellMap;
	Cell *current;

	for (int i = 0; i < h; i++) {
		string row;

		getline(cin, row);
		for (int j = 0; j < w; j++) {
			Cell *cell = &cellMap.map[INDEX(i, j)];
            *cell = Cell(i, j);
			char c;

			switch (c = row.at(j)) {
			case 'X': current = cell; // fall through
			case ' ': break;
            case '#': cell->visited = true; break;
			default: cell->gold = c - '0';
			}
		}
	}

// Write an answer using cout. DON'T FORGET THE "<< endl"
// To debug: cerr << "Debug messages..." << endl;

	cout << checkNeighbors(cellMap, *current)  << endl;
}
