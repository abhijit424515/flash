#ifndef VALUE_HH
#define VALUE_HH

#include "common.hh"
#include "wrapper.hh"

enum struct ValueType {
	OK,
	NIL,
	STR,
	LIST,
};

struct Value {
	ValueType t;
	void *v;
	virtual void print() = 0;
};

struct Ok: Value {
	Ok() { t = ValueType::OK; }
	void print() override {
		cout << "OK";
	}
};

struct Nil: Value {
	Nil() { t = ValueType::NIL; }
	void print() override {
		cout << "(nil)";
	}
};

struct String: Value {
	String(string x) {
		t = ValueType::STR;
		v = (void*) new string(x);
	}
	void print() override {
		cout << *((string*)v);
	}
};

struct List: Value {
	List() {
		t = ValueType::LIST;
		v = new vector<Value*>();
	}
	void push_back(Value* x) {
		((vector<Value*>*)v)->push_back(x);
	}
	void print() override {
		auto z = (vector<Value*>*)v;

		for (int i=0; i<z->size()-1; i++) {
			cout << '(' << i+1 << ") ";
			(*z)[i]->print();
			cout << endl;
		}
		cout << '(' << z->size() << ") ";
		(*z)[z->size()-1]->print();
	}
};

#endif