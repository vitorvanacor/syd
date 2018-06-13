#include "ServerNetwork.hpp"

#include <string>
#include <cstring>

ServerNetwork::ServerNetwork(Connection *connection) {

  void ServerNetwork::run() {
    list <string> otherServers = read("servers.txt");
    foreach (otherServers, string server) {
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
    while(true) {
      // fico cuidando pra ver se eu voltei de alguma falha ou se o detector de falhas viu que meu coord. falhou
      receive_connection(); // fico esperando receber uma mensagem de eleição. se sim, começa o election_coordinator()
    }
  }

  bool ServerNetwork::substring_compare(string IP_address) {
    std::string last_numbers_IP;

    size_t pos = IP_address.find_last_of(".");
    last_numbers_IP = IP_address.substr(pos + 1);

    // depois de comparar (preciso saber se cada computador sabe seu pŕoprio IP), retorna TRUE se eu for menor
    return false;
  }

  void ServerNetwork::election_coordinator() {
    bool i_am_bully = true;

    foreach (onlineServers, string candidate) {
      bool i_am_smaller = substring_compare(candidate.username)
      
      if i_am_smaller {
        // manda ELECTION MESSAGE pro comparador (que é maior que ele), perguntando se o mesmo ainda está vivo
        //se eu receber mensagem de ALIVE:
        i_am_bully = false; // se nao receber nada(TIMEOUT), nao seto i_am_bully para false
      }
    }

    if i_am_bully {
      // manda mensagem de vitória, PARA TODOS, avisando que sou o coordenador
    } else {
      // espero mensagem de vitória para saber quem é meu coordenador
      // em caso de timeout, chamar election_coordinator() de novo!
    }
  }
}