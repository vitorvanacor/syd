#include "ServerThread.hpp"

#include "sydUtil.h"

ServerThread::ServerThread(string username, string session, Socket* new_socket)
{
    connection = new Connection(username, session, new_socket);
}

void* ServerThread::run()
{
    connection->accept_connection();
    while (true)
    {
        debug("Waiting request from "+connection->username+"...",__FILE__);
        Message request = connection->receive();
        if (request.type == Message::T_LS)
        {
            connection->send(Message::T_LS, "file1.txt;2018-01-01 13:00:00|file2.txt;2018-02-01 15:00:00");
            connection->receive_ack();
        }
        else if (request.type == Message::T_UPLOAD)
        {
            connection->send_ack();
            //connection->receive_file(request.content);
        }
        else if (request.type == Message::T_DOWNLOAD)
        {
            //connection->send_file(request.content);
        }
        else if (request.type == Message::T_BYE)
        {
            connection->send_ack();
            connection->receive_ack();
            break;
        }
        else
        {
            debug("Message received ("+request.type+") is not a request");
        }
    }
    return NULL;
}
