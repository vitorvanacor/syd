#include "File.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <langinfo.h>
#include <time.h>

File::File(string path)
{
    this->path = path;
}

File::~File(){};

void File::create_directory(string path)
{
    int status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status < 0 && errno != EEXIST)
    {
        throw runtime_error(strerror(errno));
    }
}

unsigned int get_filesize(string filename)
{
    struct stat info;
    if(stat(filename.c_str(), &info) != 0)
    {
        return 0;
    }
    return info.st_size;   
}

string File::list_directory(string dirpath)
{
    DIR *dp;
    struct dirent *ep;
    struct stat fileInfo;
    string files = "";
    struct tm *tm;
    char mod_time[256] = {0};
    char create_time[256] = {0};
    char last_time[256] = {0};

    dp = opendir (dirpath.c_str());
    if (dp != NULL)
    {
        files += "Name|Created At|Modified At|Last Accessed At|\n";
        while ((ep = readdir(dp)))
        {
            if(strcmp(ep->d_name,".") != 0 && strcmp(ep->d_name,"..") != 0)
            {
                if (stat(ep->d_name, &fileInfo) == -1)
                    continue;
                tm = localtime(&fileInfo.st_mtime);
                strftime(mod_time, sizeof(mod_time), nl_langinfo(D_T_FMT), tm);
                tm = localtime(&fileInfo.st_ctime);
                strftime(create_time, sizeof(create_time), nl_langinfo(D_T_FMT), tm);
                tm = localtime(&fileInfo.st_atime);
                strftime(last_time, sizeof(last_time), nl_langinfo(D_T_FMT), tm);

                files += ep->d_name;
                files += "| ";
                files += create_time;
                files += "| ";
                files += mod_time;
                files += "| ";
                files += last_time;
                files += "|\n";
            }
        }
        (void) closedir (dp);
    }
    else
        debug("Couldn't open the directory");
    
    return files;
}

// Getters
string File::GetPath()
{
    return path;
}

// Setters
