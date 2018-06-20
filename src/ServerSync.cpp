#include "ServerSync.hpp"

ServerSync::ServerSync(ServerThread *server) : Thread()
{
    this->server = server;
    log_file = server->username + "/sync_log";
}

ServerSync::~ServerSync()
{
    delete connection;
}

void *ServerSync::run()
{
    this->connection = server->connection->receive_connection();
    connection->confirm();
    while (true)
    {
        sync_client_files();
        log("--- Send remaining to client");
        send_files_to_client();
        log("--- Sync Done");
    }
}

void ServerSync::sync_client_files()
{
    log("--- Begin Sync");
    updated_files.clear();
    while (true)
    {
        Message msg_sync = connection->receive(Message::type_sync());
        if (msg_sync.type == Message::Type::DONE)
        {
            break;
        }
        string filename = msg_sync.content;
        string filepath = server->username + "/" + filename;
        File file(filepath);
        connection->send(Message::Type::MODTIME, to_string(file.modification_time()));
        Message msg_action = connection->receive(Message::type_action());
        if (msg_action.type == Message::Type::UPLOAD)
        {
            log("Receive " + filename + "...");
            server->receive_upload(filename, connection);
            log("ok");
        }
        else if (msg_action.type == Message::Type::DOWNLOAD)
        {
            log("Send " + filename + "...");
            server->send_download(filename, connection);
            log("ok");
        }
        updated_files.push_back(filename);
    }
}

void ServerSync::send_files_to_client()
{
    updated_files.push_back("sync_log");
    for (File &file : File::list_directory(server->username))
    {
        if (!contains(updated_files, file.name()))
        {
            log("Send " + file.name() + "...");
            connection->send(Message::Type::SYNC, file.name());
            connection->send_file(file.filepath);
            log("ok");
        }
    }
    connection->send(Message::Type::DONE);
}