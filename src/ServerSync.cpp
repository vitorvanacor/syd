#include "ServerSync.hpp"

#include <string>

ServerSync::ServerSync(Connection *connection)
{
    string new_session;
    while (true)
    {
        string msg_s = connection->sock->receive();
        Message msg = Message::parse(msg_s);
        if (msg.type == Message::T_SYN && msg.session != connection->session)
        {
            new_session = msg.session;
            break;
        }
    }
    this->connection = new Connection(connection->username,
                                      new_session,
                                      connection->sock->get_answerer());
}

ServerSync::~ServerSync()
{
    delete connection;
}

void *ServerSync::run()
{
    connection->accept_connection();
    while (true)
    {
        Message msg = connection->receive(Message::T_SYNC);
        bool receiving_stats = true;
        connection->send_ack();

        while (receiving_stats)
        {
            Message msg = connection->receive(Message::T_STAT, Message::T_DONE);
            // Check file time stamp
            if (msg.type == Message::T_STAT)
            {
                // Parses content:
                // Find position of separators
                int timestamp_sep = msg.content.find('|');

                // Calculate sections length
                int content_len = msg.content.size();
                int timestamp_len = timestamp_sep;
                int filename_len = content_len - timestamp_len - 1;

                // Assign
                int timestamp_remote = stoi(msg.content.substr(0, timestamp_len));
                string filename = msg.content.substr(timestamp_len + 1, content_len);

                cout << "TIMESTAMP_CLIENT" << timestamp_remote << endl;

                // Gets timestamp from the file stored at server
                int timestamp_local = get_filetimestamp(connection->user_directory + "/" + "menes.txt");
                cout << "TIMESTAMP_SERVER: " << timestamp_local << endl;

                // Compares timestamp
                // Client needs to download
                if (timestamp_remote < timestamp_local)
                    cout << "Client needs to download" << endl;
                // Client need to upload
                else if (timestamp_remote > timestamp_local)
                    cout << "Client needs to upload" << endl;
                else
                    cout << "Same file." << endl;
            }
            // Sync finished
            else if (msg.type == Message::T_DONE)
            {
            }
        }
    }
}