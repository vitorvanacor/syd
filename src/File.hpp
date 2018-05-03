#ifndef FILE_H
#define FILE_H

#include <string>

using namespace std;

class File
{
public:
    File(string name);
    ~File();

    char* FileToByteArray();

    string name;

};

#endif
