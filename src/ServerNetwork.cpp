#include "ServerNetwork.hpp"

#include <string>

ServerNetwork::ServerNetwork(Connection *connection) {

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

  // ***** TODO *****
  void ServerNetwork::mainloop() {
    // re-gerar lista de servidores vivos (algum pode ter morrido na eleição - ver caso de não receber ALIVE msg)
    while(true) {
      // fico cuidando pra ver se eu voltei de alguma falha ou se o detector de falhas viu que meu coord. falhou
      receive_connection(); // fico esperando receber uma mensagem de eleição. se sim, começa o election_coordinator()
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

        // ***** TODO: pensar no caso de não recer o ack *****

        // if I receive an ALIVE message I'm not the bully
        Message msg = candidate->receive(expected_types);
        if (msg.type == Message::T_ALIVE) {
          i_am_bully = false;
          // if the IP is the biggest I have seen then he is the bully
          if (candidate.username > the_bully)
          the_bully = candidate;
        }
        // ***** TODO: pensar no caso de não receber o ALIVE = tirar candidato da lista *****
      }
    }

    if i_am_bully {
      // send COORD message to other servers saying that I'm the coordinator
      foreach (onlineServers, string loser) {
        looser->send(Message::T_COORD);
        looser->receive_ack();
      }
    } else {
      // espero mensagem de vitória para saber quem é meu coordenador
      Message msg = the_bully->receive(expected_types);
      if (msg.type == Message::T_COORD) {
        // ***** TODO: como dizer que ele é o lider?? *****
      }
      // ***** TODO: em caso de timeout, chamar election_coordinator() de novo! *****
    }
  }
}