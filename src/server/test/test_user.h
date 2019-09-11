#include <vector>
#include "../user.h"
#include "../session.h"

void test_user() {
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
}
