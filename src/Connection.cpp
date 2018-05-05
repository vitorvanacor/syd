#include "Connection.hpp"

#include <iostream>

#include "sydUtil.h"

using namespace std;

/* Client connection */
Connection::Connection(string username, string hostname, int port)
{ 
    this->session = to_string(rand()%10000);
    this->username = username;
    debug("Connecting " + username + " to " + hostname + " (session " + session + ")" );
    this->sock = new Socket(port);
    init_sequences();
    
    this->sock->set_host(hostname);
    send(Message::T_SYN, username);
    receive_ack();
    send_ack();
}

/* Server connection */
Connection::Connection(string username, string session, Socket* new_socket)
{
    debug("New Connection: " + username + " (" + session + ")");
    init_sequences();
    this->session = session;
    this->username = username;
    this->sock = new_socket;
}

Connection::~Connection()
{
    debug("Closing connection " + session + " (" + username + ")");
    delete this->sock;
}

void Connection::accept_connection()
{
    last_sequence_received = 0;
    send_ack();
    receive_ack();
}

void Connection::send(string type, string content)
{
    // if content too big, break it and call smaller sends
    int sequence = last_sequence_sent + 1;
    Message msg = Message(session, sequence, type, content);
    msg.print('>', username);
    sock->send(msg.to_string());
    unconfirmed_message = msg.to_string();
}

void Connection::send_ack()
{
    send(Message::T_ACK, to_string(last_sequence_received));
}

void Connection::resend()
{
    debug("Resending message "+to_string(last_sequence_sent));
    sock->send(unconfirmed_message);
}

Message Connection::receive()
{
    Message msg = Message::parse(sock->receive());
    msg.print('<', username);
    while (true)
    {
        if (msg.session == session)
        {
            if (msg.sequence == last_sequence_received + 1)
            {
                return msg;
            }
            else
            {
                resend();
            }
        }
        else
        {
            debug("Message received from wrong session");
        }
    }
}

void Connection::receive_ack()
{
    while (true)
    {
        Message msg = receive();
        {
            if (msg.type == Message::T_ACK && msg.content == to_string(last_sequence_sent))
            {
                last_sequence_received += 1;
            }
        }
    }
}

Message Connection::receive(string expected_type)
{
    while (true)
    {
        Message msg = receive();
        if (msg.type == expected_type)
        {
            return msg;
        }
    }
}

void Connection::init_sequences()
{
    last_sequence_sent = -1;
    last_sequence_confirmed = -1;
    last_sequence_received = -1;
}
