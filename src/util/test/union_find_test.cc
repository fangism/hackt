/**
	\file "union_find_test.cc"
	$Id: union_find_test.cc,v 1.3 2007/04/15 05:52:38 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <iostream>
#include <string>
#include "util/union_find.hh"
#include "util/using_ostream.hh"

using util::union_find;
using util::union_find_derived;
using std::string;

typedef	union_find<string>		test_type;

#define	DECLARE_NODES							\
		test_type a("a"), b("b"), c("c"), d("d"), e("e"), f("f")

#define	DUMP_NODES(ostr)						\
	dump_union_find(ostr, a) << endl;				\
	dump_union_find(ostr, b) << endl;				\
	dump_union_find(ostr, c) << endl;				\
	dump_union_find(ostr, d) << endl;				\
	dump_union_find(ostr, e) << endl;				\
	dump_union_find(ostr, f) << endl

inline
ostream&
separator(ostream& o, size_t& N) {
	o << "-------------- test #" << N << " ---------------" << endl;
	N++;
	return o;
}

inline
ostream&
dump_union_find(ostream& o, const test_type& t) {
	o << t.value() << " -> " << t.peek()->value() << " ~> " <<
		*t.find();
	return o;
}

int
main(int, char*[]) {
	size_t N = 0;
	separator(cout, N);
{	// test #0
	DECLARE_NODES;
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #1
	DECLARE_NODES;
	a = a;
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #2
	DECLARE_NODES;
	a = b;
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #3
	DECLARE_NODES;
	b = a;
	a = b;
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #4
	DECLARE_NODES;
	a = b = a;
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #5
	DECLARE_NODES;
	a = b = c = d;
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #6
	DECLARE_NODES;
	a = b;
	b = c;
	c = d;
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #7
	DECLARE_NODES;
	c = d;
	b = c;
	a = b;
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #8
	DECLARE_NODES;
	a = b;
	b = c;		// a -> b -> c
	d = e;
	e = f;		// d -> e -> f
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #9
	DECLARE_NODES;
	a = b;
	b = c;
	d = e;
	e = f;
	c = f;		// connecting two canonical nodes
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #10
	DECLARE_NODES;
	a = b;
	b = c;
	d = e;
	e = f;
	a = d;		// should perform path compression
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #11
	DECLARE_NODES;
	a = b;
	b = c;
	d = e;
	e = f;
	c = d;		// should perform path compression
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #12
	DECLARE_NODES;
	a = b;
	b = c;
	d = e;
	e = f;
	a = f;		// should perform path compression
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #13
	DECLARE_NODES;
	a = b;
	b = c;
	c = d;
	d = e;
	e = f;
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #14
	DECLARE_NODES;
	a = b;
	b = c;
	c = d;
	d = e;
	e = f;
	f = a;		// should perform path compression
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #15
	DECLARE_NODES;
	a = b;
	b = c;
	c = d;
	d = e;
	e = f;
	a = f;		// should perform path compression
	DUMP_NODES(cout);
	separator(cout, N);
}{	// test #16
	DECLARE_NODES;
	a = b;
	b = c;
	c = d;
	d = e;
	e = f;
	a = b;		// should perform path compression
	DUMP_NODES(cout);
	separator(cout, N);
}
	return 0;
}


