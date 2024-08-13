#include "file.hh"

unordered_map<string,string> db;

visitor_type visitor{
    [](const Result& v) -> void { v.x->print(); },
    [](const Error&) -> void { cout << "(error)" << endl; }
};