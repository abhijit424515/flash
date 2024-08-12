#include "file.hh"

unordered_map<string,string> db;

visitor_type visitor{
    [](const Ok&) -> std::string { return "OK"; },
    [](const Value& v) -> std::string { return v.x; },
    [](const Nil&) -> std::string { return "(nil)"; },
    [](const Error&) -> std::string { return "(error)"; }
};