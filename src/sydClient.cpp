#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio> 
#include <fstream>

#include "Socket.hpp"
#include "MessageHeader.hpp"
#include "File.hpp"
#include "sydUtil.h"

using namespace std;

int main(int argc, char *argv[])
{	
    string username;
    string hostname;
    int port;
    
    if (argc == 1)
    {
        username = "test_user";
        hostname = "localhost";
        port = 4000;
        cout << "Using: " << username << " " << hostname << " " << port << endl;
    }
    else if (argc == 4)
    {
        username = argv[1];
        hostname = argv[2];
        port = atoi(argv[3]);
    }
    else
    {
        cout << "Usage: " << argv[0] << " user host port" << endl;
        return 0;
    }
    
    Socket sock = Socket(4000);
    sock.set_host("localhost");

    string msg_type = "login!";
    string session_id = std::to_string(rand()%1000);
    string sequence = "0";

    MessageHeader header = MessageHeader(msg_type, session_id, sequence);
    string msg = header.to_string();

    sock.send_to_host(msg);
    cout << "Message sent. Waiting reply..." << endl;
    string reply = sock.receive();
    cout << "Server reply: " << reply << endl;

    File file = File("/home/pietra/Documentos/UFRGS/CIC/SISOP2/syd/menes.txt");
    char *buffer = file.FileToByteArray();
    cout << buffer;

    return 0;
}