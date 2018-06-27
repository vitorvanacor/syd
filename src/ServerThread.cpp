#include "ServerThread.hpp"

#include "Server.hpp"

ServerThread::ServerThread(Server *server, Connection *connection)
{
    is_open = true;
    this->server = server;
    this->connection = connection;
}

ServerThread::~ServerThread()
{
    delete connection;
}

void *ServerThread::run()
{
    connection->confirm();
    username = connection->receive_content(Message::Type::LOGIN);
    File::create_directory(username);
    for (Connection *backup : server->backups)
    {
        backup->send(Message::Type::LOGIN, username);
        backup->send_ack();
    }

    server_sync = new ServerSync(this);
    server_sync->start();

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
            receive_upload(request.content);
        }
        else if (request.type == Message::Type::DOWNLOAD)
        {
            send_download(request.content);
        }
        else if (request.type == Message::Type::LIST_SERVER)
        {
            list_server();
        }
        else if (request.type == Message::Type::BYE)
        {
            break;
        }
    }
    cout << "User " << username << " logged out." << endl;
    for (Connection *backup : server->backups)
    {
        backup->send(Message::Type::CLIENT_DISCONNECT, connection->ip);
    }
    is_open = false;
}

void ServerThread::receive_upload(string filename, Connection *connection)
{
    if (!connection)
    {
        connection = this->connection;
    }
    string filepath = username + '/' + filename;
    if (can_be_transfered(filename))
    {
        connection->send(Message::Type::OK);
        cout << username << " is uploading " << filename << "..." << endl;
        connection->receive_file(filepath);
        for (Connection *backup : server->backups)
        {
            backup->send(Message::Type::UPLOAD, filepath);
            backup->send_file(filepath);
        }
        unlock_file(filename);
        cout << username << " uploaded " << filename << endl;
    }
    else
    {
        cout << "Error: File " << filename << " currently syncing" << endl;
        connection->send(Message::Type::ERROR, "File currently syncing");
        return;
    }
}

void ServerThread::send_download(string filename, Connection *connection)
{
    if (!connection)
    {
        connection = this->connection;
    }
    string filepath = username + '/' + filename;
    File file(filepath);
    if (!file.exists())
    {
        cout << "Error opening file " << filename << " at " << username << endl;
        connection->send(Message::Type::ERROR, "File not found");
        return;
    }
    if (can_be_transfered(filename))
    {
        connection->send(Message::Type::OK);
        cout << username << " is downloading " << filename << "..." << endl;
        connection->send_file(filepath);
        cout << username << " downloaded " << filename << endl;
        unlock_file(filename);
    }
    else
    {
        cout << "Error: File " << filename << " currently syncing" << endl;
        connection->send(Message::Type::ERROR, "File currently syncing");
    }
}

void ServerThread::list_server()
{
    string file_list = File::list_directory_str(username);
    connection->send_long_content(Message::Type::LIST_SERVER, file_list);
}
