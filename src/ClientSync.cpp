#include "ClientSync.hpp"

#include <sys/types.h>
#include <sys/stat.h>

ClientSync::ClientSync(Client *client) : Thread()
{
    this->client = client;
    log_file = client->user_dir + "/sync_log";
}

ClientSync::~ClientSync()
{
    delete connection;
}

void *ClientSync::run()
{
    connection = client->connection->create_connection();
    while (true)
    {
        log("--- Begin Sync");
        sync_own_files();
        log("--- Receive from server");
        receive_files_from_server();
        log("--- Sync Done");
        sleep(5);
    }
}

void ClientSync::sync_own_files()
{
    for (File &file : File::list_directory(client->user_dir))
    {
        sync_file(file);
    }
    connection->send(Message::Type::DONE);
}

void ClientSync::sync_file(File file)
{
    connection->send(Message::Type::SYNC, file.name());
    int modtime = stoi(connection->receive_content(Message::Type::MODTIME));
    log(file.name()+": "+time_to_string(file.modification_time())+" | "+time_to_string(modtime));
    if (modtime < file.modification_time())
    {
        log("Upload " + file.name() + "...");
        client->upload_file(file.name(), client->user_dir, connection);
        log("ok");
    }
    else if (modtime > file.modification_time())
    {
        log("Download " + file.name() + "...");
        client->download_file(file.name(), client->user_dir, connection);
        log("ok");
    }
    else
    {
        connection->send(Message::Type::DONE);
    }
}

void ClientSync::receive_files_from_server()
{
    while (true)
    {
        Message msg = connection->receive(Message::type_sync());
        if (msg.type == Message::Type::DONE)
        {
            break;
        }
        string filename = msg.content;
        log("Download " + filename + "...");
        connection->receive_file(client->user_dir + "/" + filename);
        log("ok");
    }
}