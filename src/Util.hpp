#ifndef SYDUTIL_H
#define SYDUTIL_H

//C headers
#include <netinet/in.h>
#include <netdb.h> // hostent
#include <pthread.h>
#include <string.h> // bzero
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h> // close
#include <utime.h>

//C++ headers
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <list>

using namespace std;

#define DEFAULT_PORT 4000
#define DEFAULT_HOSTNAME "localhost"
#define DEFAULT_USERNAME "default_user"

static const string HOME = string(getenv("HOME"));

/* MUTEX */
bool can_be_transfered(string filename);
void unlock_file(string fil);

/* TABLE */
void print_table(string table);

/* DEBUG */
void debug(string msg, const char *file = NULL, int line = 0, int color = 0);
string get_filename(string filepath);
string without_extension(string filename);

/* UTIL */
bool contains(list<string> string_list, string value);
string time_to_string(time_t timestamp);

enum Color
{
  RED = 31,
  GREEN = 32,
  YELLOW = 33,
  BLUE = 34,
  MAGENTA = 35,
  CYAN = 36,
  WHITE = 37
};

class timeout_exception : public runtime_error
{
public:
  timeout_exception() : runtime_error("ERROR: Timeout") {}
};

#endif
