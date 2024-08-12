#ifndef CMD_HH
#define CMD_HH

#include "common.hh"
#include "status.hh"

extern unordered_map<string,string> db;
extern visitor_type visitor;

// --------------------------------

struct Cmd {
    virtual Status code() = 0;
};

struct Get: Cmd {
	string key;

	Get(string k): key(k) {}
	Status code() override {
		auto z = db.find(key);
		if (z == db.end()) 
			return Nil{};
		return Value{z->second};
	}
};

struct Set: Cmd {
	string key, value;

	Set(string k, string v): key(k), value(v) {}
	Status code() override {
		db[key] = value;
		return Ok{};
	}
};

// --------------------------------

template <typename T>
void process(T* x) {
	Status z = x->code();
	string s = visit(visitor, z);
	cout << s << endl;
}

#endif