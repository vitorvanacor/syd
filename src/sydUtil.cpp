#include "sydUtil.h"

#include <string>
#include <iostream>

using namespace std;

void debug(string msg, const char* file, int line)
{
    if (!DEBUG)
    {
        return;
    }
    if (file)
    {
        string filename = get_filename(string(file));
        cout << without_extension(filename);
    }
    cout << ": " << msg;
    if (line)
    {
        cout << " (line " << line << ")";
    }
    cout << endl;
}

string get_filename(string filepath)
{
    int last_slash_position = filepath.find_last_of('/');
    return filepath.substr(last_slash_position);
}

string without_extension(string filename)
{
    int dot_position = filename.find_last_of('.');
    return filename.substr(0, dot_position);
}
