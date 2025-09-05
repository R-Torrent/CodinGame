#include <iostream>
#include <string>

/*
 * KGood
 * Puzzles > Classic Puzzle > Hard
 */

using namespace std;

int kgood(int letters[])
{
    int k = 0;
    for (size_t i = 0; i < 26; i++)
        k += letters[i] != 0;
    return k;
}

int main()
{
    string s;
    getline(cin, s);
    int k;
    cin >> k; cin.ignore();

    string::size_type answer = 0;
    int letters[26] = { 0 };
    string::size_type length = 0;

    while (s.size() > answer) {
        for (string::size_type i = length; i < s.size(); i++) {
            const char c = s[i];
            letters[c - 'a']++;
            if (kgood(letters) > k) {
                letters[c - 'a']--;
                break;
            }
            length++;
        }
        if (length > answer)
            answer = length;
        letters[s.front() - 'a']--;
        s = s.substr(1, string::npos);
        length--;
    }

    cout << answer << endl;
}
