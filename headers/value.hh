#ifndef VALUE_HH
#define VALUE_HH

#include "common.hh"
#include "wrapper.hh"

enum struct ValueType {
	OK,
	NIL,
	INT,
	STR,
	LIST,
	UNO_SET,
};

bool is_number(const string& s);

struct Value {
	ValueType t;
	void *v;

	virtual void print() = 0;
	virtual bool equals(const Value* other) const = 0;
};

struct Ok: Value {
	Ok() { t = ValueType::OK; }
	void print() override {
		cout << "OK";
	}
	bool equals(const Value* other) const override {
        return other->t == ValueType::OK;
    }
};

struct Nil: Value {
	Nil() { t = ValueType::NIL; }
	void print() override {
		cout << "(nil)";
	}
	bool equals(const Value* other) const override {
        return other->t == ValueType::NIL;
    }
};

struct Int: Value {
	Int(string x) {
		t = ValueType::INT;
		v = (void*) new ll(stoll(x));
	}
	Int(ll x) {
		t = ValueType::INT;
		v = (void*) new ll(x);
	}
	ll modify(ll x) {
		ll *t = (ll*)v;
		*t += x;
		return *t;
	}
	void print() override {
		cout << "(integer) " << *((ll*)v);
	}
	bool equals(const Value* other) const override {
        if (other->t != ValueType::INT) 
			return false;
        return *((ll*)v) == *((ll*)(other->v));
    }
};

struct String: Value {
	String(string x) {
		t = ValueType::STR;
		v = (void*) new string(x);
	}
	void print() override {
		cout << '\"' << *((string*)v) << '\"';
	}
	bool equals(const Value* other) const override {
        if (other->t != ValueType::STR) 
			return false;
        return *((string*)v) == *((string*)other->v);
    }
};

struct List: Value {
	using type = deque<Value*>;

	List() {
		t = ValueType::LIST;
		v = new type();
	}
	ll size() {
		auto z = (type*)v;
		return z->size();
	}
	bool empty() {
		auto z = (type*)v;
		return z->empty();
	}
	Value* back() {
		auto z = (type*)v;
		return z->back();
	}
	Value* front() {
		auto z = (type*)v;
		return z->front();
	}
	ll push_back(Value* x) {
		auto z = (type*)v;
		z->push_back(x);
		return z->size();
	}
	ll push_front(Value* x) {
		auto z = (type*)v;
		z->push_front(x);
		return z->size();
	}
	Value* pop_back() {
		auto z = (type*)v;
		Value *x = z->back();
		z->pop_back();
		return x;
	}
	Value* pop_front() {
		auto z = (type*)v;
		Value *x = z->front();
		z->pop_front();
		return x;
	}
	Value* operator[](ll i) {
		auto z = (type*)v;
		return (*z)[i];
	}

	void print() override {
		auto z = (type*)v;
		if (z->size() == 0) {
			cout << "(empty)";
			return;
		}

		int i = 1;
		for (auto it = z->begin(); it != z->end(); ++it) {
			cout << i++ << ") ";
			(*it)->print();
			if (std::next(it) != z->end())
				cout << endl;
		}
	}
	bool equals(const Value* other) const override {
		if (other->t != ValueType::LIST) 
			return false;

		auto lhsList = static_cast<type*>(v);
		auto rhsList = static_cast<type*>(other->v);

		if (lhsList->size() != rhsList->size()) 
			return false;

		auto lhsIt = lhsList->begin();
		auto rhsIt = rhsList->begin();

		while (lhsIt != lhsList->end() && rhsIt != rhsList->end()) {
			if (!(*lhsIt)->equals(*rhsIt)) 
				return false;
			++lhsIt, ++rhsIt;
		}

		return true;
	}
};

struct UnorderedSet: Value {
	using type = unordered_set<Value*>;

	UnorderedSet() {
		t = ValueType::UNO_SET;
		v = new type();
	}
	ll add(Value* x) {
		auto z = ((type*)v);
		if (find_if(z->begin(), z->end(), [x](Value* t) { return t->equals(x); }) == z->end()) {
			z->insert(x);
			return 1LL;
		}
		return 0LL;
	}
	ll rem(Value* x) {
		auto z = ((type*)v);
		auto it = find_if(z->begin(), z->end(), [x](Value* t) { return t->equals(x); });
		if (it == z->end())
			return 0LL;
		z->erase(*it);
		return 1LL;
	}
	bool is_member(Value* x) {
		auto z = ((type*)v);
		return !(find_if(z->begin(), z->end(), [x](Value* t) { return t->equals(x); }) == z->end());
	}
	void print() override {
		auto z = (type*)v;
		if (z->size() == 0) {
			cout << "(empty)";
			return;
		}

		int i = 1;
		for (auto it = z->begin(); it != z->end(); ++it) {
			cout << i++ << ") ";
			(*it)->print();
			if (std::next(it) != z->end())
				cout << endl;
		}
	}
	bool equals(const Value* other) const override {
		if (other->t != ValueType::UNO_SET)
			return false;

		auto lhsSet = static_cast<type*>(v);
		auto rhsSet = static_cast<type*>(other->v);

		if (lhsSet->size() != rhsSet->size())
			return false;

		for (const auto& elem : *lhsSet) {
			auto it = std::find_if(rhsSet->begin(), rhsSet->end(),
								[&elem](const Value* rhsElem) { return elem->equals(rhsElem); });
			if (it == rhsSet->end())
				return false;
		}

		return true;
	}
};

#endif