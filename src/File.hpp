#ifndef FILE_H
#define FILE_H

#include "Util.hpp"

class File
{
  public:
    File(string filepath);
    
    bool exists();
    unsigned int size();
    unsigned int modification_time();

    static list<string> list_filename(string dirpath);
    static void create_directory(string path);
    static string list_directory(string dirpath);

  private:
    string filepath;
    struct stat info;

};

#endif
