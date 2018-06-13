#include "ClientSync.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

ClientSync::ClientSync(Connection *_connection)
{
    this->connection = new Connection(_connection->username);
    this->connection->sock = _connection->sock->get_answerer();
    this->connection->connect();
    this->start();
}

ClientSync::~ClientSync()
{
    delete connection;
}

void *ClientSync::run()
{

    list<string> list_filename;
    list<string> old_list_filename;

    while (true)
    {
        debug("INIT SYNC", __FILE__, __LINE__, Color::GREEN);
        connection->send(Message::T_SYNC);
        connection->receive_ack();

        list_filename = File::list_filename(connection->user_directory);

        for (list<string>::iterator it = list_filename.begin(); it != list_filename.end(); ++it)
        {
            string filename = *it;
            string filepath = connection->user_directory + '/' + filename;

            old_list_filename.remove(filename);

            if (can_be_transfered(filename))
            {
                int timestamp = get_filetimestamp(filepath);
                if (timestamp != -1)
                {
                    string content = to_string(timestamp) + '|' + filename;
                    connection->send(Message::T_STAT, content);
                    connection->receive_ack();
                    list<string> expected_types;
                    expected_types.push_back(Message::T_DOWNLOAD);
                    expected_types.push_back(Message::T_UPLOAD);
                    expected_types.push_back(Message::T_EQUAL);
                    expected_types.push_back(Message::T_ERROR);

                    Message msg = connection->receive(expected_types);

                    if (msg.type == Message::T_DOWNLOAD)
                    {
                        debug("Starting to download" + filename + " ...");
                        connection->send_ack();
                        if (connection->receive_file(filepath) == 0)
                            debug(filename + " downloaded successfully!");
                        else
                            debug(filename + " download failed!");
                    }
                    else if (msg.type == Message::T_UPLOAD)
                    {
                        debug("Starting to upload" + filename + " ...");
                        connection->send_ack();
                        try
                        {
                            if (!ifstream(filepath))
                            {
                                cout << "Error opening file " << filename << " at " << connection->user_directory << endl;

                                connection->send_ack(false);
                                connection->receive_ack();

                                unlock_file(filename);
                                continue;
                            }

                            int timestamp = get_filetimestamp(filepath);

                            connection->send(Message::T_SOF, to_string(timestamp));
                            connection->receive_ack();

                            debug("Starting to upload " + filename + " ...");
                            if (connection->send_file(filepath) == 0)
                                debug(filename + " uploaded successfully!");
                            else
                                debug(filename + " uploaded failed!");
                        }
                        catch (exception &e)
                        {
                            cout << e.what() << endl;

                            connection->send_ack(false);
                            connection->receive_ack();
                            unlock_file(filename);
                            continue;
                        }
                    }
                    else if (msg.type == Message::T_EQUAL)
                    {
                        debug("File is already up-to-date.");
                        connection->send_ack();
                        unlock_file(filename);
                        continue;
                    }
                    else
                    {
                        debug("File currently being transfered");
                        connection->send_ack();
                        unlock_file(filename);
                        continue;
                    }
                }
                else
                {
                    debug("Error generating timestamp!");
                    unlock_file(filename);
                    continue;
                }
                unlock_file(filename);
            }
        }

        list<string> expected_types;
        expected_types.push_back(Message::T_DONE);
        expected_types.push_back(Message::T_DOWNLOAD);
        expected_types.push_back(Message::T_ERROR);

        if (old_list_filename.size() > 0)
        {
            for (list<string>::iterator it = old_list_filename.begin(); it != old_list_filename.end(); ++it)
            {
                string filename = *it;
                connection->send(Message::T_DEL, filename);
                connection->receive_ack();
            }
        }

        connection->send(Message::T_DONE);
        connection->receive_ack();

        // Receives for files that client doesn't have yet
        while (true)
        {
            Message msg = connection->receive(expected_types);
            if (msg.type == Message::T_DONE)
            {
                debug("Sync finished!");
                connection->send_ack();
                break;
            }
            else if (msg.type == Message::T_DOWNLOAD)
            {
                string filename = msg.content;
                string filepath = connection->user_directory + '/' + filename;
                if (can_be_transfered(filename))
                {
                    debug("Starting to download" + filename + " ...");
                    connection->send_ack();
                    if (connection->receive_file(filepath) == 0)
                    {
                        debug(filename + " downloaded successfully!");
                    }
                    else
                    {
                        debug(filename + " download failed!");
                    }
                    unlock_file(filename);
                }
                else
                {
                    cout << "Error: File " << filename << " already being transfered" << endl;
                    connection->send_ack(false);
                    connection->receive_ack();
                    continue;
                }
            }
            else
            {
                debug("Error! File not found!");
                connection->send_ack();
                continue;
            }
        }

        old_list_filename = File::list_filename(connection->user_directory);

        debug("SLEEPING for 5", __FILE__, __LINE__, Color::RED);
        sleep(5);
    }
}