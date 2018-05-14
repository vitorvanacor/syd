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

        list<string> files_to_update = File::list_filename(connection->user_directory);

        Message msg = connection->receive(Message::T_SYNC);
        bool receiving_stats = true;
        connection->send_ack();

        list<string> expected_types;
        expected_types.push_back(Message::T_STAT);
        expected_types.push_back(Message::T_DONE);

        while (receiving_stats)
        {
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

                string filepath = connection->user_directory + '/' + filename;

                // Gets timestamp from the file stored at server
                int timestamp_local = get_filetimestamp(filepath);

                // Compares timestamp
                if (timestamp_remote < timestamp_local)
                {
                    cout << "Client needs to download" << endl;
                    try
                    {
                        if (!ifstream(filepath))
                        {
                            cout << "Error opening file " << filename << " at " << connection->user_directory << endl;
                            connection->send(Message::T_ERROR);
                            connection->receive_ack();
                        }
                        connection->send(Message::T_DOWNLOAD);
                        connection->receive_ack();

                        int timestamp = get_filetimestamp(filepath);

                        connection->send(Message::T_SOF, to_string(timestamp));
                        connection->receive_ack();

                        if (connection->send_file(filepath) == 0)
                            debug(filename + " downloaded successfully.");
                        else
                            debug(filename + " download failed.");
                    }
                    catch (exception &e)
                    {
                        cout << e.what() << endl;
                        continue;
                    }
                }
                else if (timestamp_remote > timestamp_local || timestamp_local == -1)
                {
                    cout << "Client needs to upload" << endl;
                    connection->send(Message::T_UPLOAD);
                    connection->receive_ack();

                    debug("Uploading " + filename + " ...");
                    if (connection->receive_file(filepath) == 0)
                        debug(filename + " uploaded successfully.");
                    else
                        debug(filename + " upload failed.");
                }
                else
                {
                    cout << "Same file." << endl;
                    connection->send(Message::T_EQUAL);
                    connection->receive_ack();
                }

                // Update list of files that need update
                files_to_update.remove(filename);
            }
            // Sync finished
            else if (msg.type == Message::T_DONE)
            {
                connection->send_ack();
                if (files_to_update.size() == 0)
                {
                    connection->send(Message::T_DONE);
                    connection->receive_ack();
                }
                else
                {
                    for (list<string>::iterator it = files_to_update.begin(); it != files_to_update.end(); ++it)
                    {
                        cout << "CLIENT DOESNT HAVE: " << *it << endl;
                        try
                        {
                            string filename = *it;
                            string filepath = connection->user_directory + '/' + filename;

                            if (!ifstream(filepath))
                            {
                                cout << "Error opening file " << *it << " at " << connection->user_directory << endl;
                                connection->send(Message::T_ERROR);
                                connection->receive_ack();
                            }
                            connection->send(Message::T_DOWNLOAD, filename);
                            connection->receive_ack();

                            int timestamp = get_filetimestamp(filepath);

                            connection->send(Message::T_SOF, to_string(timestamp));
                            connection->receive_ack();

                            if (connection->send_file(filepath) == 0)
                                debug(*it + " downloaded successfully.");
                            else
                                debug(*it + " download failed.");
                        }
                        catch (exception &e)
                        {
                            cout << e.what() << endl;
                            continue;
                        }
                    }

                    connection->send(Message::T_DONE);
                    connection->receive_ack();
                    break;
                }
            }
        }
    }
}