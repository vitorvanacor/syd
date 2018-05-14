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
    while (true)
    {
        ///debug("RUNNING CLIENT SYNC", __FILE__, __LINE__, Color::GREEN);

        //foreach (file in connection->user_dir)
        //{

        string filename = "menes.txt";

        connection->send(Message::T_SYNC);
        connection->receive_ack();

        int timestamp = get_filetimestamp(filename);
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

                if (connection->receive_file(filename) == 0)
                    debug(filename + " downloaded successfully!");
                else
                    debug(filename + " downloaded failed!");
            }
            else if (msg.type == Message::T_UPLOAD)
            {
                debug("Starting to upload" + filename + " ...");

                connection->send_ack();

                try
                {
                    if (!ifstream(filename))
                    {
                        cout << "Error opening file " << filename << " at " << working_directory() << endl;

                        connection->send(Message::T_ERROR);
                        connection->receive_ack();

                        continue;
                    }
                    connection->send(Message::T_SOF);
                    connection->receive_ack();

                    debug("Starting to upload " + filename + " ...");
                    if (connection->send_file(filename) == 0)
                        debug(filename + " uploaded successfully!");
                    else
                        debug(filename + " uploaded failed!");
                }
                catch (exception &e)
                {
                    cout << e.what() << endl;

                    connection->send(Message::T_ERROR);
                    connection->receive_ack();

                    continue;
                }
            }
            else if (msg.type == Message::T_EQUAL)
            {
                debug("File is already up-to-date.");
                connection->send_ack();
                continue;
            }
            else
            {
                debug("Error! File not found!");
                connection->send_ack();
                continue;
            }
        }
        else
        {
            debug("Error generating timestamp!");
            continue;
        }

        //}
        //connection->send(Message::T_DONE);
        //while (connection->receive() != Message::T_DONE)
        //{
        //download
        //}

        sleep(30);
    }
}