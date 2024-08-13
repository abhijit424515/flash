#ifndef CMD_HH
#define CMD_HH

#include "common.hh"
#include "value.hh"
#include "status.hh"

extern unordered_map<string,string> db;

// --------------------------------

struct Cmd {
    virtual Status code() = 0;
};

struct Get: Cmd {
	string *key;

	Get(string *k): key(k) {}
	Status code() override {
		auto z = db.find(*key);
		if (z == db.end()) 
			return Result{new Nil{}};
		return Result{new String(z->second)};
	}
	~Get() {
		delete key;
	}
};

struct Set: Cmd {
	string *key, *value;

	Set(string *k, string *v): key(k), value(v) {}
	Status code() override {
		db[*key] = *value;
		return Result{new Ok{}};
	}
	~Set() {
		delete key;
		delete value;
	}
};

struct Mget: Cmd {
	KeyWrapper *kw;

	Mget(KeyWrapper *kw): kw(kw) {}
	Status code() override {
		List *values = new List{};
		for (auto k: kw->keys) {
			auto z = db.find(*k);
			if (z == db.end()) {
				values->push_back(new Nil{});
				continue;
			}
			values->push_back(new String(z->second));
		}
		
		return Result{values};
	}
	~Mget() {
		delete kw;
	}
};

struct Mset: Cmd {
	KVWrapper *kvw;

	Mset(KVWrapper *kvw): kvw(kvw) {}
	Status code() override {
		for (auto z: kvw->kvs) 
			db[*(z->kv->first)] = *(z->kv->second);
		return Result{new Ok{}};
	}
	~Mset() {
		delete kvw;
	}
};

#endif