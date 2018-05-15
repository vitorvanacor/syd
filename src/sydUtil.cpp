#include "sydUtil.h"

list<string> files_in_sync;
pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;
bool can_be_transfered (string filename)
{
    pthread_mutex_lock(&sync_mutex);
    bool can_transfer = true;
    for (string &it : files_in_sync)
    {
        if (it == filename)
        {
            can_transfer = false;
            break;
        }
    }
    if (can_transfer)
    {
        files_in_sync.push_back(filename);
    }
    pthread_mutex_unlock(&sync_mutex);
    return can_transfer;
}

void unlock_file (string filename)
{
    pthread_mutex_lock(&sync_mutex);
    files_in_sync.remove(filename);
    pthread_mutex_unlock(&sync_mutex);
}



// Ex: array<string, 2> hide_from_debug = {"Message", "Socket"};
array<string, 1> hide_from_debug = {""};

void debug(string msg, const char *file, int line, int color)
{
#ifndef DEBUG
    return;
#endif

    if (file)
    {
        string filepath = without_extension(string(file));
        string filename = get_filename(filepath);
        for (string &it : hide_from_debug)
        {
            if (it == filename)
                return;
        }
        if (color)
        {
            cout << "\033[" << color << "m";
        }
        cout << filename << "\033[0m";
    }
    cout << ": " << msg;
    if (line)
    {
        cout << " (line " << line << ")";
    }
    cout << endl;
}

string get_filename(string filepath)
{
    int last_slash_position = filepath.find_last_of('/');
    return filepath.substr(last_slash_position + 1);
}

unsigned int get_filetimestamp(string filepath)
{
    struct stat result;
    if (stat(filepath.c_str(), &result) == 0)
    {
        int mod_time = result.st_mtime;
        return mod_time;
    }
    else
        return -1;
}

string without_extension(string filename)
{
    int dot_position = filename.find_last_of('.');
    return filename.substr(0, dot_position);
}

string working_directory()
{
    return string(getenv("PWD"));
}
