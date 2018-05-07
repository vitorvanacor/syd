#include <string>
#include <iostream>
#include <vector>
#include <list>

#include <unistd.h>
#include <map>
unsigned int microseconds = 1000000;

#include "Socket.hpp"
#include "Message.hpp"
#include "ServerThread.hpp"
#include "sydUtil.h"

using namespace std;

void free_closed_threads(map<string,ServerThread*> threads)
{
    auto it = threads.cbegin();
    while (it != threads.cend())
    {
        if (!it->second->is_open)
        {
            delete it->second;
            it = threads.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

int main(int argc, char *argv[])
{
    int port = DEFAULT_PORT;
    map<string,ServerThread*> threads;
    Socket listener(DEFAULT_PORT);
    listener.bind_server();
    cout << "Listening on port " << port << " for connections..." << endl;
    while (true)
    {
        Message msg = Message::parse(listener.receive());
        free_closed_threads(threads);
        msg.print('<');
        if (msg.type == Message::T_SYN)
        {
            if (!threads.count(msg.session))
            {
                Socket* new_socket = new Socket(port);
                new_socket->set_dest_address(listener.get_sender_address());
                ServerThread* new_thread = new ServerThread(msg.content, msg.session, new_socket);
                new_thread->run();
                threads[msg.session] = new_thread;
            }
            else
            {
                debug("Session " + msg.session + " already exists");
            }
        }
        else
        {
            debug("Message of type " + msg.type + " (not SYN) received");
        }
    }
}
