#include <iostream>
#include <vector>
using namespace std;

int
main()
{
    long long n;
    cin >> n;

    if (n != 0)
    {
        vector<int> num(n);

        for (int i = 0; i < n; i++)
        {
            cin >> num[i];
        }

        int max = num[0];
        for (int i = 0; i < n; i++)
        {
            if (num[i] > max) max = num[i];
        }

        cout << max;
    }

    return 0;
}
