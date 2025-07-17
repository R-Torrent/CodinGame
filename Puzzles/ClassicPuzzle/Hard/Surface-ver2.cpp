// Second attempt on this hard problem: 100% score

#include <iostream>
#include <queue>
#include <string>

/*
 * Surface
 * Puzzles > Classic Puzzle > Hard
 */

using namespace std;

int l;
int h;

struct Square {

    Square *parent = this;
    int size = 1;
    int x, y;

    Square(int x, int y): x(x), y(y) { }

    bool operator==(const Square& s) const {
        return this->x == s.x && this->y == s.y;
    }

    bool operator!=(const Square& s) const {
        return this->x != s.x || this->y != s.y;
    }

    void swap(Square &s) {
        const int tempSize = this->size;
        this->size = s.size;
        s.size = tempSize;
        Square *const tempParent = this->parent;
        this->parent = s.parent;
        s.parent = tempParent;
    }

};

// https://en.wikipedia.org/wiki/Disjoint-set_data_structure
Square* findParent(Square *x) {
    if (x->parent != x)
        return (x->parent = findParent(x->parent));
    else
        return x;
}

void unionBySize(Square* x, Square* y)
{
    x = findParent(x);
    y = findParent(y);

    if (x == y)
        return;

    if (x->size < y->size)
        x->swap(*y);

    y->parent = x;
    x->size += y->size;
}

#define PUSH(A, B) if (map[(A)][(B)] && findParent(map[(A)][(B)]) != findParent(&s)) { \
    q.push(*map[(A)][(B)]);                                                            \
    unionBySize(&s, map[(A)][(B)]);                                                    \
}

// [https://en.wikipedia.org/wiki/Flood_fill#Moving_the_recursion_into_a_data_structure]
void floodFill(Square *(*const map)[1000], Square *const square)
{
    queue<Square> q;
    q.push(*square);
    while (!q.empty()) {
        Square s = q.front();
        q.pop();
        if (s.x > 0)     { PUSH(s.x - 1, s.y) } // push west
        if (s.x < l - 1) { PUSH(s.x + 1, s.y) } // push east
        if (s.y > 0)     { PUSH(s.x, s.y - 1) } // push north
        if (s.y < h - 1) { PUSH(s.x, s.y + 1) } // push south
    }
}

int checkLakes(Square *(*const map)[1000], Square* s)
{
    if (!s)
        return 0;
    Square *p = findParent(s);
    if (p->size == 1)
        floodFill(map, p);
    return p->size;
}

int main()
{
    Square *map[1000][1000];
    cin >> l; cin.ignore();
    cin >> h; cin.ignore();
    for (int y = 0; y < h; y++) {
        string row;
        getline(cin, row);
        int x = 0;
        for (char &c : row) {
            map[x][y] = (c == 'O') ? new Square(x, y) : NULL;
            x++;
        }
    }
    int n;
    cin >> n; cin.ignore();
    for (int i = 0; i < n; i++) {
        int x;
        int y;
        cin >> x >> y; cin.ignore();

        cout << checkLakes(map, map[x][y]) << endl;
    }
    for (int x = 0; x < l; x++)
        for (int y = 0; y < h; y++)
            if (map[x][y])
                delete map[x][y];
}
