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

// --------------------------------

struct SAdd: Cmd {
	string *key;
	KeyWrapper *members;

	SAdd(string *k, KeyWrapper *m): key(k), members(m) {}
	Status code() override {
		Int *x = new Int(0);
		UnorderedSet *s;
		auto z = db.find(*key);

		if (z == db.end()) {
			s = new UnorderedSet{};
			db[*key] = s;
		} else {
			if (z->second->t != ValueType::UNO_SET)
				return Error{"value is not a set"};
			s = (UnorderedSet*)(z->second);
		}
		for (auto m: members->keys) {
			ll t = s->add(new String{*m});
			x->modify(t);
		}
		return Result{x};
	}
	~SAdd() {
		delete key;
		delete members;
	}
};

struct SRem: Cmd {
	string *key;
	KeyWrapper *members;

	SRem(string *k, KeyWrapper *m): key(k), members(m) {}
	Status code() override {
		Int *x = new Int(0);
		UnorderedSet *s;
		auto z = db.find(*key);

		if (z == db.end()) {
			s = new UnorderedSet{};
			db[*key] = s;
		} else {
			if (z->second->t != ValueType::UNO_SET)
				return Error{"value is not a set"};
			s = (UnorderedSet*)(z->second);
		}
		for (auto m: members->keys) {
			ll t = s->rem(new String{*m});
			x->modify(t);
		}
		return Result{x};
	}
	~SRem() {
		delete key;
		delete members;
	}
};

struct SIsMember: Cmd {
	string *key, *member;

	SIsMember(string *k, string *m): key(k), member(m) {}
	Status code() override {
		UnorderedSet *s;
		auto z = db.find(*key);

		if (z == db.end())
			return Result{new Int(0)};
		else {
			if (z->second->t != ValueType::UNO_SET)
				return Error{"value is not a set"};
			s = (UnorderedSet*)(z->second);
		}
		bool t = s->is_member(new String{*member});
		return Result{new Int((ll)t)};
	}
	~SIsMember() {
		delete key;
		delete member;
	}
};

struct SInter: Cmd {
	KeyWrapper *keys;

	SInter(KeyWrapper *k): keys(k) {}
	Status code() override {
		using type = unordered_set<Value*>;

		UnorderedSet *s = new UnorderedSet{};
		bool first=1;

		for (auto k: keys->keys) {
			auto z = db.find(*k);
			if (z == db.end())
				continue;
			if (z->second->t != ValueType::UNO_SET)
				return Error{"atleast one value is not a set"};
			UnorderedSet *x = (UnorderedSet*)(z->second);

			if (first) {
				for (auto i: *(type*)(x->v))
					s->add(i);
				first = 0;
			} else {
				UnorderedSet *y = new UnorderedSet{};
				for (auto i: *(type*)(s->v)) {
					if (x->is_member(i))
						y->add(i);
				}
				s = y;
			}
		}
		return Result{s};
	}
	~SInter() {
		delete keys;
	}
};

struct SCard: Cmd {
	string *key;

	SCard(string *k): key(k) {}
	Status code() override {
		auto z = db.find(*key);
		if (z == db.end())
			return Result{new Int(0)};
		if (z->second->t != ValueType::UNO_SET)
			return Error{"value is not a set"};
		UnorderedSet *s = (UnorderedSet*)(z->second);
		return Result{new Int(((unordered_set<Value*>*)(s->v))->size())};
	}
	~SCard() {
		delete key;
	}
};

#endif