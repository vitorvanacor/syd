#include "ServerSync.hpp"

ServerSync::ServerSync(ServerThread* server)
{
    this->server = server;
    this->connection = server->connection->receive_connection();
}

ServerSync::~ServerSync()
{
    delete connection;
}

void *ServerSync::run()
{
    connection->confirm();
    list<string> updated_files;
    while (true)
    {
        Message msg_sync = connection->receive(Message::type_sync());
        if (msg_sync.type == Message::Type::DONE)
        {
            break;
        }
        string filename = msg_sync.content;
        File file(filename);
        connection->send(Message::Type::MODTIME, to_string(file.modification_time()));
        Message msg_action = connection->receive(Message::type_action());
        if (msg_action.type == Message::Type::UPLOAD)
        {
            server->receive_file(filename, connection);
        }
        else if (msg_action.type == Message::Type::DOWNLOAD)
        {
            server->send_file(filename, connection);
        }
        updated_files.push_back(filename);
    }
    for (File &file : File::list_directory(server->username))
    {
        if (!contains(updated_files, file.filepath))
        {
            connection->send(Message::Type::DOWNLOAD);
            server->send_file(file.filepath, connection);
        }
    }
    connection->send(Message::Type::DONE);
}