#ifndef CMD_HH
#define CMD_HH

#include "common.hh"
#include "value.hh"
#include "status.hh"

extern unordered_map<string,Value*> db;

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
		return Result{z->second};
	}
	~Get() {
		delete key;
	}
};

struct Set: Cmd {
	string *key, *value;

	Set(string *k, string *v): key(k), value(v) {}
	Status code() override {
		bool z = is_number(*value);
		Value *v;
		if (z) v = new Int(*value);
		else v = new String(*value);
		db[*key] = v;
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
			values->push_back(z->second);
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
		for (auto z: kvw->kvs) {
			string *key = z->kv->first, *value = z->kv->second;
			bool t = is_number(*value);
			Value *v;
			if (t) v = new Int(*value);
			else v = new String(*value);
			db[*key] = v;
		}
		return Result{new Ok{}};
	}
	~Mset() {
		delete kvw;
	}
};

// --------------------------------

struct ModifyInt: Cmd {
	string *key, *inc;
	bool neg;

	ModifyInt(string *k, string *inc, bool neg=0): key(k), inc(inc), neg(neg) {}
	Status code() override {
		auto z = db.find(*key);
		if (z == db.end())
			return Result{db[*key] = new Int("1")};
		if (z->second->t != ValueType::INT)
			return Error{"value is not an integer"};
		if (!is_number(*inc))
			return Error{"modification is not an integer"};

		Int *x = (Int*)z->second;
		x->modify((neg ? -1 : 1)*stoll(*inc));
		return Result{x};
	}
	~ModifyInt() {
		delete key;
		delete inc;
	}
};

#endif