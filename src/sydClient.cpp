#include "sydClient.h"

#include "Socket.hpp"
#include "Message.hpp"
#include "File.hpp"
#include "Connection.hpp"
#include "ClientSync.hpp"

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
    cout << "Connection to " << hostname << "..." << endl;
    Connection *connection = new Connection(username);
    connection->sock = new Socket(port);
    connection->sock->set_host(hostname);
    connection->connect();
    ClientSync client_sync(connection);
    cout << "Successfully logged in as " << username << "!" << endl;

    // Main loop
    string command, filename;
    while (true)
    {
        cout << "Enter command: ";
        //getline(cin, command);
        cin >> command;
        if (command == "upload" || command == "download")
        {
            cin >> filename;
        }
        if (command == "upload")
        {
            try
            {
                if (!ifstream(filename))
                {
                    cout << "Error opening file " << filename << " at " << working_directory() << endl;
                    continue;
                }
                connection->send(Message::T_UPLOAD, filename);
                connection->receive_ack();
                connection->send(Message::T_SOF);
                connection->receive_ack();
                cout << "Uploading " << filename << "..." << endl;
                if (connection->send_file(filename) == 0)
                    cout << filename << " uploaded successfully!" << endl;
                else
                    cout << filename << " uploaded failed!" << endl;
            }
            catch (exception &e)
            {
                connection->send(Message::T_ERROR);
                connection->receive_ack();

                cout << e.what() << endl;
                continue;
            }
        }
        else if (command == "download")
        {
            connection->send(Message::T_DOWNLOAD, filename);
            connection->receive_ack();
            cout << "Downloading " << filename << "..." << endl;
            if (connection->receive_file(filename) == 0)
                cout << filename << " downloaded successfully!" << endl;
            else
                cout << filename << " downloaded failed!" << endl;
        }
        else if (command == "list_server" || command == "ls")
        {
            connection->send(Message::T_LS);
            string server_list = connection->receive(Message::T_LS).content;
            cout << server_list << endl;
            connection->send_ack();
        }
        else if (command == "list_client" || command == "lc")
        {
            list_client(string(getenv("HOME")) + "/sync_dir_" + username);
        }
        else if (command == "exit")
        {
            cout << "Logging out..." << endl;
            connection->send(Message::T_BYE);
            connection->receive_ack();
            connection->send_ack();
            break;
        }
        else
        {
            cout << "Invalid command" << endl;
        }
    }
    delete connection;
    cout << "Successfully logged out!" << endl;
    return 0;
}

void list_client(string username)
{
    DIR *dir = opendir(username.c_str());
    struct dirent *dp;
    struct stat statbuf;
    struct passwd *pwd;
    struct group *grp;
    struct tm *tm;
    char datestring[256];

    while ((dp = readdir(dir)) != NULL)
    {
        /* Get entry's information. */
        if (stat(dp->d_name, &statbuf) == -1)
            continue;

        /* Print size of file. */
        //printf("%u", (intmax_t)statbuf.st_size);

        tm = localtime(&statbuf.st_mtime);
        /* Get localized date string. */
        strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);
        printf(" %s %s\n", datestring, dp->d_name);
    }
}
