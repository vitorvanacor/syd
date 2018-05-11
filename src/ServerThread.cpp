#include "ServerThread.hpp"

ServerThread::ServerThread(string username, string session, Socket* new_socket)
{
    is_open = true;
    connection = new Connection(username, session, new_socket);
}

ServerThread::~ServerThread()
{
    delete connection;
}

void* ServerThread::run()
{
    connection->accept_connection();
    File::create_directory(connection->username);
    cout << connection->username << " successfully logged in!" << endl;
    while (true)
    {
        Message request = connection->receive_request();
        if (request.type == Message::T_LS)
        {
            connection->send(Message::T_LS, "file1.txt;2018-01-01 13:00:00|file2.txt;2018-02-01 15:00:00");
            connection->receive_ack();
        }
        else if (request.type == Message::T_UPLOAD)
        {
            connection->send_ack();
            connection->receive_file();
        }
        else if (request.type == Message::T_DOWNLOAD)
        {
            //connection->send_file(request.content);
        }
        else if (request.type == Message::T_BYE)
        {
            connection->send_ack();
            connection->receive_ack();
            break;
        }
    }
    cout << "User " << connection->username << " logged out." << endl;
    is_open = false;
    return NULL;
}
