#include "File.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <langinfo.h>
#include <time.h>

File::File(string filepath)
    : filepath(filepath)
    , modification_time_str("")
    , access_time_str("")
    , creation_time_str("")
{
    if (stat(filepath.c_str(), &info) >= 0)
    {
        modification_time_str = time_to_string(info.st_mtime);
        access_time_str = time_to_string(info.st_atime);
        creation_time_str = time_to_string(info.st_ctime);
    }
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

string File::stringify()
{
    string file_str = "";
    file_str += get_filename(filepath);
    file_str += "|";
    file_str += creation_time_str;
    file_str += "|";
    file_str += modification_time_str;
    file_str += "|";
    file_str += access_time_str;
    file_str += "|";
}

string File::list_directory(string dirpath)
{
    list<string> file_list;
    DIR *directory;
    struct dirent *dir_entry;
    directory = opendir(dirpath.c_str());
    if (directory)
    {
        while (dir_entry = readdir(directory))
        {
            string filename = string(dir_entry->d_name);
            if (filename != "." && filename != "..")
            {
                File file(filename);
                file_list += file.stringify() + "\n";
            }
        }
    }
    closedir(directory);
    return file_list;
}

void File::print_file_list(string file_list)
{
    file_list.insert(0, "Name|Created At|Modified At|Last Accessed At|\n");
    print_table(file_list);
}

string File::working_directory()
{
    return string(getenv("PWD"));
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
    {
        debug("Couldn't open the directory");
    }

    return files;
}

list<File> File::parse_file_list(string msg)
{
    string delimiter = "|";
    string token;
    int pos = 0;
    while ((pos = filelist.find(delimiter)) != std::string::npos)
    {
        token = filelist.substr(0, pos);
        printElement(token, fieldWidth, separator);
        filelist.erase(0, pos + delimiter.length());
    }
}

File File::parse_file(string file_str)
{
    // Find position of separators
    int name_sep = file_str.find('|');
    int creation_sep = file_str.find('|', name_sep + 1);
    int mod_sep = file_str.find('|', creation_sep + 1);
    int access_sep = file_str.find_last_of('|');

    // Calculate sections length
    int name_len = name_sep;
    int creation_len = creation_sep - name_sep - 1;
    int mod_len = mod_sep - creation_sep - 1;
    int access_len = access_sep - mod_sep - 1;

    // Assign
    string name = file_str.substr(0, name_len);
    string creation_time = file_str.substr(name_sep + 1, creation_len);
    string mod_time = file_str.substr(creation_sep + 1, mod_len);
    string access_time = file_str.substr(mod_sep + 1, access_len);

    File file(name);
    file.creation_time_str = creation_time;
    file.modification_time_str = mod_time;
    file.access_time_str = access_time;
    return file;
}

