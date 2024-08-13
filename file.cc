#include "file.hh"

unordered_map<string,Value*> db;

visitor_type visitor{
    [](const Result& v) -> void { v.x->print(); },
    [](const Error& v) -> void { cout << "(error) " << v.msg << endl; }
};

bool is_number(const string& s) {
	char *p;
	ll x = strtoll(s.c_str(), &p, 10);
	return (*p == 0);
}