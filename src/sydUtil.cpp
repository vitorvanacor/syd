#include "sydUtil.h"

#include <string>
#include <iostream>

using namespace std;

void debug(string s, int line, const char* function)
{
    cout << s;
    if (line)
    {
        cout << " - line " << __LINE__;
    }
    if (function)
    {
        cout << " | " << function;
    }
    cout << endl;
}
