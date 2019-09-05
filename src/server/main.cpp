#define TEST_USER 1

#include <iostream>

#include "persistence_global.h"
#include "sys.h"

#if TEST_USER
#include <vector>
#include "user.h"
#include "session.h"
#endif

#include "client.h"
#include <unistd.h>
#include "socket.h"

#include "exceptions.h"

using namespace se_exceptions;

int main(int argc, char *argv[]) {
  initiate_system();

#if TEST_USER
  auto users{DB::getAll<User>()};
  if(users.empty()) {
    std::cout << "Inserisco Utente" << std::endl;
    User u = User::registra("aleoli", "aleoli96@gmail.com", "ciao");
    users = DB::getAll<User>();
  }
  std::cout << "LISTA UTENTI" << std::endl;
  for(auto& u: users) {
    std::cout << "\t" << u.getNickname().toStdString() << std::endl;
  }
  std::cout << std::endl << std::endl;


  QString token;
  std::cout << "LOGIN" << std::endl;
  try {
    //auto s = User::login("' OR 1 OR '", "ciao");
    auto s = User::login("aleoli", "ciao");
    std::cout << "\t" << s.getUser().getNickname().toStdString() << std::endl << "\tToken: " << s.getToken().toStdString() << std::endl;
    token = s.getToken();
  } catch(SQLException e) {
    std::cout << "EXCEPTION: " << e.what() << std::endl;
    throw e;
  }

  std::cout << std::endl << std::endl;

  std::cout << "VERIFICA LOGIN" << std::endl;
  try {
    auto s = Session::get(token);
    std::cout << "\t" << s.getUser().getNickname().toStdString() << std::endl << "\tToken: " << s.getToken().toStdString() << std::endl;
  } catch(SQLException e) {
    std::cout << "EXCEPTION: " << e.what() << std::endl;
    throw e;
  }

  std::cout << std::endl << std::endl;

  std::cout << "CHIUDO SESSIONE" << std::endl;
  {
    auto s = Session::get(token);
    try {
      s.close();

      // verifico che non riesca più a trovare la sessione
      try {
        auto s2 = Session::get(token);
        std::cerr << "ERRORE: riesco ancora a trovare la sessione" << std::endl;
      } catch(SQLException e2) {
        std::cout << "\tGiustamente non trovo più la sessione" << std::endl;
      }
    } catch(SQLException e) {
      std::cout << "EXCEPTION: " << e.what() << std::endl;
      throw e;
    }
  }
#endif

  {
    /*Client c{};
    c();
    sleep(1);*/
    Socket s{};
    if(s._bind(1234) != 0) {
      exit(1);
    }
    s._listen();
    while(true) {
      auto s2 = s._accept();
      Client c{std::move(s2)};
      c();
      sleep(10);
      std::cout << "Chiudo client" << std::endl;
    }
    /*{
      auto s2 = s._accept();
      bool has_res = false;
      while(!has_res) {
        auto str = s2._recv(has_res);
        if(has_res) {
          s2._sendn(str);
        }
      }
    }
    {
      auto s2 = s._accept();
      bool has_res = false;
      while(!has_res) {
        auto str = s2._recv(has_res);
        if(has_res) {
          s2._sendn(str);
        }
      }
    }*/
    /*auto s2 = s._accept();
    auto str = s2._recv();
    s2._sendn("ciao!!");*/
  }

  return 0;
}
