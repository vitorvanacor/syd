#include "ServerSync.hpp"

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
            list<string> expected_types;
            expected_types.push_back(Message::T_STAT);
            expected_types.push_back(Message::T_DONE);
            Message msg = connection->receive(expected_types);

            // Check file timestamp
            if (msg.type == Message::T_STAT)
            {
                connection->send_ack();

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

                // Gets timestamp from the file stored at server
                int timestamp_local = get_filetimestamp(connection->user_directory + "/" + "menes.txt");

                // Compares timestamp
                if (timestamp_remote < timestamp_local)
                {
                    debug("Client needs to download");

                    try
                    {
                        if (!ifstream(connection->user_directory + '/' + filename))
                        {
                            cout << "Error opening file " << filename << " at " << connection->user_directory << endl;
                            connection->send(Message::T_ERROR);
                            connection->receive_ack();
                        }
                        connection->send(Message::T_SOF);
                        connection->receive_ack();

                        connection->send_file(connection->user_directory + "/" + filename);
                    }
                    catch (exception &e)
                    {
                        cout << e.what() << endl;
                        continue;
                    }
                }
                else if (timestamp_remote > timestamp_local || timestamp_local == -1)
                {

                    debug("Client needs to upload");

                    connection->send(Message::T_UPLOAD);
                    connection->receive_ack();

                    debug("Uploading " + filename + " ...");
                    connection->receive_file(connection->user_directory + "/" + filename);
                    debug(filename + " uploaded.");
                }
                else
                {
                    debug("Same file.");

                    connection->send(Message::T_EQUAL);
                    connection->receive_ack();
                }
            }
            // Sync finished
            else if (msg.type == Message::T_DONE)
            {
            }
        }
    }
}