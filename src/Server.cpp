#include "Server.hpp"

void Server::master_loop(int port, list<string> client_ips)
{
    listener = new Connection(port);
    listener->sock->set_timeout(3);
    listener->resend_on_timeout = false;

    notify_clients(client_ips);

    cout << "Listening on port " << port << " for connections..." << endl;
    while (true)
    {
        debug("Waiting for connection", __FILE__, __LINE__, Color::RED);
        try
        {
            Connection *connection = listener->receive_connection();

            delete_closed_threads();

            if (connection->session.at(0) == 'B') // If it's a backup connection
            {
                cout << "Backup " << connection->ip << " connected" << endl;
                connection->confirm();
                backups.push_back(connection);
            }
            else if (!threads.count(connection->session)) // If new client connection
            {
                ServerThread *new_thread = new ServerThread(this, connection);
                new_thread->start();
                threads[connection->session] = new_thread;
                cout << "Connecttion ip " << connection->ip << endl;
                for (Connection *backup : backups)
                {
               
                    backup->send(Message::Type::CLIENT_CONNECT, connection->ip);
                }
            }
        }
        catch (timeout_exception &e)
        {
            cout << "SENDING HEARTBEAT" << endl;
            for (Connection *backup : backups)
            {
                backup->send(Message::Type::HEARTBEAT);
            }
        }
    }
}

void Server::notify_clients(list<string> client_ips)
{
    Socket *sock = new Socket(4002);
    string my_ip = get_ip();
    for (string &client_ip : client_ips)
    {
        sock->set_host(client_ip);
        sock->send(my_ip);
    }
}

void Server::backup_loop(string master_ip, int port)
{
    list<string> client_ips;
    listener = new Connection(master_ip, port, true);
    listener->resend_on_timeout = false;
    bool is_backup = true;
    while (is_backup)
    {
        try
        {
            listener->sock->set_timeout(5);
            Message msg = listener->receive(Message::type_backup());

            if (msg.type == Message::Type::UPLOAD)
            {
                string filepath = msg.content;
                cout << "Backing up " << filepath << "...";
                listener->receive_file(filepath);
                cout << " Ok!" << endl;
            }
            else if (msg.type == Message::Type::DELETE)
            {
                string filepath = msg.content;
                cout << "Deleting " << filepath;
                remove(filepath.c_str());
                cout << " Ok!" << endl;
            }
            else if (msg.type == Message::Type::CLIENT_CONNECT)
            {
                cout << "Backing up connection with " << msg.content << endl;
                client_ips.push_back(msg.content);
            }
            else if (msg.type == Message::Type::CLIENT_DISCONNECT)
            {
                cout << msg.content << " disconnected" << endl;
                client_ips.remove(msg.content);
            }
            else if (msg.type == Message::Type::LOGIN)
            {
                File::create_directory(msg.content);
                listener->receive_ack();
            }
            else if (msg.type == Message::Type::HEARTBEAT)
            {
                cout << "recebeu HEARTBEAT" << endl;
            }
        }
        catch (timeout_exception &e)
        {
            string new_master = election();
            if (new_master.empty()) // I am master
            {
                is_backup = false;
            }
            else
            {
                delete listener;
                listener = new Connection(new_master, port, true);
            }
        }
    }
    if (!is_backup)
    {
        master_loop(port, client_ips);
    }
}

string Server::election()
{
    Socket *sock = new Socket(4001);
    sock->bind_server();
    sock->set_timeout(7);
    string my_ip = get_ip();
    for (string &ip : ip_list())
    {
        if (ip > my_ip)
        {
            sock->set_host(ip);
            sock->send("ELECTION");
        }
    }
    while (true)
    {
        try
        {
            string msg = sock->receive();
            if (msg == "ANSWER")
            {
                continue;
            }
            else if (msg == "ELECTION")
            {
                sock->set_to_answer(sock);
                sock->send("ANSWER");
            }
            else
            {
                // Se receber um IP, é o do novo coordinator
                return msg;
            }
        }
        catch (timeout_exception &e)
        {
            return "";
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
