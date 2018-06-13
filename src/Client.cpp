#include "Client.hpp"

#include "File.hpp"
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
#include <iomanip>

Client::~Client()
{
    delete connection;
}

void Client::start(string username, string hostname, int port)
{
    cout << "Connecting to " << hostname << ":" << port << " ..." << endl;
    connection = new Connection();
    connection->sock = new Socket(port);
    connection->sock->set_host(hostname);
    connection->connect(username);

    user_directory = HOME + "/sync_dir_" + username;
    File::create_directory(user_directory);

    ClientSync client_sync(connection);
    cout << "Successfully logged in as " << username << "!" << endl;

    mainloop();
}

void Client::mainloop()
{
    string command, filename;
    while (true)
    {
        cout << "Enter command: ";
        cin >> command;
        if (command == "upload" || command == "download")
        {
            cin >> filename;
        }
        if (command == "upload")
        {
            upload_file(filename);
        }
        else if (command == "download")
        {
            download_file(filename);
        }
        else if (command == "list_server" || command == "ls")
        {
            list_server();
        }
        else if (command == "list_client" || command == "lc")
        {
            list_client();
        }
        else if (command == "exit")
        {
            close_session();
            break;
        }
        else
        {
            cout << "Invalid command" << endl;
        }
    }
}

void Client::upload_file(string filename)
{
    try
    {
        File file(filename);
        if (!file.exists())
        {
            cout << "File " << filename << " not found at " << working_directory() << endl;
            return;
        }

        connection->send(Message::T_UPLOAD, filename);
        int ok = connection->receive_ack();
        if (!ok)
        {
            cout << "Wait, " << filename << " is currently syncing." << endl;
            connection->send_ack();
            return;
        }
        connection->send(Message::T_MODTIME, to_string(file.modification_time()));

        cout << "Uploading " << filename << "..." << endl;
        connection->send_file(filename);
    }
    catch (exception &e)
    {
        connection->send_ack(false);
        connection->receive_ack();

        cout << e.what() << endl;
        return;
    }
}

void Client::download_file(string filename)
{
    connection->send(Message::T_DOWNLOAD, filename);
    bool ok = connection->receive_ack();
    if (!ok)
    {
        cout << "Wait, " << filename << " is currently in sync." << endl;
        connection->send_ack();
        return;
    }
    cout << "Downloading " << filename << "..." << endl;
    string filepath = working_directory() + '/' + filename;
    connection->receive_file(filepath);
    cout << filename << " downloaded successfully into " << working_directory() << "!" << endl;
}

void Client::list_server()
{
    connection->send(Message::T_LS);
    string server_list = connection->receive_string();
    filelist_parser(server_list);
    connection->send_ack();
}

void Client::list_client()
{
    DIR *dp = opendir(user_directory.c_str());
    struct dirent *ep;
    struct stat fileInfo;
    struct passwd *pwd;
    struct group *grp;
    struct tm *tm;
    string files = "";
    char mod_time[256] = {0};
    char create_time[256] = {0};
    char last_time[256] = {0};

    if (dp != NULL)
    {
        files += "Name|Created At|Modified At|Last Accessed At|\n";
        while ((ep = readdir(dp)) != NULL)
        {
            if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0)
            {
                if (stat(user_directory.c_str(), &fileInfo) == -1)
                {
                    perror(0);
                    continue;
                }
                tm = localtime(&fileInfo.st_mtime);
                strftime(mod_time, sizeof(mod_time), "%H:%M:%S %d/%m/%Y", tm);
                tm = localtime(&fileInfo.st_ctime);
                strftime(create_time, sizeof(create_time), "%H:%M:%S %d/%m/%Y", tm);
                tm = localtime(&fileInfo.st_atime);
                strftime(last_time, sizeof(last_time), "%H:%M:%S %d/%m/%Y", tm);

                files += ep->d_name;
                files += "| ";
                files += create_time;
                files += "| ";
                files += mod_time;
                files += "| ";
                files += last_time;
                files += "|\n";
            }
        }
        (void)closedir(dp);
        filelist_parser(files);
    }
    else
        debug("Couldn't open user sync dir");
}

void Client::close_session()
{
    cout << "Logging out..." << endl;
    connection->send(Message::T_BYE);
    connection->receive_ack();
    connection->send_ack();
    cout << "Successfully logged out!" << endl;
}

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

    Client client;
    client.start(username, hostname, port);
}



void printElement(string data, int width, char separator)
{
    cout << left << setw(width) << setfill(separator) << data;
}

void filelist_parser(string filelist)
{
    char separator = ' ';
    int fieldWidth = 26;
    string delimiter = "|";
    string token;
    int pos = 0;
    while ((pos = filelist.find(delimiter)) != std::string::npos)
    {
        token = filelist.substr(0, pos);
        printElement(token, fieldWidth, separator);
        filelist.erase(0, pos + delimiter.length());
    }
    cout << endl;
}