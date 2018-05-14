#ifndef FILE_H
#define FILE_H

#include "sydUtil.h"

class File
{
  public:
    File(string path);
    ~File();

    static list<string> list_filename(string dirpath);
    static void create_directory(string path);
    unsigned int get_filesize(string filename);
    static string list_directory(string dirpath);

    string GetPath();

    string path;
    string name;
    string extension;
    string lastModified;
    int length;
};

#endif
