#include <string>
#include <iostream>
#include <vector>

#include "Socket.hpp"
#include "MessageHeader.hpp"
#include "sydUtil.h"

using namespace std;

int main(int argc, char *argv[])
{
    Socket sock = Socket(4000);
    sock.bind_server();
    std::string msg;
    while (1)
    {
        cout << "Waiting for message..." << endl;
        msg = sock.receive();
        cout << "Message received." << endl;
        MessageHeader header = MessageHeader::parse(msg);
        header.print();
        sock.send("ACK");
        cout << "ACK sent." << endl;
    }
    return 0;
}
