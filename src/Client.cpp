#include "Client.hpp"

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
    connection = new Connection(hostname, port);
    connection->send(Message::Type::LOGIN, username);
    debug("Login ok");
    user_dir = HOME + "/sync_dir_" + username;
    File::create_directory(user_dir);

    ClientSync* client_sync = new ClientSync(this);
    client_sync->start();
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
            string file_list = File::list_directory_str(user_dir);
            File::print_file_list(file_list);
        }
        else if (command == "list_dir" || command == "dir")
        {
            string file_list = File::list_directory_str(File::working_directory());
            File::print_file_list(file_list);
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

void Client::upload_file(string filename, string dirpath, Connection *connection)
{
    if (dirpath.empty())
    {
        dirpath = File::working_directory();
    }
    if (!File(dirpath+"/"+filename).exists())
    {
        cout << "File not found at " << dirpath << endl;
        return;
    }
    if (!connection)
    {
        connection = this->connection;
    }
    try
    {
        connection->send(Message::Type::UPLOAD, filename);
        connection->receive(Message::Type::OK);
        cout << "Uploading " << filename << "..." << endl;
        connection->send_file(dirpath+"/"+filename);
        cout << filename << " uploaded successfully!" << endl;
    }
    catch (ResponseException &e)
    {
        cout << e.what() << endl;
    }
}

void Client::download_file(string filename, string dirpath, Connection *connection)
{
    if (dirpath.empty())
    {
        dirpath = File::working_directory();
    }
    if (!connection)
    {
        connection = this->connection;
    }
    try
    {
        connection->send(Message::Type::DOWNLOAD, filename);
        connection->receive(Message::Type::OK);
        cout << "Downloading " << filename << " to " << dirpath << "..." << endl;
        connection->receive_file(dirpath + "/" + filename);
        cout << filename << " downloaded successfully!" << endl;
    }
    catch (ResponseException &e)
    {
        cout << e.what() << endl;
    }
}

void Client::list_server()
{
    connection->send(Message::Type::LIST_SERVER);
    string file_list = connection->receive_long_content(Message::Type::LIST_SERVER);
    File::print_file_list(file_list);
}

void Client::close_session()
{
    cout << "Logging out..." << endl;
    connection->send(Message::Type::BYE);
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
