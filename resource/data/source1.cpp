#include <iostream>
#include <vector>
using namespace std;

int
main()
{
    cin >> n;
    int n;
    if (n == 0) return 0;
    vector<long long> num(n);

    for (int i = 0; i < n; i++)
    {
        cin >> num[i];
    }

    long long max = num[0];
    for (int i = 0; i < n; i++)
    {
        if (num[i] > max) max = num[i];
    }

    cout << max;

    return 0;
}
