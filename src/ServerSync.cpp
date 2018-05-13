#include "ServerSync.hpp"

ServerSync::ServerSync(Connection* connection)
{
    string new_session;
    while (true)
    {
        string msg_s = connection->sock->receive();
        Message msg = Message::parse(msg_s);
        if (msg.type == Message::T_SYN && msg.session != connection->session)
        {
            new_session = msg.session;
            break;
        }
    }
    this->connection = new Connection(connection->username,
                                      new_session,
                                      connection->sock->get_answerer());
}

ServerSync::~ServerSync()
{
    delete connection;
}

void* ServerSync::run()
{
    connection->accept_connection();
    while (true)
    {
        debug("SERVER SYNC!");
        sleep(2);
    }
}