#include "sydUtil.h"

#include "Socket.hpp"
#include "Message.hpp"
#include "File.hpp"
#include "Connection.hpp"

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
            cout << "Dummy list client" << endl;
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