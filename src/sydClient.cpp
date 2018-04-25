#include <string>
#include <iostream>

#include "Socket.h"

using namespace std;

#define PORT 4000

int main(int argc, char *argv[])
{	
    if (argc < 2) {
        cerr << "usage: " << argv[0] << " hostname" << endl;
        return 0;
    }

    Socket sock = Socket(PORT);
    sock.set_host(argv[1]);

    string msg;
    cout << "Enter the message: ";
    cin >> msg;

    sock.send(msg);
    cout << "Message sent. Waiting reply..." << endl;
    string reply = sock.wait_reply();
    cout << "Server reply: " << reply << endl;

    return 0;
}