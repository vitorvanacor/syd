#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "Socket.hpp"
#include "Message.hpp"
#include "Connection.hpp"
#include "sydUtil.h"

using namespace std;

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
    
    Connection* connection = new Connection(username, hostname, port);

    // Main loop
    string command;
    while (true)
    {
        cout << "Enter command: ";
        getline(cin, command);
        if (command == "upload")
        {
            connection->send(Message::T_UPLOAD, "file_u.txt");
            connection->receive_ack();
            //connection->send_file(File file("file_u.txt"));
        }
        else if (command == "download")
        {
            connection->send(Message::T_DOWNLOAD, "file_d.txt");
            //connection->receive_file();
        }
        else if (command == "list_server" || command == "ls")
        {
            connection->send(Message::T_LS);
            string server_list = connection->receive().content;
            cout << server_list << endl;
        }
        else if (command == "list_client")
        {
            cout << "Dummy list client" << endl;
        }
        else if (command == "exit")
        {
            cout << "(Dummy)Exiting..." << endl;
            connection->send(Message::T_BYE);
            connection->receive_ack();
            break;
        }
    }
    delete connection;
    cout << "Exit successful" << endl;
    return 0;
}