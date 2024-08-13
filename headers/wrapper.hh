#ifndef WRAPPER_HH
#define WRAPPER_HH

#include "common.hh"

struct KeyWrapper {
	vector<string*> keys;

	KeyWrapper() {}
	KeyWrapper(vector<string*> a): keys(a) {}
	KeyWrapper(string* arg) {
		keys.push_back(arg);
	}
	void push_back(string* arg) {
		keys.push_back(arg);
	}
	~KeyWrapper() {
		for (auto arg : keys) 
			delete arg;
	}
};

struct KV {
	pair<string*,string*> *kv;

	KV(pair<string*,string*> *kv): kv(kv) {}
	KV(const KV& a): kv(a.kv) {}
	~KV() {
		delete kv->first;
		delete kv->second;
		delete kv;
	}
};

struct KVWrapper {
	vector<KV*> kvs;

	KVWrapper() {}
	KVWrapper(vector<KV*> a): kvs(a) {}
	KVWrapper(KV *a) {
		kvs.push_back(a);
	}
	void push_back(KV *a) {
		kvs.push_back(a);
	}
	~KVWrapper() {
		for (auto arg : kvs)
			delete arg;
	}
};

#endif