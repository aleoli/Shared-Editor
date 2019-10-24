#pragma once

#include "utils.h"
#include "../FSElement_db.h"
#include "../user.h"
#include "../session.h"
#include "../SharedLink.h"

void test_fs() {
  User u = User::registra("fs_tester", "fs_tester@localhost", "ciao");
  User u2 = User::registra("fs_tester2", "fs_tester2@localhost", "ciao");
  Session s = User::login("fs_tester", "ciao");

  {
    auto f1 = FSElement_db::root(s.getUserId()).mkfile(s, "f1.se");
    auto fs = f1.getFSElement();
    debug(QString(fs.to_string().c_str()));
    auto f = f1.load();
    debug(QString(f.to_string().c_str()));

    f1.remove();
    info("Remove test PASSED");
  }

  {
    debug("Tree test");
    auto dir = FSElement_db::root(s.getUserId()).mkdir(s, "d1");
    auto f1 = dir.mkfile(s, "f1.se");
    auto f2 = dir.mkfile(s, "f2.se");

    debug(dir.getName());
    for(auto& c: dir.ls(s)) {
      debug("\t+ "+c->getName());
    }

    dir.remove();

    auto list = DB::getAll<FSElement_db>();
    if(list.size() == 2) {
      info("Tree test PASSED");
    } else {
      warn("There are "+QString::number(list.size())+" elements in list");
    }
  }

  {
    debug("Share Test");
    auto s2 = User::login("fs_tester2", "ciao");

    auto dir = FSElement_db::root(s.getUserId()).mkdir(s, "d1");
    auto f1 = dir.mkfile(s, "f1.se");
    auto f2 = dir.mkfile(s, "f2.se");

    auto sl = f1.share(s);
    auto f1_2 = FSElement_db::link(s2, sl.getToken());

    bool match = f1.load().to_string() == f1_2.load().to_string();

    f1_2.remove();
    bool t1 = DB::getAll<FSElement_db>().size() == 5;

    auto f1_3 = FSElement_db::link(s2, sl.getToken());
    f1.remove();

    bool t2 = DB::getAll<FSElement_db>().size() == 4;

    f1 = dir.mkfile(s, "f1.se");
    sl = f1.share(s);
    auto f1_4 = FSElement_db::link(s2, sl.getToken());

    dir.remove();

    bool t3 = DB::getAll<FSElement_db>().size() == 2;

    if(match && t1 && t2 && t3) {
      info("Share Test PASSED");
    } else {
      warn("Share Test FAILED");
    }

    s2.close();
  }

	{
		debug("Move Test");

		auto root = FSElement_db::root(s.getUserId());
		auto d1 = root.mkdir(s, "d1");
		auto d2 = root.mkdir(s, "d2");
		auto f1 = d1.mkfile(s, "f1.se");
		auto f2 = d2.mkfile(s, "f2.se");

		debug("\nd1 files");
		for(auto& f: d1.ls(s)) {
			debug("\t+ "+f->getName());
		}
		debug("\nd2 files");
		for(auto& f: d2.ls(s)) {
			debug("\t+ "+f->getName());
		}

		debug("\nMove f2 to d1\n");
		f2.mv(s, d1);
		d2.clearCache();	// devo pulire la cache della vecchia cartella, non può farlo da solo

		debug("\nd1 files");
		for(auto& f: d1.ls(s)) {
			debug("\t+ "+f->getName());
		}
		debug("\nd2 files");
		for(auto& f: d2.ls(s)) {
			debug("\t+ "+f->getName());
		}

		if(d1.ls(s).size() == 2 && d2.ls(s).size() == 0) {
			info("Move Test PASSED");
		} else {
			warn("Move Test FAILED");
		}

		d1.remove();
		d2.remove();
	}

  s.close();
}
