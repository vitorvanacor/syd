#include "File.hpp"

File::File(string path)
{
    this->path = path;
}

File::~File(){};

// Getters
string File::GetPath()
{
    return path;
}

// Setters
