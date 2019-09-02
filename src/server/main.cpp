#define TEST_USER 1

#include <iostream>

#include "persistence_global.h"
#include "sys.h"

#if TEST_USER
#include <vector>
#include "user.h"
#endif

int main(int argc, char *argv[]) {
  initiate_system();

#if TEST_USER
  auto users{DB::getAll<User>()};
  if(users.empty()) {
    std::cout << "Inserisco Utente" << std::endl;
    User u = User::registra("aleoli", "aleoli96@gmail.com", "ciao");
    users = DB::getAll<User>();
  }
  for(auto& u: users) {
    std::cout << u.getNickname().toStdString() << std::endl;
  }
  std::cout << std::endl << std::endl;


  try {
    //auto u = User::login("' OR 1 OR '", "ciao");
    auto u = User::login("aleoli", "ciao");
    std::cout << u.getNickname().toStdString() << std::endl;
  } catch(std::exception e) {
    std::cout << "EXCEPTION: " << e.what() << std::endl;
    throw e;
  }
#endif

  return 0;
}
