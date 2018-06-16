#include "ServerSync.hpp"
#include "ServerThread.hpp"

ServerSync::ServerSync(Connection *connection, map<string, ServerSync *> *sync_threads_pointer)
{
    sync_threads = sync_threads_pointer;
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
        debug("RECEIVE SYNC", __FILE__, __LINE__, Color::BLUE);
        list<string> files_to_update = File::list_filename(connection->user_directory);

        Message msg = connection->receive(Message::T_SYNC);
        debug("SYNC RECEIVED", __FILE__, __LINE__, Color::GREEN);
        bool receiving_stats = true;
        connection->send_ack();

        list<string> expected_types;
        expected_types.push_back(Message::T_STAT);
        expected_types.push_back(Message::T_DONE);
        expected_types.push_back(Message::T_DEL);

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

                if (!can_be_transfered(filename))
                {
                    cout << filename << " currently being transfered" << endl;
                    connection->send_ack(false);
                    connection->receive_ack();
                    continue;
                }

                // Gets timestamp from the file stored at server
                int timestamp_local = get_filetimestamp(filepath);

                // Compares timestamp
                if (timestamp_remote < timestamp_local)
                {
                    cout << connection->username << " needs to download " << filename << endl;
                    try
                    {
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
                        unlock_file(filename);
                        continue;
                    }
                }
                else if (timestamp_remote > timestamp_local || timestamp_local == -1)
                {
                    cout << connection->username << " needs to upload " << filename << endl;
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
                    connection->send(Message::T_EQUAL);
                    connection->receive_ack();
                }

                // Update list of files that need update
                files_to_update.remove(filename);
                unlock_file(filename);
            }
            // Sync finished
            else if (msg.type == Message::T_DONE)
            {
                connection->send_ack();
                if (files_to_update.size() == 0)
                {
                    connection->send(Message::T_DONE);
                    debug("WAITING LAST ACK", __FILE__, __LINE__, Color::MAGENTA);
                    connection->receive_ack();
                    debug("RECEIVED LAST ACK", __FILE__, __LINE__, Color::RED);
                    break;
                }
                else
                {
                    for (list<string>::iterator it = files_to_update.begin(); it != files_to_update.end(); ++it)
                    {
                        try
                        {
                            string filename = *it;
                            string filepath = connection->user_directory + '/' + filename;
                            cout << connection->username << " needs to download " << filename << endl;
                            if (!ifstream(filepath))
                            {
                                cout << "Error opening file " << *it << " at " << connection->user_directory << endl;
                                continue;
                            }
                            connection->send(Message::T_DOWNLOAD, filename);
                            bool ok = connection->receive_ack();
                            if (ok)
                            {
                                int timestamp = get_filetimestamp(filepath);

                                connection->send(Message::T_SOF, to_string(timestamp));
                                connection->receive_ack();

                                if (connection->send_file(filepath) == 0)
                                    debug(*it + " downloaded successfully.");
                                else
                                    debug(*it + " download failed.");
                            }
                            else
                            {
                                cout << "User cant receive " << filename << endl;
                                connection->send_ack();
                            }
                        }
                        catch (exception &e)
                        {
                            cout << e.what() << endl;
                            continue;
                        }
                    }

                    connection->send(Message::T_DONE);
                    debug("WAITING LAST ACK", __FILE__, __LINE__, Color::MAGENTA);
                    connection->receive_ack();
                    debug("RECEIVED LAST ACK", __FILE__, __LINE__, Color::RED);
                    break;
                }
            }
            else if (msg.type == Message::T_DEL)
            {
                string filename = msg.content;
                string filepath = connection->user_directory + '/' + filename;
                if (File::delete_file(filepath) == 0)
                {

                    for (map<string, ServerSync *>::iterator it = (*sync_threads).begin(); it != (*sync_threads).end(); ++it)
                    {
                        ServerSync *thread = it->second;

                        if (thread->connection->username != this->connection->username && thread->connection->session != this->connection->session)
                        {
                            thread->connection->send(Message::T_DEL, filename);
                        }
                    }
                }

                else
                    cout << "Delete file failed!" << endl;

                files_to_update.remove(filename);
                connection->send_ack();
            }
        }
    }
}