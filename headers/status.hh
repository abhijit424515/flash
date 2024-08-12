#include "common.hh"

template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
	overload(Ts... ts) : Ts(std::move(ts))... {}
};

template<class... Ts>
overload(Ts...) -> overload<Ts...>;

// --------------------------------

struct Ok {};
struct Value { 
	string x;
	Value(string x): x(x) {}
};
struct Nil {};
struct Error {};

using Status = variant<Ok,Value,Nil,Error>;

using visitor_type = overload<
    function<string(const Ok&)>,
    function<string(const Value&)>,
    function<string(const Nil&)>,
    function<string(const Error&)>
>;