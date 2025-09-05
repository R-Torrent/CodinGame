#include <iostream>
#include <cstdlib>
#include <string>

/*
 * Darts
 * Puzzles > Classic Puzzle > Easy
 */

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

struct Player {
    string name;
    int score;

    Player(): Player("") { }
    Player(const string& name): name(name), score(0) { }

    bool operator<(const Player& p) const
    {
        return this->score < p.score;
    }

    void addScore(int size, int x, int y)
    {
        if (2 * (abs(x) + abs(y)) <= size)
            score +=5;
        if (4 * (x * x + y * y) <= size * size)
            score +=5;
        if (2 * abs(x) <= size && 2 * abs(y) <= size)
            score +=5;
    }
};

Player* findPlayer(Player *players, int n, const string& name)
{
    for ( ; n--; players++)
        if (players->name == name)
            return players;

    return NULL;
}

int main()
{
    int size;
    cin >> size; cin.ignore();
    int n;
    cin >> n; cin.ignore();
    Player players[n];
    for (int i = 0; i < n; i++)
        getline(cin, players[i].name);
    int t;
    cin >> t; cin.ignore();
    for (int i = 0; i < t; i++) {
        string throw_name;
        int throw_x;
        int throw_y;
        cin >> throw_name >> throw_x >> throw_y; cin.ignore();
        findPlayer(players, n, throw_name)->addScore(size, throw_x, throw_y);
    }

// Bubble sort: https://en.wikipedia.org/wiki/Bubble_sort
    int n1 = n;
    while (n1 > 1) {
        int newn = 0;
        for (int i = 1; i < n1; i++)
            if (players[i - 1] < players[i]) {
                Player temp = players[i - 1];
                players[i - 1] = players[i];
                players[i] = temp;
                newn = i;
            }
        n1 = newn;
    }

    // Write an answer using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    for (int i = 0; i < n; i++)
        cout << players[i].name << " " << players[i].score << endl;
}
