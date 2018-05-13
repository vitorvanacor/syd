#include "sydUtil.h"

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
        for(string& it: hide_from_debug)
        {
            if (it == filename) return;
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
    return filepath.substr(last_slash_position+1);
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
