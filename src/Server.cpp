#include "Server.hpp"

Server::Server(int port)
{
    sock = new Socket(port);
    this->port = port;
}

void Server::start()
{
    listener = new Connection(port);
    sock->bind_server();
    cout << "Listening on port " << port << " for connections..." << endl;
    while (true)
    {
        debug("Waiting for connection", __FILE__, __LINE__, Color::RED);
        Connection *connection = listener->receive_connection();

        Message r_msg = Message::parse(sock->receive());
        if (r_msg.type == Message::Type::BACKUP)
        {
            Socket backup_socket = Socket(port);
            backup_socket.set_host(r_msg.content);
            Server::backups[r_msg.content] = backup_socket;
        }

        delete_closed_threads();
        if (!threads.count(connection->session)) // If session doesn't exist
        {
            ServerThread *new_thread = new ServerThread(this, connection);
            new_thread->start();
            threads[connection->session] = new_thread;
        }
    }
}

void Server::backup(string host_master)
{
    sock->set_host(host_master);

    Message msg = Message(NULL, NULL, Message::Type::BACKUP, "meu ip");
    sock->send(msg.stringify());

    sock->bind_server();
    while(true)
    {
        Message r_msg = Message::parse(sock->receive());
        
        if (r_msg.type == Message::Type::UPLOAD)
        {
            listener = new Connection(host_master, port);
            listener->send(Message::Type::DOWNLOAD, r_msg.content);
            listener->receive(Message::Type::OK);
            listener->receive_file(dirpath + "/" + filename);
            delete listener;
        }
        else if (r_msg.type == Message::Type::IP)
        {
            Server::client_ips.push_back(r_msg.content);
        }
        else if (r_msg.type == Message::Type::DELETE)
        {

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
    Server* server = new Server(port);
    server->start();
}
