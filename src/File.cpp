#include "File.hpp"

#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

File::File(string path)
{
    this->path = path;
    this->length = FileLength();
}

File::~File() {};

// Getters
int File::GetLength() {
    return length;
}

// Setters

// Others
int File::FileLength() {
    // Opens file
    std::ifstream infile(path);

    // Gets length of file
    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);

    infile.close();

    return length;
}

void File::FileToByteArray(char *pointer) {
    
    // Opens file
    std::ifstream infile(path);

    char buffer[length];

    // Reads file
    infile.read(buffer, length);

    infile.close();

    memcpy(pointer, buffer, length);

}