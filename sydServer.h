struct client {
	int devices[2];
	char userid[MAXNAME];
	struct file_info[MAXFILES];
	int logged_in;
}

struct file_info {
	char name[MAXNAME];
	char extension[MAXNAME];
	char last_modified[MAXNAME];
	int size;
}

/**
* Sync server with user's “sync_dir_<username>”
*/
void sync_server();

/**
* Receive file from client. Called when uploading a file.
* file – path/filename.ext of file being received
*/
void receive_file(char *file);

/**
* Send file to user. Called when downloading a file.
* file – filename.ext
*/
void send_file(char *file);
