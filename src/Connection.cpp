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
    debug("Creating session "+this->session, __FILE__);
    init_sequences();
    
    this->sock->set_host(hostname);
    this->sock->set_timeout(TIMEOUT_IN_SECONDS);
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
    this->sock->set_timeout(TIMEOUT_IN_SECONDS);
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

void Connection::send_file(char* fileByteArray, int fileLength)
{
    // Get byte array of file
    //int fileLength = file.GetLength();
    //char byteArray[fileLength];
    //file.FileToByteArray(byteArray);
    //cout << byteArray;

    if(fileLength > PACKET_SIZE) 
    {
        char buffer[PACKET_SIZE];

        // Break file
        for(int i=0; i<fileLength; i += PACKET_SIZE) 
        {
            // Fill packet
            for(int j=0; j<PACKET_SIZE; j++)
                buffer[i] = fileByteArray[i];

            send(Message::T_DATA, buffer);
        }

        // A little packet is missing
        int sizeOfLittlePacket = fileLength % PACKET_SIZE;
        if(sizeOfLittlePacket != 0){
            char buffer[sizeOfLittlePacket];

            // Fill packet
            for(int i=0; i<sizeOfLittlePacket; i++)
               buffer[i] = fileByteArray[i];

            send(Message::T_DATA, buffer); 
        }
    } else
        send(Message::T_DATA, fileByteArray);        
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
    sock->set_timeout(TIMEOUT_IN_SECONDS);
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
    debug("Waiting for "+expected_type+"...", __FILE__);
    sock->set_timeout(TIMEOUT_IN_SECONDS);
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
    this->sock->set_timeout(0); // Never timeout
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
        try
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
        catch (timeout_exception& e)
        {
            resend();
        }
    }
}

void Connection::receive_file() {
    debug("Waiting for data!");
    sock->set_timeout(TIMEOUT_IN_SECONDS);
    while (true)
    {
        Message msg = receive();
        {
            // TODO: consider that error or bye can be received too
            if (msg.type == Message::T_DATA)
            {
                cout << "Content: " << msg.content << endl;
                return;
            }
        }
    }
}

void Connection::init_sequences()
{
    last_sequence_sent = -1;
    last_sequence_received = -1;
}
