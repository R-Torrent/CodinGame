#include <iostream>
#include <string>
#include <vector>

/*
 * Scrabble
 * Puzzles > Classic Puzzle > Medium
 */

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
    int n;
    cin >> n; cin.ignore();
    vector<string> dictionary;
    for (int i = 0; i < n; i++) {
        string w;
        getline(cin, w);
        dictionary.push_back(w);
    }
    string letters;
    getline(cin, letters);

    string answer;
    int maxPoints = 0;
    for (string word : dictionary) {
        string letters1(letters);
        string::iterator it;
        size_t found;
        int points = 0;
        for (it = word.begin(); it != word.end() && (found = letters1.find_first_of(*it)) != string::npos; it++) {
            switch (letters1[found]) {
            case 'e': case 'a': case 'i': case 'o': case 'n':
            case 'r': case 't': case 'l': case 's': case 'u':
                points++;
                break;
            case 'd': case 'g':
                points += 2;
                break;
            case 'b': case 'c': case 'm': case 'p':
                points += 3;
                break;
            case 'f': case 'h': case 'v': case 'w': case 'y':
                points += 4;
                break;
            case 'k':
                points += 5;
                break;
            case 'j': case 'x':
                points += 8;
                break;
            case 'q': case 'z':
                points += 10;
            default:
                ;
            }
            letters1.erase(letters1.begin() + found);
        }
        if (it == word.end() && points > maxPoints) {
            answer = word;
            maxPoints = points;
        }
    }

    // Write an answer using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << answer << endl;
}
