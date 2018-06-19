#include "Server.hpp"

void Server::start(int port)
{
    Connection *listener = Connection::listener(port);
    cout << "Listening on port " << port << " for connections..." << endl;
    while (true)
    {
        debug("Waiting for connection", __FILE__, __LINE__, Color::RED);
        Connection *connection = listener->receive_connection();
        delete_closed_threads();
        if (!threads.count(connection->session)) // If session doesn't exist
        {
            ServerThread *new_thread = new ServerThread(connection);
            new_thread->start();
            threads[connection->session] = new_thread;
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
