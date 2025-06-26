// First attempt on this hard problem: 80% score
// It timed out in the last test case

#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

/*
 * Surface
 * Puzzles > Classic Puzzle > Hard
 */

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int l;
int h;

struct PairHash {

    size_t operator()(const pair<int, int> &p) const
    {
        return p.first ^ p.second;
    }

};

struct Lake {

    int surface = 0;
    unordered_set<pair<int, int>, PairHash> coordinates;

};

#define PUSH if (map[x][y]) q.push(pair(x, y))

// [https://en.wikipedia.org/wiki/Flood_fill#Moving_the_recursion_into_a_data_structure]
void floodFill(Lake& lake, const bool (*const map)[10000],
        const pair<int, int> &coordinates)
{
    queue<pair<int, int>> q;
    q.push(coordinates);
    while (!q.empty()) {
        pair<int, int> n = q.front();
        q.pop();
        if (map[n.first][n.second] && !lake.coordinates.contains(n)) {
            lake.surface++;
            lake.coordinates.insert(n);
            int x, y;
            if (n.first > 0) {     // push west
                x = n.first - 1, y = n.second;
                PUSH;
            }
            if (n.first < l - 1) {  // push east
                x = n.first + 1, y = n.second;
                PUSH;
            }
            if (n.second > 0) {     // push north
                x = n.first, y =n.second - 1;
                PUSH;
            }
            if (n.second < h - 1) { // push south
                x = n.first, y = n.second + 1;
                PUSH;
            }
        }
    }
}

int checkLakes(vector<Lake> &lakes, const bool (*const map)[10000],
        const pair<int, int> &coordinates)
{
    if (!map[coordinates.first][coordinates.second])
        return 0;
    for (Lake& l : lakes)
        if (l.coordinates.contains(coordinates))
            return l.surface;
    Lake newLake;
    lakes.push_back(newLake);
    floodFill(newLake, map, coordinates);
    return newLake.surface;
}

int main()
{
    bool (*const map)[10000] = reinterpret_cast<bool (*)[10000]>(new bool[100000000]);
    cin >> l; cin.ignore();
    cin >> h; cin.ignore();
    for (int y = 0; y < h; y++) {
        string row;
        getline(cin, row);
        int x = 0;
        for (char &c : row)
            map[x++][y] = c == 'O';
    }
    vector<Lake> lakes;
    int answers[1000];
    int n;
    cin >> n; cin.ignore();
    for (int i = 0; i < n; i++) {
        int x;
        int y;
        cin >> x >> y; cin.ignore();
        pair coordinate(x, y);

        // Write an answer using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;
        cout << checkLakes(lakes, map, coordinate) << endl;
    }
    delete[] map;
}
