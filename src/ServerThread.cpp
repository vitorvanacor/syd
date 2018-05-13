#include "ServerThread.hpp"

#include "ServerSync.hpp"

ServerThread::ServerThread(Connection* connection)
{
    is_open = true;
    this->connection = connection;
}

ServerThread::~ServerThread()
{
    delete connection;
}

void* ServerThread::run()
{
    connection->accept_connection();
    ServerSync server_sync(connection);
    server_sync.start();
    cout << connection->username << " logged in" << endl;
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
            cout << connection->username << " is uploading " << request.content << "..." << endl;
            connection->receive_file(request.content);
            cout << connection->username << " uploaded " << request.content << endl;
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
