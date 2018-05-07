#ifndef SYDUTIL_H
#define SYDUTIL_H

#include <string>
#include <stdexcept>

using namespace std;

#define DEBUG true
#define TIMEOUT_IN_SECONDS 1

#define DEFAULT_PORT 4000
#define DEFAULT_HOSTNAME "localhost"
#define DEFAULT_USERNAME "default_user"

void debug(string msg, const char* file = NULL, int line = 0);
string get_filename(string filepath);
string without_extension(string filename);

class timeout_exception: public runtime_error
{
public:
    timeout_exception(): runtime_error("ERROR: Timeout") { }
};

#endif
