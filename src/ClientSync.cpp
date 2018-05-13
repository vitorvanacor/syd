#include "ClientSync.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

ClientSync::ClientSync(Connection *_connection)
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

void *ClientSync::run()
{
    while (true)
    {
        ///debug("RUNNING CLIENT SYNC", __FILE__, __LINE__, Color::GREEN);

        //foreach (file in connection->user_dir)
        //{

        connection->send(Message::T_SYNC);
        connection->receive_ack();

        int timestamp = get_filetimestamp("menes.txt");
        if (timestamp != -1)
        {
            string content = to_string(timestamp) + '|' + "menes.txt";
            connection->send(Message::T_STAT, content);
            connection->receive_ack();
        }

        //}
        //connection->send(Message::T_DONE);
        //while (connection->receive() != Message::T_DONE)
        //{
        //download
        //}

        sleep(30);
    }
}