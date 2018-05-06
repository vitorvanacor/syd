#include "Connection.hpp"

#include <iostream>

#include "sydUtil.h"

using namespace std;

/* Client connection */
Connection::Connection(string username, string hostname, int port)
{ 
    this->session = to_string(rand()%10000);
    this->username = username;
    this->sock = new Socket(port);
    debug("Creating session "+session, __FILE__);
    init_sequences();
    
    this->sock->set_host(hostname);
    send(Message::T_SYN, username);
    receive_ack();
    sock->set_dest_address(sock->get_sender_address());
    send_ack();
    cout << "Successfully logged in as " << username << "!" << endl;
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
    debug("Closing connection " + session);
    delete this->sock;
}

void Connection::accept_connection()
{
    last_sequence_received = 0;
    send_ack();
    receive_ack();
    debug("Connection "+session+" established");
}

void Connection::send(string type, string content)
{
    // if content too big, break it and call smaller sends
    last_sequence_sent += 1;
    Message msg = Message(session, last_sequence_sent, type, content);
    msg.print('>', username);
    sock->send(msg.to_string());
    messages_sent[last_sequence_sent] = msg.to_string();
}

void Connection::send_ack()
{
    send(Message::T_ACK, to_string(last_sequence_received));
}

void Connection::resend()
{
    for (map<int,string>::iterator it = messages_sent.begin(); it != messages_sent.end(); ++it)
    {
        debug("Resending message "+to_string(it->first), __FILE__);
        sock->send(string(it->second));
    }
}

void Connection::receive_ack()
{
    debug("Waiting for ACK "+to_string(last_sequence_sent)+"...");
    while (true)
    {
        Message msg = receive();
        {
            // TODO: consider that error or bye can be received too
            if (msg.type == Message::T_ACK && stoi(msg.content) == last_sequence_sent)
            {
                last_sequence_received = msg.sequence;
                messages_sent.clear();
                return;
            }
        }
    }
}

Message Connection::receive(string expected_type)
{
    while (true)
    {
        // TODO: consider that error or bye can be received too
        Message msg = receive();
        if (msg.type == expected_type)
        {
            last_sequence_received = msg.sequence;
            return msg;
        }
    }
}

Message Connection::receive_request()
{
    debug("Waiting request from "+username+"...",__FILE__);
    while (true)
    {
        Message msg = receive();
        if (msg.is_request())
        {
            last_sequence_received = msg.sequence;
            return msg;
        }
    }
}

Message Connection::receive()
{
    while (true)
    {
        Message msg = Message::parse(sock->receive());
        msg.print('<', username);
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

void Connection::init_sequences()
{
    last_sequence_sent = -1;
    last_sequence_received = -1;
}
