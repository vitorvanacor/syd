#ifndef FILE_H
#define FILE_H

#include "Util.hpp"

class File
{
  public:
    File(string filepath);
    
    string name();
    bool exists();
    unsigned int size();
    unsigned int modification_time();
    void set_modification_time(int modtime);
    string stringify();

    string filepath;
    string modification_time_str;
    string access_time_str;
    string creation_time_str;

    static string working_directory();
    static void create_directory(string path);
    static list<File> list_directory(string dirpath);
    static string list_directory_str(string dirpath);
    static void print_file_list(string file_list);
    static list<File> parse_file_list(string msg);
    static File parse_file(string file_str);

  private:
    bool update_info();
    struct stat info;

};

#endif
