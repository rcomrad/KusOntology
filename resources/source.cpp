#include <iostream>
#include <vector>
using namespace std;

int
main()
{
    int min, max;
    cin >> min;
    cin >> max;

    for (int i = min; i < max + 1; i++)
    {
        if (i % 2 == 0) cout << i << " ";
    }

    return 0;
}
