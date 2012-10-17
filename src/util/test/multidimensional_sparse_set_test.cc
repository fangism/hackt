/**
	\file "multidimensional_sparse_set_test.cc"
	Test for multidimensional sparse set.
	$Id: multidimensional_sparse_set_test.cc,v 1.9 2006/04/18 18:42:47 fang Exp $
 */

// always enable assertions
#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <iostream>
#include "util/qmap.tcc"

#include "util/multidimensional_sparse_set.tcc"
#include "util/memory/excl_ptr.hh"

#include <list>
#include "util/using_ostream.hh"

using std::list;
using std::pair;
using namespace util::memory;
using util::multidimensional_sparse_set;

#define	TEST_TEMPLATE_SIGNATURE						\
	template <class T, class R, class L>

TEST_TEMPLATE_SIGNATURE
int
test(void);

int
main(int, char*[]) {
	test<int, pair<int,int>, list<pair<int, int> > >();
//	test<int, pair<int,int>, sublist>();
//	test<const int, pair<const int, const int>, list>();	// nonsense
	return 0;
}


TEST_TEMPLATE_SIGNATURE
int
test(void) {
	typedef	multidimensional_sparse_set<1,T,R,L>	set_1d_type;
	typedef	multidimensional_sparse_set<2,T,R,L>	set_2d_type;
	typedef	multidimensional_sparse_set<3,T,R,L>	set_3d_type;

{
	cout << "1D-array test: " << endl;
	set_1d_type ar0;
	cout << "ar0.dimensions() = " << ar0.dimensions() << endl;

#if 0
//	this should be rejected, can't do this
//	multidimensional_sparse_set<ar0.dimensions(),int> ar1;
	excl_ptr<base_multidimensional_sparse_set<int> >
		ar1(base_multidimensional_sparse_set<int>::
		make_multidimensional_sparse_set(ar0.dimensions()));
	assert(ar1);
	cout << "ar1->dimensions() = " << ar1->dimensions() << endl;
#endif

	ar0.dump(cout << "ar0: ") << endl;

	// discrete_interval_set better be fully tested!
	{
	list<typename set_1d_type::range_type> r0;
	r0.push_back(typename set_1d_type::range_type(0,3));
	assert(!ar0.add_ranges(r0));
	ar0.dump(cout << "ar0: ") << endl;

//	r0.push_back(typename set_1d_type::range_type(0,3));
//	ar0.add_ranges(r0);		// should internal assert fail, yep
	}
	{
	list<typename set_1d_type::range_type> r0;
	r0.push_back(typename set_1d_type::range_type(10,13));
	assert(!ar0.add_ranges(r0));
	ar0.dump(cout << "ar0: ") << endl;
	}
	{
	list<typename set_1d_type::range_type> r0;
	r0.push_back(typename set_1d_type::range_type(4,4));
	assert(!ar0.add_ranges(r0));
	ar0.dump(cout << "ar0: ") << endl;
	}
	{
	list<typename set_1d_type::range_type> r0;
	r0.push_back(typename set_1d_type::range_type(3,5));
	assert(ar0.add_ranges(r0));
	ar0.dump(cout << "ar0: ") << endl;
	}
	{
	list<typename set_1d_type::range_type> r0;
	r0.push_back(typename set_1d_type::range_type(9,9));
	assert(!ar0.add_ranges(r0));
	ar0.dump(cout << "ar0: ") << endl;
	}
	{
	list<typename set_1d_type::range_type> r0;
	r0.push_back(typename set_1d_type::range_type(8,10));
	assert(ar0.add_ranges(r0));
	ar0.dump(cout << "ar0: ") << endl;
	}
	{
	list<typename set_1d_type::range_type> r0;
	r0.push_back(typename set_1d_type::range_type(6,7));
	assert(!ar0.add_ranges(r0));
	ar0.dump(cout << "ar0: ") << endl;
	}
}

{
	cout << "2D-array test: " << endl;
	set_2d_type ar0;
	cout << "ar0.dimensions() = " << ar0.dimensions() << endl;

#if 0
//	this should be rejected, can't do this
//	multidimensional_sparse_set<ar0.dimensions(),int> ar1;
	excl_ptr<base_multidimensional_sparse_set<int> >
		ar1(base_multidimensional_sparse_set<int>::
		make_multidimensional_sparse_set(ar0.dimensions()));
	assert(ar1);
	cout << "ar1->dimensions() = " << ar1->dimensions() << endl;
#endif

	ar0.dump(cout << "ar0: ") << endl;
	{
	list<typename set_2d_type::range_type> r0;
	r0.push_back(typename set_2d_type::range_type(0,3));
//	ar0.add_ranges(r0);		// should internal assert fail, yep
	r0.push_back(typename set_2d_type::range_type(0,1));
	assert(!ar0.add_ranges(r0));
	ar0.dump(cout << "ar0: ") << endl;

	int i;
	for (i=5; i>=0; i--) {
		list<typename set_2d_type::range_type> r1;
		r1.push_back(typename set_2d_type::range_type(i,i));
		int j;
		for (j=5; j>=0; j--) {
			list<typename set_2d_type::range_type> r2(r1);
			r2.push_back(typename set_2d_type::range_type(j,j));
			assert ((i>3 || j>1) != ar0.add_ranges(r2));
			ar0.dump(cout << "ar0: ") << endl;
		}
	}
	}

}

{
	cout << "3D-array test: " << endl;
	set_3d_type ar0;
	cout << "ar0.dimensions() = " << ar0.dimensions() << endl;

#if 0
//	this should be rejected, can't do this
//	multidimensional_sparse_set<ar0.dimensions(),int> ar1;
	excl_ptr<base_multidimensional_sparse_set<int> > ar1(
		base_multidimensional_sparse_set<int>::
		make_multidimensional_sparse_set(ar0.dimensions()));
	assert(ar1);
	cout << "ar1->dimensions() = " << ar1->dimensions() << endl;
#endif
}

	return 0;
}

#undef	TEST_TEMPLATE_SIGNATURE

