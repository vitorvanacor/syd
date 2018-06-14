#include "sydUtil.h"

#include "Thread.hpp"
#include "Socket.hpp"
#include "Connection.hpp"

class ServerThread : public Thread {

  public:
    void run();
    void main_loop();
    void election_coordinator();
    string substring_compare();

    string myIP;
    list<string> otherServers;
    map<string,Connection*> onlineServers;
}