/**
	\file "new_functortest.cc"
	$Id: new_functor_test.cc,v 1.2 2005/03/04 03:17:42 fang Exp $
 */

#include <iostream>
#include <vector>
#include "memory/excl_ptr.h"
#include "new_functor.tcc"

#include "using_ostream.h"
using std::vector;
using util::new_functor;
using util::binder_new_functor;
using util::nullary_function_virtual;
using util::memory::excl_ptr;

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
	const int	val;

	explicit
	C(const int v) : val(v) { }

	~C() { }

	void who(void) const {
		cerr << "C" << val << endl;
	}
};

typedef	nullary_function_virtual<Base*>		new_functor_base;

int
main(int argc, char* argv[]) {
	typedef	vector<new_functor_base*> new_func_vec_type;
	new_func_vec_type func_vec;
	new_functor<A,Base> A_ctor;
	new_functor<B,Base> B_ctor;
	binder_new_functor<C,Base,int> C_ctor(4);
	binder_new_functor<C,Base,int> D_ctor(9);
	func_vec.push_back(&A_ctor);
	func_vec.push_back(&B_ctor);
	func_vec.push_back(&C_ctor);
	func_vec.push_back(&D_ctor);
	excl_ptr<Base> foo((*func_vec[0])());
	excl_ptr<Base> bar((*func_vec[1])());
	excl_ptr<Base> ick((*func_vec[2])());
	excl_ptr<Base> yum((*func_vec[3])());
	foo->who();
	bar->who();
	ick->who();
	yum->who();
	// foo, bar, ick, yum, auto-deleted
}

