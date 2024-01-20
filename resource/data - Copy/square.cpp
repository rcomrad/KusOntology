    #include <iostream>
    #include <cmath>
    using namespace std;
     
    int main()
    {
        float a, b, c, D;
        cin >> a;
        cin >> b;
        cin >> c;
        
        D = b * b - 4 * a * c;
        if ( D < 0) {
            cout << "no solutions";
            //return;
        }
        else if ( D == 0) {
            cout << "one solution" << "\n";
            cout << fixed << (-1 * b ) / (2 * a );
        }
        else {
            cout << "two solutions" << "\n";
            cout << fixed << (-1 * b + D ) / (2 * a ) << "\n";
            cout << fixed << (-1 * b - D ) / (2 * a );
        }
     
        return 0;
    }