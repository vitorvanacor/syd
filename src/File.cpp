#include "File.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <langinfo.h>
#include <time.h>

File::File(string filepath)
{
    this->filepath = filepath;
    stat(filepath.c_str(), &info);
}

bool File::exists()
{
    if (ifstream(filepath))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void File::create_directory(string path)
{
    int status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status < 0 && errno != EEXIST)
    {
        throw runtime_error(strerror(errno));
    }
}

unsigned int File::size()
{
    return info.st_size;
}

unsigned int File::modification_time()
{
    return info.st_mtime;
}

list<string> File::list_filename(string dirpath)
{
    list<string> filenames;
    DIR *dp;
    struct dirent *ep;
    dp = opendir(dirpath.c_str());
    if (dp != NULL)
    {
        while ((ep = readdir(dp)))
        {
            if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0)
            {
                filenames.push_back(ep->d_name);
            }
        }
        (void)closedir(dp);
    }
    else
        debug("Couldn't open the directory");
    return filenames;
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

    dp = opendir(dirpath.c_str());
    if (dp != NULL)
    {
        files += "Name|Created At|Modified At|Last Accessed At|\n";
        while ((ep = readdir(dp)))
        {
            if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0)
            {
                char fullpath[100];
                strcpy(fullpath, dirpath.c_str());
                strcat(fullpath, "/");
                strcat(fullpath, ep->d_name);

                if (stat(fullpath, &fileInfo) == -1)
                {
                    perror(0);
                    continue;
                }
                tm = localtime(&fileInfo.st_mtime);
                strftime(mod_time, sizeof(mod_time), "%H:%M:%S %d/%m/%Y", tm);
                tm = localtime(&fileInfo.st_ctime);
                strftime(create_time, sizeof(create_time), "%H:%M:%S %d/%m/%Y", tm);
                tm = localtime(&fileInfo.st_atime);
                strftime(last_time, sizeof(last_time), "%H:%M:%S %d/%m/%Y", tm);

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
        (void)closedir(dp);
    }
    else
        debug("Couldn't open the directory");

    return files;
}
