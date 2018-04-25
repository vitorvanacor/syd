#include <string>
#include <iostream>

#include "Socket.h"

using namespace std;

#define PORT 4000

int main(int argc, char *argv[])
{
    Socket sock = Socket(PORT);
    sock.bind_server();
    std::string msg;
    while (1)
    {
        cout << "Waiting for message..." << endl;
        msg = sock.receive();
        cout << "Message received: " << msg << endl;
        sock.reply("Server ACK");
        cout << "ACK sent." << endl;
    }
    return 0;
}
