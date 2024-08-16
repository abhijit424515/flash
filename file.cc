#include "file.hh"

unordered_map<string,Value*> db;

visitor_type visitor{
    [](const Result& v) -> void { v.x->print(); },
    [](const Error& v) -> void { cout << "(error) " << v.msg; }
};

void clear() {
	#if defined _WIN32
		system("cls");
	#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
		system("clear");
	#elif defined (__APPLE__)
		system("clear");
	#endif
}

bool is_number(const string& s) {
	char *p;
	ll x = strtoll(s.c_str(), &p, 10);
	return (*p == 0);
}