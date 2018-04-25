#ifndef SYDCLIENT_H
#define SYDCLIENT_H

/**
* Stablish session between client and server.
* host – server address
* port – server port
*/
int login_server(char *host, int port);

/**
* Sync directory “sync_dir_<username>” with server.
*/
void sync_client();

/**
* Send file to server. Called when uploading a file.
* file – filename.ext of file being sent
*/
void send_file(char *file);

/** Get file from server. Called when downloading a file.
* file – filename.ext
*/
void get_file(char *file);

/**
* Delete file from "sync_dir_<username>".
* file –filename.ext
*/
void delete_file(char *file);

/**
* Close session with server
*/
void close_session();

#endif
