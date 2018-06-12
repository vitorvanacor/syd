#include "ServerThread.hpp"

#include "ServerSync.hpp"

ServerThread::ServerThread(Connection *connection, map<string, ServerThread *> threads, map<string, ServerThread *> sync_threads)
{
    is_open = true;
    threads = threads;
    sync_threads = sync_threads;
    this->connection = connection;
}

ServerThread::~ServerThread()
{
    delete connection;
}

void *ServerThread::run()
{
    connection->accept_connection();
    ServerSync server_sync(connection, sync_threads);
    server_sync.start();
    cout << connection->username << " logged in" << endl;
    while (true)
    {
        Message request = connection->receive_request();
        if (request.type == Message::T_LS)
        {
            connection->send_string(connection->list_server_dir(connection->user_directory));
            connection->receive_ack();
        }
        else if (request.type == Message::T_UPLOAD)
        {
            string filename = request.content;
            string filepath = connection->user_directory + '/' + filename;
            if (!can_be_transfered(request.content))
            {
                cout << "Error: File " << filename << " already being transfered" << endl;
                connection->send_ack(false);
                connection->receive_ack();
                continue;
            }
            connection->send_ack();
            cout << connection->username << " is uploading " << filename << "..." << endl;
            connection->receive_file(filepath);
            unlock_file(filename);
            cout << connection->username << " uploaded " << filename << endl;
        }
        else if (request.type == Message::T_DOWNLOAD)
        {
            string filename = request.content;
            string filepath = connection->user_directory + '/' + filename;
            if (!can_be_transfered(filename))
            {
                cout << "Error: File " << filename << " already being transfered" << endl;
                connection->send_ack(false);
                connection->receive_ack();
                cout << "Continuing" << endl;
                continue;
            }
            connection->send_ack();
            try
            {
                if (!ifstream(filepath))
                {
                    cout << "Error opening file " << request.content << " at " << connection->user_directory << endl;

                    connection->send_ack(false);
                    connection->receive_ack();
                    unlock_file(filename);
                    continue;
                }
                int timestamp = get_filetimestamp(filepath);

                connection->send(Message::T_SOF, to_string(timestamp));
                connection->receive_ack();

                cout << connection->username << " is downloading " << request.content << "..." << endl;
                connection->send_file(filepath);
                unlock_file(filename);
                cout << connection->username << " downloaded " << request.content << endl;
            }
            catch (exception &e)
            {
                connection->send_ack(false);
                connection->receive_ack();

                cout << e.what() << endl;
                unlock_file(filename);
                continue;
            }
            unlock_file(filename);
        }
        else if (request.type == Message::T_BYE)
        {
            connection->send_ack();
            connection->receive_ack();
            break;
        }
    }
    cout << "User " << connection->username << " logged out." << endl;
    is_open = false;
    return NULL;
}
