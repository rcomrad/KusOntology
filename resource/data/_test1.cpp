#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <set>
using namespace std;

int
main()
{
    int n;
    cin >> n;
    vector<list<queue<int>>> gr(n);
    gr[0].back().push(4);

    set<int> ss;
    ss.clear();
}
