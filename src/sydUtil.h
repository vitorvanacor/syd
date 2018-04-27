#ifndef SYDUTIL_H
#define SYDUTIL_H

#include <string>

using namespace std;

#define TYPE_LEN 8
#define SESSION_LEN 4
#define SEQUENCE_LEN 4
#define HEADER_LEN (TYPE_LEN + SESSION_LEN + SEQUENCE_LEN + 3)
#define HEADER_SEPARATOR '|'
#define HEADER_FILLER ' '

string mount_header(const char* type, const char* session, const char* sequence);

void debug(string s, int line = 0, const char* function = NULL);

#endif
