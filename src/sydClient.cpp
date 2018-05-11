#include "sydClient.h"

#include "Socket.hpp"
#include "Message.hpp"
#include "File.hpp"
#include "Connection.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    srand(time(NULL));
    string username = DEFAULT_USERNAME;
    string hostname = DEFAULT_HOSTNAME;
    int port = DEFAULT_PORT;

    if (argc > 1)
    {
        username = string(argv[1]);
    }
    if (argc > 2)
    {
        hostname = string(argv[2]);
    }
    if (argc > 3)
    {
        port = atoi(argv[3]);
    }

    File::create_directory(string(getenv("HOME"))+"/sync_dir_"+username);
    Connection *connection = new Connection(username, hostname, port);

    // Main loop
    string command;
    while (true)
    {
        cout << "Enter command: ";
        getline(cin, command);
        if (command == "upload")
        {
            File file = File("/home/pietra/Documentos/UFRGS/CIC/SISOP2/syd/jpg.jpg");

            connection->send(Message::T_UPLOAD, "menes.txt");
            connection->receive_ack();
            connection->send_file(file);
        }
        else if (command == "download")
        {
            connection->send(Message::T_DOWNLOAD, "file_d.txt");
            //connection->receive_file();
        }
        else if (command == "list_server" || command == "ls")
        {
            connection->send(Message::T_LS);
            string server_list = connection->receive(Message::T_LS).content;
            cout << server_list << endl;
            connection->send_ack();
        }
        else if (command == "list_client")
        {
            list_client(string(getenv("HOME"))+"/sync_dir_"+username);
        }
        else if (command == "exit")
        {
            cout << "Logging out..." << endl;
            connection->send(Message::T_BYE);
            connection->receive_ack();
            connection->send_ack();
            break;
        }
    }
    delete connection;
    cout << "Successfully logged out!" << endl;
    return 0;
}

void list_client(string username)
{
    //char dir[255] = string(getenv("HOME"))+username
    DIR* dir = opendir(username.c_str());
    struct dirent  *dp;
    struct stat     statbuf;
    struct passwd  *pwd;
    struct group   *grp;
    struct tm      *tm;
    char            datestring[256];

    while ((dp = readdir(dir)) != NULL) {
        /* Get entry's information. */
        if (stat(dp->d_name, &statbuf) == -1)
            continue;

        /* Print size of file. */
        printf("%u", (intmax_t)statbuf.st_size);

        tm = localtime(&statbuf.st_mtime);
        /* Get localized date string. */
        strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);
        printf(" %s %s\n", datestring, dp->d_name);
    }
}
