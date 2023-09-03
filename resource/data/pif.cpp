#include <cmath>
#include <iostream>

using namespace std;
int
main()
{
    double a, b, c;
    cin >> a >> b >> c;
    double a1, b1, c1;
    if (a > b && a > c)
    {
        c1 = a;
        b1 = b;
        a1 = c;
    }
    else if (b > a && b > c)
    {
        c1 = b;
        b1 = c;
        a1 = b;
    }
    else
    {
        c1 = c;
        b1 = b;
        a1 = a;
    }

    if (c1 == a1 + b1 && a > 0 && b > 0 && c > 0)
    {
        cout << "YES";
    }
    else
    {
        cout << "NO";
    }
}
