#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

/*
 * Network Cabling
 * Puzzles > Classic Puzzle > Medium
 */

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

struct Building {

    int x;
    int y;

};

bool comp(Building& i, Building& j) { return i.y < j.y; }

int main()
{
    int n, xMin = numeric_limits<int>::max(), xMax = numeric_limits<int>::min();
    cin >> n; cin.ignore();
    vector<Building> network;
    for (int i = 0; i < n; i++) {
        Building b;
        cin >> b.x >> b.y; cin.ignore();
        network.push_back(b);
        if (b.x < xMin)
            xMin = b.x;
        if (b.x > xMax)
            xMax = b.x;
    }

    // median clculation
    sort(network.begin(), network.end(), comp);
    int yMain = network[n / 2].y;

    long cable = xMax - xMin;
    for (Building b : network)
        cable += abs(b.y - yMain);

    // Write an answer using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << cable << endl;
}
