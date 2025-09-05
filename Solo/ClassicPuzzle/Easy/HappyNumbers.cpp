#include <algorithm>
#include <iostream>
#include <string>

/*
 * Happy Numbers
 * Puzzles > Classic Puzzle > Easy
 */

#define HAPPY ":)"
#define UNHAPPY ":("

using namespace std;

class BigInt {
    string digits;

    string::size_type length() const {
        return digits.size();
    }

public:
    BigInt(const string& str): digits(str) {
        reverse(digits.begin(), digits.end());
        for (string::iterator i = digits.begin(); i < digits.end(); i++)
            *i -= '0';
    }

    BigInt(unsigned long nl = 0) {
        do {
            digits.push_back(nl % 10);
            nl /= 10;
        } while (nl);
    }

    BigInt& operator+=(const BigInt& bi) {
        string::size_type m = length(), n = bi.length();
        int carry = 0, sum;

        if (n > m) {
            digits.append(n - m, 0);
            m = length();
        }
        for (string::size_type i = 0; i < m; i++) {
            sum = digits[i] + (i < n ? bi.digits[i] : 0) + carry;
            digits[i] = sum % 10;
            carry = sum / 10;
        }
        if (carry)
            digits.push_back(carry);

        return *this;
    }

    bool operator==(const BigInt& bi) const {
        return this->digits == bi.digits;
    }

    // https://en.wikipedia.org/wiki/Perfect_digital_invariant
    void perfectDigitalInvariant() {
        BigInt y;

        for (char c : digits)
            y += c * c;
        digits = y.digits;
    }

    friend ostream& operator<<(ostream& os,const BigInt& bi);
};

ostream& operator<<(ostream& os,const BigInt &bi)
{
    string str(bi.digits);
    reverse(str.begin(), str.end());
    for (string::iterator i = str.begin(); i < str.end(); i++)
        *i += '0';

    return os << str;
}

const BigInt bi1(1L), bi4(4L);

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int n;
    cin >> n; cin.ignore();
    for (int i = 0; i < n; i++) {
        bool happy;
        string x;
        getline(cin, x);
        BigInt bix(x);
        // https://en.wikipedia.org/wiki/Happy_number#10-happy_numbers
        while (!(happy = bix == bi1) && bix != bi4)
            bix.perfectDigitalInvariant();

        // Write an answer using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;
        cout << x << ' ' << (happy ? HAPPY : UNHAPPY) << endl;
    }  
}
