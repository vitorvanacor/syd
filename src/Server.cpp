#include "Server.hpp"

void Server::start(int port)
{
    Socket listener(port);
    listener.bind_server();
    cout << "Listening on port " << port << " for connections..." << endl;
    while (true)
    {
        receive_connection(listener);
    }
}

void Server::receive_connection(Socket listener)
{
    while(true)
    {
        debug("Waiting for connection", __FILE__, __LINE__, Color::RED);
        Message msg = Message::parse(listener.receive());
        delete_closed_threads();
        msg.print('<');
        if (msg.type == Message::T_SYN)
        {
            if (!threads.count(msg.session)) // If session does not already exists
            {
                Connection *connection = new Connection(msg.session, listener.get_answerer());
                ServerThread *new_thread = new ServerThread(connection);
                new_thread->start();
                threads[msg.session] = new_thread;
            }
        }
    }
}

void Server::delete_closed_threads()
{
    auto it = threads.cbegin();
    while (it != threads.cend())
    {
        ServerThread *serverThread = it->second;
        if (!serverThread->is_open)
        {
            delete serverThread;
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
    Server server;
    server.start(port);
}
