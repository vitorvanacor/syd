#include "File.hpp"

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
    if (stat(filename.c_str(), &info) != 0)
    {
        return 0;
    }
    return info.st_size;
}

// Getters
string File::GetPath()
{
    return path;
}

// Setters
