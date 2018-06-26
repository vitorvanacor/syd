#include "Server.hpp"

void Server::master_loop(int port)
{
    listener = new Connection(port);
    cout << "Listening on port " << port << " for connections..." << endl;
    while (true)
    {
        debug("Waiting for connection", __FILE__, __LINE__, Color::RED);
        Connection *connection = listener->receive_connection();

        delete_closed_threads();

        if (connection->session.at(0) == 'B') // If it's a backup connection
        {
            backups.push_back(connection);
        }
        else if (!threads.count(connection->session)) // If new client connection
        {
            ServerThread *new_thread = new ServerThread(this, connection);
            new_thread->start();
            threads[connection->session] = new_thread;
            for (Connection* backup : backups)
            {
                backup->send(Message::Type::CLIENT_CONNECT, connection->ip);
            }
        }
    }
}

void Server::backup_loop(string master_ip, int port)
{
    listener = new Connection(master_ip, port, true);
    while (true)
    {
        Message msg = listener->receive(Message::type_backup());

        if (msg.type == Message::Type::UPLOAD)
        {
            string filepath = msg.content;
            listener->receive_file(filepath);
        }
        else if (msg.type == Message::Type::DELETE)
        {
            string filepath = msg.content;
            remove(filepath.c_str());
        }
        else if (msg.type == Message::Type::CLIENT_CONNECT)
        {
            threads[msg.content] = NULL;
        }
        else if (msg.type == Message::Type::CLIENT_DISCONNECT)
        {
            threads.erase(msg.content);
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
    string master = "";
    string my_ip = get_ip();
    for (string &ip : ip_list())
    {
        if (ip > my_ip)
        {
            master = ip;
            break;
        }
    }
    Server *server = new Server();
    if (master.empty())
    {
        server->master_loop(port);
    }
    else
    {
        server->backup_loop(master, port);
    }
}
