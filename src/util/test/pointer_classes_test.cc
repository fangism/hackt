/**
	\file "pointer_classes_test.cc"
	Test for pointer classes.  
	$Id: pointer_classes_test.cc,v 1.4 2005/01/16 04:47:33 fang Exp $
 */

#include <iostream>
#include <vector>

#include "memory/pointer_classes.h"
#include "var.h"

using std::vector;
#include "using_ostream.h"
using namespace util::memory;

void basic_vector_test(void);
void polymorph_test(void);
void const_polymorph_test(void);

int main(int argc, char* argv[]) {
	basic_vector_test();
	polymorph_test();
	const_polymorph_test();
	return 0;
}

void basic_vector_test(void) {
	excl_ptr<int> i(new int);
	*i = 42;
	assert(i);
	cout << "*i = " << *i << endl;

	excl_ptr<int> j(i);		// transfer of ownership
	assert(j);
	cout << "*j = " << *j << endl;
	assert(!i);
	cout << "i = NULL" << endl;

	int k;
//	vector<excl_ptr<int> > V(4);	// excl_ptr is not copy-constructible
	vector<some_ptr<int> > V(4);
//	k = *i;				// assert should fail
	k = *j;
	assert(j);
	cout << "k = " << k << endl;
	V[1] = i;	assert(!i);	assert(!V[1]);
	V[2] = j;	assert(!j);	assert(V[2]);
//	V[3] = new int(-1);	// can't assign directly to naked pointer!
	{
	excl_ptr<int> foo(new int(-1));		// inconvenient, but safe?
	V[3] = foo;
//	V[3] = excl_ptr<int>(new int(-1));	// can't do this, const temp?
	}
	assert(V[3]);
	k = *V[2];
	assert(V[2]);
	cout << "k = " << k << endl;
	for (k=0 ; k<4; k++) {
		if (V[k])	cout << "*V[" << k << "] = " << *V[k] << endl;
		else	cout << "V[" << k << "] = NULL" << endl;
	}
	assert(!j);
	cout << "j = NULL" << endl;

	assert(V[3]);
	i = V[3];	assert(i);
	assert(V[3]);	assert(!V[3].owned());
	for (k=0 ; k<4; k++) {
		if (V[k])	cout << "*V[" << k << "] = " << *V[k] << endl;
		else	cout << "V[" << k << "] = NULL" << endl;
	}
	cout << "*i = " << *i << endl;

	never_ptr<int> a;	// no default 
	assert(!a);
	cout << "a = NULL" << endl;

//	never_ptr<int> b(new int);	// forbidden use of naked pointer!
	never_ptr<int> b(a);
	assert(!a);
	assert(!b);
	cout << "a = NULL" << endl;
	cout << "b = NULL" << endl;

	never_ptr<int> c(i);
	assert(i);
	cout << "*i = " << *i << endl;
	assert(c);
	cout << "*c = " << *c << endl;
	assert(i == c);

	never_ptr<int> d(j);
	assert(!d);
	assert(!j);
	cout << "j = NULL" << endl;
	cout << "d = NULL" << endl;

	never_ptr<int> e(c);
	assert(c);
	assert(e);
	cout << "*c = " << *c << endl;
	cout << "*e = " << *e << endl;
	assert(c == e);

	vector<never_ptr<int> > W(6);
	for (k=0 ; k<6; k++) {
		assert(!W[k]);
		cout << "W[" << k << "] = NULL" << endl;
	}

	W[0] = a;	assert(!W[0]);
	W[1] = b;	assert(!W[1]);
	assert(c);
	*c = 555;
	W[2] = c;	assert(W[2]);
	W[3] = d;	assert(!W[3]);
	W[4] = e;	assert(W[4]);
	for (k=0 ; k<6; k++) {
		if (W[k])	cout << "*W[" << k << "] = " << *W[k] << endl;
		else	cout << "W[" << k << "] = NULL" << endl;
	}

	W[0] = W[5] = V[2];
	assert(W[5] == V[2]);
	assert(W[0] == V[2]);
	*W[4] = 666;
	for (k=0 ; k<6; k++) {
		if (W[k])	cout << "*W[" << k << "] = " << *W[k] << endl;
		else	cout << "W[" << k << "] = NULL" << endl;
	}

	cout << "end of scope, should be no memory errors!" << endl;
}

void polymorph_test(void) {
	excl_ptr<var_base> x;
	assert(!x);
	cout << "x doesn't exist." << endl;

	excl_ptr<var_base> y(new var_anon(3));
	assert(y);
	cout << y->get_name() << " = " << y->get_val() << endl;

	excl_ptr<var_anon> yy(new var_anon(4));
	assert(yy);
	cout << yy->get_name() << " = " << yy->get_val() << endl;
	assert(y != yy);

	excl_ptr<var_named> z(new var_named(5,"z"));
	assert(z);
	cout << z->get_name() << " = " << z->get_val() << endl;

	excl_ptr<var_base> w(z);
	assert(w);
	assert(!z);
	cout << "z doesn't exist." << endl;
	cout << w->get_name() << " = " << w->get_val() << endl;
	// w == "z"

	excl_ptr<var_base> v(yy);
	assert(!yy);
	assert(v);
	cout << "yy doesn't exist." << endl;
	cout << v->get_name() << " = " << v->get_val() << endl;

	// at this point, w is valid, x is empty, y is valid, v is valid
	// problems with friend-inaccessible constructors across templates
	// now test dynamic_cast-ing via "is_a()"
//	assert(dynamic_cast<excl_ptr<var_named> >(w));
//	assert(dynamic_cast<excl_ptr<var_anon> >(y));
//	assert(dynamic_cast<excl_ptr<var_anon> >(v));
#if 1
	assert(w.is_a<var_named>());
	assert(y.is_a<var_anon>());
	assert(v.is_a<var_anon>());
#else
	assert(is_a<var_named>(w));
	assert(is_a<var_anon>(y));
	assert(is_a<var_anon>(v));
#endif
//	assert(never_ptr<var_named>(w));
//	assert(never_ptr<var_anon>(y));
//	assert(never_ptr<var_anon>(v));
	assert(!yy);
	assert(!x);
	assert(!z);

//	y = new var_named(25, "why");	// forbidden
	y = excl_ptr<var_named>(new var_named(25, "why"));	// wrapped
	assert(y);
	cout << y->get_name() << " = " << y->get_val() << endl;

//	yy = new var_anon(-123);	// forbidden
	yy = excl_ptr<var_anon>(new var_anon(-123));
	assert(yy);
	cout << yy->get_name() << " = " << yy->get_val() << endl;

// somewhere need to test const excl_ptr's non transferrability
// now testing non-deleting pointers "never_ptr"
	never_ptr<var_base> a0(w);	// w is a var_named
	assert(a0);	assert(w);
	cout << a0->get_name() << " = " << a0->get_val() << endl;

	never_ptr<var_named> a1(w.is_a<var_named>());
	assert(a1);	assert(w);
	cout << a1->get_name() << " = " << a1->get_val() << endl;

	never_ptr<var_anon> a2(w.is_a<var_anon>());
	assert(!a2);	assert(w);
	cout << "a2 doesn't exist." << endl;


	never_ptr<var_base> b0(y);	// y is a var_named
	assert(b0);	assert(y);
	cout << b0->get_name() << " = " << b0->get_val() << endl;

	never_ptr<var_named> b1(y.is_a<var_named>());
	assert(b1);	assert(y);
	cout << b1->get_name() << " = " << b1->get_val() << endl;

	never_ptr<var_anon> b2(y.is_a<var_anon>());
	assert(!b2);	assert(y);
	cout << "b2 doesn't exist." << endl;


	never_ptr<var_base> c0(v);	// v is a var_anon
	assert(c0);	assert(v);
	cout << c0->get_name() << " = " << c0->get_val() << endl;

	never_ptr<var_named> c1(v.is_a<var_named>());
	assert(!c1);	assert(v);
	cout << "c1 doesn't exist." << endl;

	never_ptr<var_anon> c2(v.is_a<var_anon>());
	assert(c2);	assert(v);
	cout << c2->get_name() << " = " << c2->get_val() << endl;

	cout << "end of scope, should be no memory errors!" << endl;
}

void const_polymorph_test(void) {
#if 0
	{
		const int* foo = new int(13);
		*foo = 666;	// error: assignment of read-only location
		delete foo;
	}
#endif
	// compare the next two blocks, one reports error, other warning, WTF?
#if 0
	{
		const var_base* foo = new var_anon(12);		// var_base
		assert(foo);
		*foo = 13;	// should be error, reported as warning?
		cout << foo->get_name() << " = " << foo->get_val() << endl;
		delete foo;
	}

	{
		const var_anon* foo = new var_anon(12);		// var_anon
		assert(foo);
		*foo = 13;	// should be error, is reported as such
		cout << foo->get_name() << " = " << foo->get_val() << endl;
		delete foo;
	}
#endif

#if 1
	// pointers to read-only variables
	excl_ptr<const var_base> a(new var_named(21, "fang"));
	cout << a->get_name() << " = " << a->get_val() << endl;

	excl_ptr<const var_base> b(new var_anon(91));
	cout << b->get_name() << " = " << b->get_val() << endl;

#if 0
	*a = 41;	// should be error, but only reported as warning :(
	*b = 61;	// should be error, but only reported as warning :(
	cout << a->get_name() << " = " << a->get_val() << endl;
	cout << b->get_name() << " = " << b->get_val() << endl;
#endif
#endif

#if 0
	// THIS SHOULD NOT BE ALLOWED, and is caught by compiler
	excl_ptr<const var_anon> c0(never_ptr<const var_anon>(NULL));

	// suspicious: is_a is not transferrable!
	excl_ptr<const var_anon> c1(a.is_a<const var_anon>());	// ERROR
	assert(!c1);
	cout << "c1 doesn't exist." << endl;

	excl_ptr<const var_named> c2(a.is_a<const var_named>()); // ERROR
	assert(c2);
	cout << c2->get_name() << " = " << c2->get_val() << endl;


	excl_ptr<const var_anon> d1(b.is_a<const var_anon>());	// ERROR
	assert(d1);
	cout << d1->get_name() << " = " << d1->get_val() << endl;

	excl_ptr<const var_named> d2(b.is_a<const var_named>()); // ERROR
	assert(!d2);
	cout << "d2 doesn't exist." << endl;

	assert(a.is_a<const var_anon>() || b.is_a<const var_anon>());
	assert(a.is_a<const var_named>() || b.is_a<const var_named>());
#endif

	cout << "end of scope, should be no memory errors!" << endl;
}


