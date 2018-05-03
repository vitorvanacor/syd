#include "File.hpp"

#include <fstream>
#include <iostream>

using namespace std;

File::File(string name)
{
    this->name = name;
}

File::~File() {};

char* File::FileToByteArray() {
    
    // Opens file
    std::ifstream infile(name);

    // Gets length of file
    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);
   
    char *buffer;

    // Reads file
    infile.read(buffer, length);

    return buffer;
}