#ifndef SYDUTIL_H
#define SYDUTIL_H

//C headers
#include <netinet/in.h>
#include <netdb.h> // hostent
#include <pthread.h>
#include <string.h> // bzero
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h> // close

//C++ headers
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <stdexcept>
#include <vector>


using namespace std;

#define DEFAULT_PORT 4001
#define DEFAULT_HOSTNAME "localhost"
#define DEFAULT_USERNAME "default_user"

static const string HOME = string(getenv("HOME"));

void debug(string msg, const char *file = NULL, int line = 0);
string get_filename(string filepath);
string without_extension(string filename);
string working_directory();

class timeout_exception : public runtime_error
{
  public:
    timeout_exception() : runtime_error("ERROR: Timeout") {}
};

#endif
