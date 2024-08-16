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

struct Del: Cmd {
	KeyWrapper *kw;

	Del(KeyWrapper *kw): kw(kw) {}
	Status code() override {
		Int *x = new Int(0);
		for (auto k: kw->keys) {
			auto z = db.find(*k);
			if (z == db.end())
				continue;
			db.erase(z);
			x->modify(1);
		}
		return Result{x};
	}
	~Del() {
		delete kw;
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

struct LPush: Cmd {
	string *key;
	KeyWrapper *values;

	LPush(string *k, KeyWrapper *v): key(k), values(v) {}
	Status code() override {
		Int *x = new Int(0);
		List *l;
		auto z = db.find(*key);

		if (z == db.end()) {
			l = new List{};
			db[*key] = l;
		} else {
			if (z->second->t != ValueType::LIST)
				return Error{"value is not a list"};
			l = (List*)(z->second);
		}

		for (auto v: values->keys) {
			l->push_front(is_number(*v) ? (Value*) new Int{*v} : (Value*) new String{*v});
			x->modify(1);
		}
		return Result{x};
	}
	~LPush() {
		delete key;
		delete values;
	}
};

struct RPush: Cmd {
	string *key;
	KeyWrapper *values;

	RPush(string *k, KeyWrapper *v): key(k), values(v) {}
	Status code() override {
		Int *x = new Int(0);
		List *l;
		auto z = db.find(*key);

		if (z == db.end()) {
			l = new List{};
			db[*key] = l;
		} else {
			if (z->second->t != ValueType::LIST)
				return Error{"value is not a list"};
			l = (List*)(z->second);
		}
		for (auto v: values->keys) {
			l->push_back(is_number(*v) ? (Value*) new Int{*v} : (Value*) new String{*v});
			x->modify(1);
		}
		return Result{x};
	}
	~RPush() {
		delete key;
		delete values;
	}
};

struct LPop: Cmd {
	string *key, *count;

	LPop(string *k, string *c): key(k), count(c) {}
	Status code() override {
		auto z = db.find(*key);
		if (z == db.end())
			return Result{new Nil{}};
		if (z->second->t != ValueType::LIST)
			return Error{"value is not a list"};
		if (!is_number(*count))
			return Error{"count is not an integer"};

		ll c = stoll(*count);
		List *l = (List*)(z->second);
		List *ret = new List{};

		while (c--) {
			if (l->empty())
				continue;
			ret->push_back(l->front());
			l->pop_front();
		}

		if (ret->empty()) {
			delete ret;
			return Result{new Nil{}};
		}
		return Result{ret};
	}
	~LPop() {
		delete key;
	}
};

struct RPop: Cmd {
	string *key, *count;

	RPop(string *k, string *c): key(k), count(c) {}
	Status code() override {
		auto z = db.find(*key);
		if (z == db.end())
			return Result{new Nil{}};
		if (z->second->t != ValueType::LIST)
			return Error{"value is not a list"};
		if (!is_number(*count))
			return Error{"count is not an integer"};

		ll c = stoll(*count);
		List *l = (List*)(z->second);
		List *ret = new List{};

		while (c--) {
			if (l->empty())
				return Result{new Nil{}};
			ret->push_back(l->back());
			l->pop_back();
		}

		return Result{ret};
	}
	~RPop() {
		delete key;
	}
};

struct LLen: Cmd {
	string *key;

	LLen(string *k): key(k) {}
	Status code() override {
		auto z = db.find(*key);
		if (z == db.end())
			return Result{new Int(0)};
		if (z->second->t != ValueType::LIST)
			return Error{"value is not a list"};
		List *l = (List*)(z->second);
		return Result{new Int(l->size())};
	}
	~LLen() {
		delete key;
	}
};

struct LMove: Cmd {
	string *src, *dest;
	bool sl, dl;

	LMove(string *s, string *d, bool sl, bool dl): src(s), dest(d), sl(sl), dl(dl) {}
	Status code() override {
		Value *ret;

		auto z = db.find(*src);
		if (z == db.end())
			return Result{new Nil{}};
		if (z->second->t != ValueType::LIST)
			return Error{"source value is not a list"};
		List *l = (List*)(z->second);

		auto y = db.find(*dest);
		if (y == db.end()) {
			db[*dest] = new List{};
			y = db.find(*dest);
		}
		if (y->second->t != ValueType::LIST)
			return Error{"destination value is not a list"};
		List *m = (List*)(y->second);

		if (l->empty())
			return Result{new Nil{}};

		if (sl) {
			ret = l->front();
			l->pop_front();
		} else {
			ret = l->back();
			l->pop_back();
		}

		if (dl) m->push_front(ret);
		else m->push_back(ret);
		return Result{ret};
	}
};

struct LRange: Cmd {
	string *key, *start, *end;

	LRange(string *k, string *s, string *e): key(k), start(s), end(e) {}
	Status code() override {
		auto z = db.find(*key);
		if (z == db.end())
			return Result{new Nil{}};
		if (z->second->t != ValueType::LIST)
			return Error{"value is not a list"};
		if (!is_number(*start) || !is_number(*end))
			return Error{"start or end is not an integer"};

		List *l = (List*)(z->second);
		ll s = stoll(*start), e = stoll(*end);
		ll n = l->size();

		if (s >= n) 
			s = n-1;
		if (e >= n)
			e = n-1;
		if (s < 0)
			s = ((s%n)+n)%n;
		if (e < 0)
			e = ((e%n)+n)%n;

		List *ret = new List{};
		for (ll i=s; i<=e; i++)
			ret->push_back((*l)[i]);
		return Result{ret};
	}
	~LRange() {
		delete key;
		delete start;
		delete end;
	}
};

struct LTrim: Cmd {
	string *key, *start, *end;

	LTrim(string *k, string *s, string *e): key(k), start(s), end(e) {}
	Status code() override {
		auto z = db.find(*key);
		if (z == db.end())
			return Result{new Nil{}};
		if (z->second->t != ValueType::LIST)
			return Error{"value is not a list"};
		if (!is_number(*start) || !is_number(*end))
			return Error{"start or end is not an integer"};

		List *l = (List*)(z->second);
		ll s = stoll(*start), e = stoll(*end);
		ll n = l->size();

		if (s >= n) 
			s = n-1;
		if (e >= n)
			e = n-1;
		if (s < 0)
			s = ((s%n)+n)%n;
		if (e < 0)
			e = ((e%n)+n)%n;

		List *ret = new List{};
		for (ll i=s; i<=e; i++)
			ret->push_back((*l)[i]);
		db[*key] = ret;
		return Result{new Ok{}};
	}
	~LTrim() {
		delete key;
		delete start;
		delete end;
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
			ll t = s->add(is_number(*m) ? (Value*) new Int{*m} : (Value*) new String{*m});
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
			ll t = s->rem(is_number(*m) ? (Value*) new Int{*m} : (Value*) new String{*m});
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
		bool t = s->is_member(is_number(*member) ? (Value*) new Int{*member} : (Value*) new String{*member});
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
		auto s = (unordered_set<Value*>*)((UnorderedSet*)(z->second)->v);
		return Result{new Int(s->size())};
	}
	~SCard() {
		delete key;
	}
};

#endif