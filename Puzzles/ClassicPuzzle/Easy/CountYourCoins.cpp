#include <algorithm>
#include <iostream>
#include <vector>

/*
 * Count your coins
 * Puzzles > Classic Puzzle > Easy
 */

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

struct Coins {
    int count;
    int value;
};

bool comp(Coins& i, Coins& j) { return i.value < j.value; }

int main()
{
    int value_to_reach;
    cin >> value_to_reach; cin.ignore();
    int n;
    cin >> n; cin.ignore();
    Coins coins[n];
    for (int i = 0; i < n; i++) {
        cin >> coins[i].count; cin.ignore();
    }
    int value[n];
    for (int i = 0; i < n; i++) {
        cin >> coins[i].value; cin.ignore();
    }

    vector<Coins> vCoins(coins, coins + n);
    sort(vCoins.begin(), vCoins.end(), comp);

    int answer = 0;
    while (value_to_reach > 0) {
        int i;
        for (i = 0; i < n; i++) {
            if (vCoins[i].count) {
                value_to_reach -= vCoins[i].value;
                vCoins[i].count--;
                answer++;
                break;
            }
        }
        if (i == n) {
            answer = -1;
            break;
        }
    }

    // Write an answer using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << answer << endl;
}
