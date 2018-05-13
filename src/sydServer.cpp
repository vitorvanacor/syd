#include "ServerThread.hpp"
#include "sydUtil.h"

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
    if (argc > 1)
    {
        port = atoi(argv[1]);
    }

    map<string,ServerThread*> threads;
    Socket listener(port);
    listener.bind_server();
    cout << "Listening on port " << port << " for connections..." << endl;
    while (true)
    {
        debug("Waiting for connection", __FILE__, __LINE__, Color::RED);
        Message msg = Message::parse(listener.receive());
        free_closed_threads(threads);
        msg.print('<');
        if (msg.type == Message::T_SYN)
        {
            if (!threads.count(msg.session)) // If session does not already exists
            {
                Connection* connection = new Connection(msg.content, msg.session, listener.get_answerer());
                ServerThread* new_thread = new ServerThread(connection);
                new_thread->start();
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
