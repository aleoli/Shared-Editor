#pragma once

#include "utils.h"
#include "../FSElement_db.h"
#include "../user.h"
#include "../session.h"

void test_fs() {
  User u = User::registra("fs_tester", "fs_tester@localhost", "ciao");
  User u2 = User::registra("fs_tester2", "fs_tester2@localhost", "ciao");
  Session s = User::login("fs_tester", "ciao");

  int id = -1;
  {
    auto fs_e = FSElement_db::create(s, -1, "prova.se", true);
    id = fs_e.getId();
  }

  {
    auto fs_e = DB::getOne<FSElement_db>(id);
    auto fs = fs_e.getFSElement();
    debug(QString(fs.to_string().c_str()));
    auto f = fs_e.load();
    debug(QString(f.to_string().c_str()));

    fs_e.remove();
    info("Remove test PASSED");
  }

  {
    debug("Tree test");
    auto dir = FSElement_db::create(s, -1, "dir", false);
    auto f1 = FSElement_db::create(s, dir.getId(), "f1", true);
    auto f2 = FSElement_db::create(s, dir.getId(), "f2", true);

    debug(dir.getName());
    for(auto& c: dir.getChildren()) {
      debug("\t+ "+c->getName());
    }

    dir.remove();

    auto list = DB::getAll<FSElement_db>();
    if(list.empty()) {
      info("Tree test PASSED");
    } else {
      warn("There are elements in list");
    }
  }

  {
    debug("Share Test");
    Session s2 = User::login("fs_tester2", "ciao");

    auto dir = FSElement_db::create(s, -1, "dir", false);
    auto f1 = FSElement_db::create(s, dir.getId(), "f1", true);
    auto f2 = FSElement_db::create(s, dir.getId(), "f2", true);

    auto f1_2 = f1.clone(s2);

    bool match = f1.load().to_string() == f1_2.load().to_string();

    f1_2.remove();
    bool t1 = DB::getAll<FSElement_db>().size() == 3;

    auto f1_3 = f1.clone(s2);
    f1.remove();

    bool t2 = DB::getAll<FSElement_db>().size() == 2;

    f1 = FSElement_db::create(s, dir.getId(), "f1", true);
    auto f1_4 = f1.clone(s2);

    dir.remove();

    bool t3 = DB::getAll<FSElement_db>().size() == 0;

    if(match && t1 && t2 && t3) {
      info("Share Test PASSED");
    } else {
      warn("Share Test FAILED");
    }

    s2.close();
  }

  s.close();
}
