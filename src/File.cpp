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
    char datestring[256] = {0};

    dp = opendir (dirpath.c_str());
    if (dp != NULL)
    {
        while ((ep = readdir(dp)))
        {
            if(strcmp(ep->d_name,".") != 0 && strcmp(ep->d_name,"..") != 0)
            {
                stat(ep->d_name, &fileInfo);
                tm = localtime(&fileInfo.st_mtime);
                strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);
                files += ep->d_name;
                files += ";";
                files += datestring;
                files += "|";
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
