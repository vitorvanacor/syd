#include "sydUtil.h"

#include <string>
#include <iostream>

using namespace std;

void debug(string msg, const char* file, int line)
{
    if (file)
    {
        cout << file;
    }
    if (line)
    {
        cout << " (line " << line << ")";
    }
    cout << ": " << msg << endl;
}
