#include <iostream>
#include "LaserScannerDriver.h"

using namespace std;

int main()
{
    //trying to round down numbers
    /*
    double res = 0.35788399;
    const int angle = 180;
    int cells = angle / res;
    cout << cells << endl;
    return 0;
    */
    LaserScannerDriver prova(1.0);
    for (int i = 0; i < 5; i++) {
        vector<double> vec;
        for (int j = 0; j < 180; j++) {
            vec.push_back(j);
        }
        prova.new_scan(vec);
    }
    
    cout << prova;
    prova.clear_buffer();
}
