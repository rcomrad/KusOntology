#include <iostream>
#include <vector>
using namespace std;

int
main()
{
    int n, mx = -1e9;
    cin >> n;

    for (int i = 0; i < n; i++)
    {
        int num;
        cin >> num;
        if (num > mx) mx = num;
    }

    cout << mx;

    return 0;
}
