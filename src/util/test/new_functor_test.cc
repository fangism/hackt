/**
	\file "new_functortest.cc"
	$Id: new_functor_test.cc,v 1.1 2005/03/03 04:20:00 fang Exp $
 */

#include <iostream>
#include <vector>
#include "new_functor.tcc"

#include "using_ostream.h"
using std::vector;
using util::new_functor_base;
using util::new_functor;

class Base {
public:
	virtual ~Base() { }
	virtual void who(void) const = 0;
};

class A : public Base {
public:
	~A() { }
	void who(void) const {
		cerr << "A" << endl;
	}
};

class B : public Base {
public:
	~B() { }
	void who(void) const {
		cerr << "B" << endl;
	}
};

class C : public Base {
public:
	~C() { }
	void who(void) const {
		cerr << "C" << endl;
	}
};

int
main(int argc, char* argv[]) {
	typedef	vector<new_functor_base<Base*>*> new_func_vec_type;
	new_func_vec_type func_vec;
	new_functor<A,Base> A_ctor;
	new_functor<B,Base> B_ctor;
	new_functor<C,Base> C_ctor;
	func_vec.push_back(&A_ctor);
	func_vec.push_back(&B_ctor);
	func_vec.push_back(&C_ctor);
	Base* foo = (*func_vec[0])();
	Base* bar = (*func_vec[1])();
	Base* ick = (*func_vec[2])();
	foo->who();
	bar->who();
	ick->who();
	delete foo;
	delete bar;
}

