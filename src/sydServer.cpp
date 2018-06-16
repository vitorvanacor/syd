#include "ServerThread.hpp"
#include "ServerSync.hpp"
#include "sydUtil.h"

class sydServer
{
  public:
    sydServer()
    {
        threads = new map<string, ServerThread *>();
        sync_threads = new map<string, ServerSync *>();
    }
    ~sydServer() {}

    map<string, ServerThread *> *threads;
    map<string, ServerSync *> *sync_threads;

    void free_closed_threads()
    {
        auto it = (*threads).cbegin();
        while (it != (*threads).cend())
        {
            if (!it->second->is_open)
            {
                delete it->second;
                it = (*threads).erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
};

int main(int argc, char *argv[])
{

    sydServer *server = new sydServer();

    int port = DEFAULT_PORT;
    if (argc > 1)
    {
        port = atoi(argv[1]);
    }

    Socket listener(port);
    listener.bind_server();
    cout << "Listening on port " << port << " for connections..." << endl;
    while (true)
    {
        debug("Waiting for connection", __FILE__, __LINE__, Color::RED);
        Message msg = Message::parse(listener.receive());
        server->free_closed_threads();
        msg.print('<');
        if (msg.type == Message::T_SYN)
        {

            if (!(*server->threads).count(msg.session)) // If session does not already exists
            {
                Connection *connection = new Connection(msg.content, msg.session, listener.get_answerer());
                ServerThread *new_thread = new ServerThread(connection, server->threads, server->sync_threads);
                new_thread->start();
                (*server->threads)[msg.session] = new_thread;
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
