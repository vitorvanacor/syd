#ifndef FILE_H
#define FILE_H

#include "sydUtil.h"

class File
{
public:
  static list<string> list_filename(string dirpath);
  static void create_directory(string path);
  static string list_directory(string dirpath);
  static int delete_file(string filepath);
};

#endif
