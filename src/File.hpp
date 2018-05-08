#ifndef FILE_H
#define FILE_H

#include <string>

using namespace std;

class File
{
  public:
    File(string path);
    ~File();

    string GetPath();

    string path;
    string name;
    string extension;
    string lastModified;
    int length;
};

#endif
