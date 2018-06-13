#include "ServerThread.hpp"

#include "ServerSync.hpp"

ServerThread::ServerThread(Connection *connection, string username)
{
    is_open = true;
    this->connection = connection;
    this->username = username;
}

ServerThread::~ServerThread()
{
    delete connection;
}

void *ServerThread::run()
{
    connection->accept_connection();
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

        if (request.type == Message::T_UPLOAD)
        {
            receive_file(request.content);
        }
        else if (request.type == Message::T_DOWNLOAD)
        {
            send_file(request.content);
        }
        else if (request.type == Message::T_LS)
        {
            list_server();
        }
        else if (request.type == Message::T_BYE)
        {
            close_session();
            break;
        }
    }
    cout << "User " << connection->username << " logged out." << endl;
    is_open = false;
}

void ServerThread::receive_file(string filename)
{
    string filepath = username + '/' + filename;
    if (!can_be_transfered(filename))
    {
        cout << "Error: File " << filename << " already being transfered" << endl;
        connection->send_ack(false);
        connection->receive_ack();
        return;
    }
    connection->send_ack();
    cout << username << " is uploading " << filename << "..." << endl;
    connection->receive_file(filepath);
    unlock_file(filename);
    cout << username << " uploaded " << filename << endl;
}

void ServerThread::send_file(string filename)
{
    string filepath = username + '/' + filename;
    if (!can_be_transfered(filename))
    {
        cout << "Error: File " << filename << " already being transfered" << endl;
        connection->send_ack(false);
        connection->receive_ack();
        return;
    }
    connection->send_ack();
    try
    {
        File file(filepath);
        if (!file.exists())
        {
            cout << "Error opening file " << filename << " at " << username << endl;
            connection->send_ack(false);
            connection->receive_ack();
            unlock_file(filename);
            return;
        }

        connection->send(Message::T_MODTIME, to_string(file.modification_time()));
        connection->receive_ack();

        cout << username << " is downloading " << filename << "..." << endl;
        connection->send_file(filepath);
        unlock_file(filename);
        cout << username << " downloaded " << filename << endl;
    }
    catch (exception &e)
    {
        connection->send_ack(false);
        connection->receive_ack();

        cout << e.what() << endl;
        unlock_file(filename);
        return;
    }
    unlock_file(filename);
}

void ServerThread::list_server()
{
    connection->send_string(File::list_directory(username));
}
