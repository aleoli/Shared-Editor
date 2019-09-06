#include "../client.h"
#include <unistd.h>
#include "socket.h"

void test_socket() {
  Socket s{};
  if(s._bind(1234) != 0) {
    std::cerr << "Non posso fare il bind alla porta 1234" << std::endl;
    exit(1);
  }
  s._listen();
  while(true) {
    std::cout << "In attesa su localhost:1234, invia tutto quello che vuoi, il terminatore di pacchetto è 'a'" << std::endl;
    auto s2 = s._accept();
    std::cout << "Tra 10 secondi chiudo il Client (e tra 5 ti mando un 'wei!!')" << std::endl;
    Client c{std::move(s2)};
    try {
      c();
    } catch(std::exception e) {
      std::cout << e.what() << std::endl;
      throw e;
    }
    sleep(5);
    c.send("wei!!");
    sleep(5);
    std::cout << "Chiudo client" << std::endl;
  }
}
