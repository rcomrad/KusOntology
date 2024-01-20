#include <iostream>
#include <vector>
#include <list>
#include <queue>
using namespace std;

int
main()
{
    int n;
    cin >> n;
    list<vector<queue<int>>> gr(n);
    gr[0].push(4);
}
