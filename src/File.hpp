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
    string stringify();

    string modification_time_str;
    string access_time_str;
    string creation_time_str;

    static string working_directory();
    static list<string> list_filename(string dirpath);
    static void create_directory(string path);
    static string list_directory(string dirpath);
    static void print_file_list(string file_list);
    static list<File> parse_file_list(string msg);
    static File parse_file(string file_str);

  private:
    string filepath;
    struct stat info;

};

#endif
