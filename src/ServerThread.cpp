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
    connection->confirm_new_connection();
    username = connection->receive_content(Message::Type::LOGIN);
    File::create_directory(username);

    ServerSync server_sync(connection);
    server_sync.start();

    cout << username << " logged in" << endl;

    mainloop();

    return NULL;
}

void ServerThread::mainloop()
{
    while (true)
    {
        Message request = connection->receive_request();

        if (request.type == Message::Type::UPLOAD)
        {
            receive_file(request.content);
        }
        else if (request.type == Message::Type::DOWNLOAD)
        {
            send_file(request.content);
        }
        else if (request.type == Message::Type::LIST_SERVER)
        {
            list_server();
        }
        else if (request.type == Message::Type::BYE)
        {
            close_session();
            break;
        }
    }
    cout << "User " << username << " logged out." << endl;
    is_open = false;
}

void ServerThread::receive_file(string filename)
{
    string filepath = username + '/' + filename;
    if (can_be_transfered(filename))
    {
        connection->send_ack();
        cout << username << " is uploading " << filename << "..." << endl;
        connection->receive_file(filepath);
        unlock_file(filename);
        cout << username << " uploaded " << filename << endl;
    }
    else
    {
        cout << "Error: File " << filename << " currently syncing" << endl;
        connection->send_ack(false);
        return;
    }
}

void ServerThread::send_file(string filename)
{
    string filepath = username + '/' + filename;
    File file(filepath);
    if (!file.exists())
    {
        cout << "Error opening file " << filename << " at " << username << endl;
        connection->send_ack(false);
        return;
    }
    if (can_be_transfered(filename))
    {
        connection->send_ack();
        connection->send(Message::Type::MODTIME, to_string(file.modification_time()));

        cout << username << " is downloading " << filename << "..." << endl;
        connection->send_file(filepath);
        cout << username << " downloaded " << filename << endl;
        unlock_file(filename);
        }
    else
    {
        cout << "Error: File " << filename << " already being transfered" << endl;
        connection->send_ack(false);
        return;
    }
}

void ServerThread::list_server()
{
    connection->send_long_content(Message::Type::LIST_SERVER, File::list_directory(username));
}

void ServerThread::close_session()
{
    connection->send_ack();
    connection->receive_ack();
}
