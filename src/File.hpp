#ifndef FILE_H
#define FILE_H

#include <string>

using namespace std;

class File
{
public:
    File(string path);
    ~File();

    int GetLength();

    void FileToByteArray(char *buffer);

    int FileLength();

    string path;
    string name;
    string extension;
	string lastModified;
    int length;

};

#endif
