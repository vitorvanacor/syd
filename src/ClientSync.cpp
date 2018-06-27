#include "ClientSync.hpp"

ClientSync::ClientSync(Client *client) : Thread()
{
    this->client = client;
    log_file = client->user_dir + "/sync_log";
    filenames = File::list_directory_filenames(client->user_dir);
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
        filenames = File::list_directory_filenames(client->user_dir);
        sleep(5);
    }
}

void ClientSync::sync_own_files()
{
    for (File &file : File::list_directory(client->user_dir))
    {
        sync_file(file);
        filenames.remove(file.name());
    }
    for (string &filename : filenames)
    {
        log("Delete " + filename + "...");
        bool delete_ok = false;
        while (!delete_ok)
        {
            connection->send(Message::Type::DELETE, filename);
            delete_ok = true;
            try
            {
                connection->receive(Message::Type::OK);
            }
            catch (ResponseException &e)
            {
                log("Delete failed, trying again");
                sleep(2);
                delete_ok = false;
            }
        }
        log("ok");
    }
    connection->send(Message::Type::DONE);
}

void ClientSync::sync_file(File file)
{
    connection->send(Message::Type::SYNC, file.name());
    int modtime = stoi(connection->receive_content(Message::Type::MODTIME));
    log(file.name() + ": " + time_to_string(file.modification_time()) + " | " + time_to_string(modtime));
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
        else if (msg.type == Message::Type::SYNC)
        {
            string filename = msg.content;
            log("Download " + filename + "...");
            connection->receive_file(client->user_dir + "/" + filename);
            log("ok");
        }
        else if (msg.type == Message::Type::DELETE)
        {
            string filepath = client->user_dir + "/" + msg.content;
            remove(filepath.c_str());
        }
    }
}