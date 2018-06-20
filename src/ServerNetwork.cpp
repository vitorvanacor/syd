#include "ServerNetwork.hpp"

#include <string>

ServerNetwork::ServerNetwork(Connection *connection) {

  list<string> expected_types;
  expected_types.push_back(Message::T_ELECTION);
  expected_types.push_back(Message::T_ALIVE);
  expected_types.push_back(Message::T_COORD);

  void ServerNetwork::run() {
    list <string> otherServers = read("servers.txt");
    string myIP = otherServers[0];
    bool skip_first = true;

    foreach (otherServers, string server) {
      if skip_first { // because first IP is my IP
        skip_first = false;
        continue;
      }

      Connection* connection = new Connection();
      if (connection->connect(server)) {
        onlineServers[server] = connection;
      } else {
        delete connection;
      }
    }

    mainloop();
  }

  void ServerNetwork::mainloop() {
    // run just generated the list of connections, I should call elections
    election_coordinator();
    while(true) {
      // keep receiving messages from all to check if one of them recovered and wants an election
      foreach (onlineServers, string server) {
        Message msg = server->receive(expected_types); //precisa de um timeout aqui pra dar continue ou ele faz sozinho?

        if (msg.type == Message::T_ELECTION) {
          election_coordinator();
        }
      }
    }
  }

  void ServerNetwork::election_coordinator() {
    bool i_am_bully = true;
    string the_bully = "";

    foreach (onlineServers, string candidate) {
      bool i_am_smaller = false;

      i_am_smaller = (myIP < candidate.username);
      
      if i_am_smaller {
        // send ELECTION message to the candidate whose IP is bigger than mine
        candidate->send(Message::T_ELECTION);
        candidate->receive_ack();

        // if I receive an ALIVE message I'm not the bully
        Message msg = candidate->receive(expected_types);
        if (msg.type == Message::T_ALIVE) {
          i_am_bully = false;
          // if the IP is the biggest I have seen then he is the bully
          if (candidate.username > the_bully)
          the_bully = candidate;
        }
        // ***** TODO: pensar no caso de não receber o ALIVE = tirar candidato da lista, pode ser só timeout ou problema temporário *****
      }
    }

    if i_am_bully {
      // send COORD message to other servers saying that I'm the coordinator
      foreach (onlineServers, string loser) {
        looser->send(Message::T_COORD);
        looser->receive_ack();
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