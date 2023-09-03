#include <iostream>
#include <vector>
using namespace std;

int
main()
{
    int n;
    cin >> n;

    if (n == 0) return 0;
    vector<long long> num;

    for (int i = 0; i < n; i++)
    {
        int temp;
        cin >> temp;
        num = temp;
    }

    int k = 0;
    for (int i = 0; i < n; i++)
    {
        if (num[i] > num[k]) k = i;
    }

    cout << num[k];

    return 0;
}
