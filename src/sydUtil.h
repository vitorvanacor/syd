#ifndef SYDUTIL_H
#define SYDUTIL_H

#include <string>

using namespace std;

#define DEBUG false

#define DEFAULT_PORT 4000
#define DEFAULT_HOSTNAME "localhost"
#define DEFAULT_USERNAME "default_user"

void debug(string msg, const char* file = NULL, int line = 0);
string get_filename(string filepath);
string without_extension(string filename);

#endif
