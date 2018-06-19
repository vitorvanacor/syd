#include "Util.hpp"

list<string> files_in_sync;
pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;
bool can_be_transfered(string filename)
{
    pthread_mutex_lock(&sync_mutex);
    bool can_transfer = true;
    for (string &file_i : files_in_sync)
    {
        if (file_i == filename)
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

void unlock_file(string filename)
{
    pthread_mutex_lock(&sync_mutex);
    files_in_sync.remove(filename);
    pthread_mutex_unlock(&sync_mutex);
}

// Ex: array<string, 2> hide_from_debug = {"Message", "Socket"};
array<string, 2> hide_from_debug = {"Message", "Socket"};

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

string without_extension(string filename)
{
    int dot_position = filename.find_last_of('.');
    return filename.substr(0, dot_position);
}

string time_to_string(time_t timestamp)
{
    char buffer[256];
    struct tm *tm = localtime(&timestamp);
    strftime(buffer, sizeof(buffer), "%H:%M:%S %d/%m/%Y", tm);
    return string(buffer, strlen("HH:MM:SS DD/MM/YYYY"));
}

int terminal_width()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

void print_table_data(string data, int field_width)
{
    cout << left << setw(field_width) << setfill(' ') << data;
}

void print_table_row(string row, char delimiter)
{
    int pos = 0;
    int field_width = 0;
    while ((pos = row.find(delimiter)) != string::npos)
    {
        string data = row.substr(0, pos);
        if (!field_width)
        {
            field_width = terminal_width() / count(row.begin(), row.end(), delimiter);
        }
        print_table_data(data, field_width);
        row.erase(0, pos + 1);
    }
}

void print_table(string table)
{
    string row;
    istringstream string_stream(table);
    for (string line; getline(string_stream, row);)
    {
        print_table_row(row);
    }
}
