#include "ServerNetwork.hpp"

#include <string>

ServerNetwork::ServerNetwork(Connection *connection) {

  list<string> expected_types;
  expected_types.push_back(Message::T_ELECTION);
  expected_types.push_back(Message::T_ALIVE);
  expected_types.push_back(Message::T_COORD);

  void ServerNetwork::run() {
    list <string> otherServers = read("servers.txt");
    // the first ip is mine so I remove from the list
    string myIP = otherServers[0];
    otherServers.pop_front();

    mainloop();
  }

  void ServerNetwork::mainloop() {
    // run just generated the list of servers, I should call elections
    election_coordinator();

    Socket listener(1996);
    listener.bind_server();
    while(true) {
      Message msg = Message::parse(listener.receive());
      if (msg.type == Message::T_ELECTION) {
        election_coordinator();
      }
    }
  }

  void ServerNetwork::election_coordinator() {
    bool i_am_bully = true;
    string the_bully = "";

    foreach (otherServers, string candidate) {
      bool i_am_smaller = false;

      i_am_smaller = (myIP < candidate);
      
      if i_am_smaller {
        // send ELECTION message to the candidate whose IP is bigger than mine
        Connection* bully_connection = new Connection();
        if (bully_connection->connect(candidate)) {
          bully_connection->send(Message::T_ELECTION);
          bully_connection->receive_ack();
        } else {
          delete bully_connection;
        }
        // if I receive an ALIVE message I'm not the bully
        Message msg = bully_connection->receive(expected_types);
        if (msg.type == Message::T_ALIVE) {
          i_am_bully = false;
          // if the IP is the biggest I have seen then he is the bully
          if (bully_connection.username > the_bully)
          the_bully = bully_connection;
        }
      }
    }

    if i_am_bully {
      // send COORD message to other servers saying that I'm the coordinator
      foreach (otherServers, string smaller) {
        Connection* loser = new Connection();
        if (loser = connection->connect(smaller)) {
          loser->send(Message::T_COORD);
          loser->receive_ack();
        } else {
          delete loser;
        }
      // RETURN SOMETHING HERE TO SET SELF AS MASTER
      }
    } else {
      Message msg = the_bully->receive(expected_types);
      if (msg.type == Message::T_COORD) {
        // do nothing, he is the coordinator
      } else { // if the bully does not send a message, start election again
        election_coordinator();
      }
    }
  }
}