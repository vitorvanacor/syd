#ifndef FILE_H
#define FILE_H

#include "sydUtil.h"

class File
{
  public:

    static list<string> list_filename(string dirpath);
    static void create_directory(string path);
    unsigned int get_filesize(string filename);
    static string list_directory(string dirpath);

};

#endif
