#include <iostream>
#include <string>

/*
 * Mayan Calculation
 * Puzzles > Classic Puzzle > Medium
 */

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

struct Mayan {

    long value;
    string row[100];

    static Mayan numerals[];
    static int h, l;

    Mayan() { }

    Mayan(long n): value(n) { }

    void determineNumeral(void)
    {
        for (int i = 0, j; i < 20; i++) {
            for (j = 0; j < h && !this->row[j].compare(numerals[i].row[j]); j++)
                ;
            if (j == h) {
                value = i;
                return;
            }
        }
        value = -1;
    }

    static long readMayan(const int);

};

Mayan Mayan::numerals[20];
int Mayan::h = 100;
int Mayan::l = 100;

long Mayan::readMayan(const int lines)
{
    long n = 0;
    for (int i = 0; i < lines / h; i++) {
        Mayan m;
        for (int j = 0; j < h; j++)
            cin >> m.row[j]; cin.ignore();
        m.determineNumeral();
        n *= 20;
        n += m.value;
    }

    return n;
}

std::ostream& place_numeral(std::ostream& os, long n)
{
    long x = n / 20;
    if (x)
        place_numeral(os, x);
    for(int j = 0; j < Mayan::h; j++)
        os << Mayan::numerals[n % 20].row[j] << endl;

    return os;
}

std::ostream& operator<<(std::ostream& os, const Mayan& m)
{
    return place_numeral(os, m.value);
}

int main()
{
    cin >> Mayan::l >> Mayan::h; cin.ignore();

    for (int i = 0; i < 20; i++)
        Mayan::numerals[i].value = i;
    for (int j = 0; j < Mayan::h; j++) {
        string numeral;
        cin >> numeral; cin.ignore();
        for (int i = 0; i < 20; i++)
            Mayan::numerals[i].row[j] = numeral.substr(i * Mayan::l, Mayan::l);
    }

    int s;
    long n1;
    cin >> s; cin.ignore();
    n1 = Mayan::readMayan(s);
    long n2;
    cin >> s; cin.ignore();
    n2 = Mayan::readMayan(s);

    string operation;
    cin >> operation; cin.ignore();
    Mayan result;

    if (!operation.compare("+"))
        result = n1 + n2;
    else if (!operation.compare("-"))
        result = n1 - n2;
    else if (!operation.compare("*"))
        result = n1 * n2;
    else
        result = n1 / n2;

    // Write an answer using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << result;
}
