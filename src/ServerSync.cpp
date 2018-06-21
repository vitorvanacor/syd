#include "ServerSync.hpp"

#include "Server.hpp"
#include "ServerThread.hpp"

ServerSync::ServerSync(ServerThread *parent) : Thread()
{
    this->parent = parent;
    log_file = parent->username + "/sync_log";
}

ServerSync::~ServerSync()
{
    delete connection;
}

void *ServerSync::run()
{
    this->connection = parent->connection->receive_connection();
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
    files_not_synced = File::list_directory_filenames(parent->username);
    while (true)
    {
        Message msg = connection->receive(Message::type_sync());
        if (msg.type == Message::Type::DONE)
        {
            break;
        }
        else if (msg.type == Message::Type::SYNC)
        {
            sync_file(msg.content);
        }
        else if (msg.type == Message::Type::DELETE)
        {
            delete_file(msg.content);
        }
    }
}

void ServerSync::sync_file(string filename)
{
    File file(parent->username + "/" + filename);
    connection->send(Message::Type::MODTIME, to_string(file.modification_time()));
    Message msg_action = connection->receive(Message::type_action());
    if (msg_action.type == Message::Type::UPLOAD)
    {
        log("Receive " + filename + "...");
        parent->receive_upload(filename, connection);
        log("ok");
    }
    else if (msg_action.type == Message::Type::DOWNLOAD)
    {
        log("Send " + filename + "...");
        parent->send_download(filename, connection);
        log("ok");
    }
    files_not_synced.remove(filename);
}

void ServerSync::delete_file(string filename)
{
    log("Delete " + filename);
    if (can_be_transfered(filename))
    {
        connection->send(Message::Type::OK);
        string filepath = parent->username + "/" + filename;
        remove(filepath.c_str());
        for (auto const &thread_map : parent->server->threads)
        {
            ServerThread *thread = thread_map.second;
            if (thread->username != parent->username)
            {
                thread->server_sync->files_to_delete.push_back(filename);
            }
        }
        log("ok");
        unlock_file(filename);
        files_not_synced.remove(filename);
    }
    else
    {
        log("Can't delete right now");
        connection->send(Message::Type::ERROR);
    }
}

void ServerSync::send_files_to_client()
{
    for (string &filename : files_to_delete)
    {
        log("Delete " + filename + "...");
        connection->send(Message::Type::DELETE, filename);
        log("ok");
    }
    files_to_delete.clear();
    for (string &filename : files_not_synced)
    {
        log("Send " + filename + "...");
        connection->send(Message::Type::SYNC, filename);
        connection->send_file(parent->username + "/" + filename);
        log("ok");
    }
    connection->send(Message::Type::DONE);
}