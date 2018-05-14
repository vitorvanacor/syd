#include "ServerThread.hpp"

#include "ServerSync.hpp"

ServerThread::ServerThread(Connection *connection)
{
    is_open = true;
    this->connection = connection;
}

ServerThread::~ServerThread()
{
    delete connection;
}

void *ServerThread::run()
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
            connection->send(Message::T_LS, connection->list_server_dir(connection->user_directory));
            connection->receive_ack();
        }
        else if (request.type == Message::T_UPLOAD)
        {
            connection->send_ack();
            cout << connection->username << " is uploading " << request.content << "..." << endl;
            connection->receive_file(connection->user_directory + "/" + request.content);
            cout << connection->username << " uploaded " << request.content << endl;
        }
        else if (request.type == Message::T_DOWNLOAD)
        {
            connection->send_ack();
            try
            {
                if (!ifstream(connection->user_directory + '/' + request.content))
                {
                    cout << "Error opening file " << request.content << " at " << connection->user_directory << endl;

                    connection->send(Message::T_ERROR);
                    connection->receive_ack();

                    continue;
                }
                connection->send(Message::T_SOF);
                connection->receive_ack();
                cout << connection->username << " is downloading " << request.content << "..." << endl;
                connection->send_file(connection->user_directory + "/" + request.content);
                cout << connection->username << " downloaded " << request.content << endl;
            }
            catch (exception &e)
            {
                connection->send(Message::T_ERROR);
                connection->receive_ack();

                cout << e.what() << endl;

                continue;
            }
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
