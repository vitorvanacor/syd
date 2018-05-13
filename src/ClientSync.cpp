#include "ClientSync.hpp"

ClientSync::ClientSync(Connection* _connection)
{
    this->connection = new Connection(_connection->username);
    this->connection->sock = _connection->sock->get_answerer();
    this->connection->connect();
    this->start();
}

ClientSync::~ClientSync()
{
    delete connection;
}

void* ClientSync::run()
{
    while (true)
    {
        debug("RUNNING CLIENT SYNC",__FILE__,__LINE__,Color::GREEN);
        sleep(2);
        /*
        foreach (file in connection->user_dir)
        {
            sync_file(file, mod_time);
        }
        connection->send(Message::T_DONE);
        while (connection->receive() != Message::T_DONE)
        {
            download
        }
        */
    }
}