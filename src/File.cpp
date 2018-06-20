#include "File.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <langinfo.h>
#include <time.h>

File::File(string filepath)
    : filepath(filepath), modification_time_str(""), access_time_str(""), creation_time_str("")
{
    update_info();
}

bool File::update_info()
{
    if (stat(filepath.c_str(), &info) >= 0)
    {
        modification_time_str = time_to_string(info.st_mtime);
        access_time_str = time_to_string(info.st_atime);
        creation_time_str = time_to_string(info.st_ctime);
        return true;
    }
    else
    {
        return false;
    }
}

string File::name()
{
    return get_filename(filepath);
}

bool File::exists()
{
    return stat(filepath.c_str(), &info) == 0;
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
    if (update_info())
    {
        return info.st_mtime;
    }
    else
    {
        return 0;
    }
}

void File::set_modification_time(int modtime)
{
    struct utimbuf ubuf;
    ubuf.modtime = modtime;
    if (utime(filepath.c_str(), &ubuf) == 0)
    {
        update_info();
    }
    else
    {
        perror("utime() error");
    }
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
    return file_str;
}

list<File> File::list_directory(string dirpath)
{
    list<File> file_list;
    DIR *directory;
    struct dirent *dir_entry;
    directory = opendir(dirpath.c_str());
    if (directory)
    {
        while ((dir_entry = readdir(directory)))
        {
            string filename = string(dir_entry->d_name);
            if (filename != "." && filename != ".." && filename != "sync_log")
            {
                File file(dirpath+"/"+filename);
                file_list.push_back(file);
            }
        }
    }
    closedir(directory);
    return file_list;
}

string File::list_directory_str(string dirpath)
{
    string list_dir_str = "";
    list<File> list_dir = File::list_directory(dirpath);
    for (File &file : list_dir)
    {
        list_dir_str += file.stringify() + "\n";
    }
    return list_dir_str;
}

void File::print_file_list(string file_list)
{
    if (file_list.empty())
    {
        file_list = "(Empty)|";
    }
    file_list.insert(0, "Name|Created At|Modified At|Last Accessed At|\n");
    print_table(file_list);
}

string File::working_directory()
{
    return string(getenv("PWD"));
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
