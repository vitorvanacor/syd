#include "ClientSync.hpp"

#include <sys/types.h>
#include <sys/stat.h>

ClientSync::ClientSync(Client *client)
{
    this->client = client;
    this->connection = client->connection->receive_connection();
}

ClientSync::~ClientSync()
{
    delete connection;
}

void *ClientSync::run()
{
    connection->confirm();
    while (true)
    {
        for (File &file : File::list_directory(client->user_dir))
        {
            sync_file(file);
        }
        connection->send(Message::Type::DONE);
        while (true)
        {
            Message msg = connection->receive(Message::type_action());
            if (msg.type == Message::Type::DONE)
            {
                break;
            }
            string filename = msg.content;
            client->download_file(filename, client->user_dir, connection);
        }
        sleep(5);
    }
}

void ClientSync::sync_file(File file)
{
    connection->send(Message::Type::SYNC, file.name());
    int modtime = stoi(connection->receive_content(Message::Type::MODTIME));
    if (modtime < file.modification_time())
    {
        client->upload_file(file.name(), client->user_dir, connection);
    }
    else if (modtime > file.modification_time())
    {
        client->download_file(file.name(), client->user_dir, connection);
    }
    else
    {
        connection->send(Message::Type::DONE);
    }
}