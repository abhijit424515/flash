#ifndef STATUS_HH
#define STATUS_HH

#include "common.hh"
#include "value.hh"

template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
	overload(Ts... ts) : Ts(std::move(ts))... {}
};

template<class... Ts>
overload(Ts...) -> overload<Ts...>;

// --------------------------------

struct Result {
	Value *x;
	Result(Value *x): x(x) {}
};
struct Error {};

using Status = variant<Result,Error>;

using visitor_type = overload<
    function<void(const Result&)>,
    function<void(const Error&)>
>;

extern visitor_type visitor;

// --------------------------------

template <typename T>
void process(T* x) {
	visit(visitor, x->code());
	cout << endl;
	delete x;
}

#endif