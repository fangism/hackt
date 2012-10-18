/**
	\file "list_vector_test.cc"
	Example for using the list_vector class.  
	$Id: list_vector_test.cc,v 1.6 2008/11/12 21:43:12 fang Exp $
 */

#ifdef	NDEBUG
#undef	NDEBUG
#endif

#include <iostream>
#include <numeric>
#include <algorithm>
#include <functional>
#include "util/list_vector.tcc"

#include "util/using_ostream.hh"
using std::ostream_iterator;
using std::copy;
using std::reverse_copy;
using namespace util;

// make sure this is off when running make check
#define	DEBUG_ME	0
#if DEBUG_ME
#define	DUMP_IT(x)	(x).dump_details(cerr);
#else
#define	DUMP_IT(x)
#endif

// can also try with different allocator... ooo!
typedef	list_vector<size_t>	vec_type;

static
void
dump_list_vector(const vec_type& v, ostream& o) {
	o << "iterated: ";
	ostream_iterator<int> osi(o, " ");
	copy(v.begin(), v.end(), osi);
	o << endl;
	o << "indexed : ";
	size_t i = 0;
	const size_t s = v.size();
//	if (s) o << v[0] << ' ' << endl;
	for ( ; i < s; i++) {
//		o << v[i] << ' ';
		o << v.at(i) << ' ';
	}
	o << endl;
	o << "reversed: ";
	reverse_copy(v.begin(), v.end(), osi);
	o << endl;
	o << "reversed: ";
	copy(v.rbegin(), v.rend(), osi);
	o << endl;
	o << "forward : ";
	reverse_copy(v.rbegin(), v.rend(), osi);
	o << endl;
}

template <class Iter>
static
void
check_iterators(Iter first, Iter last) {
	while (first != last) {
		assert(first == --(++first));
		first++;
	}
}

/**
	Push 2, pop 2, push 1.
 */
static
void
complex_push_list_vector(vec_type& v, const vec_type::value_type i) {
	v.push_back(i);
	DUMP_IT(v);
	v.check_invariants();
	v.push_back(i+1);
	DUMP_IT(v);
	v.check_invariants();
	v.pop_back();
	DUMP_IT(v);
	v.check_invariants();
	v.pop_back();
	DUMP_IT(v);
	v.check_invariants();
	v.push_back(i);
	DUMP_IT(v);
	v.check_invariants();
}

static
void
check_empty_list_vector(vec_type& v) {
	vec_type::iterator vb = v.begin();
	vec_type::const_iterator vbc = v.begin();
	vec_type::iterator ve = v.end();
	vec_type::const_iterator vec = v.end();

	assert(vb == ve);
	assert(vbc == vec);
	assert(vb == vec);
	assert(vbc == ve);
	assert(v.empty());
	assert(!v.size());
	assert(!distance(vb, ve));
}

int
main(int, char*[]) {
	vec_type v;
	check_empty_list_vector(v);

	DUMP_IT(v);
	v.check_invariants();
	v.set_chunk_size(4);
	DUMP_IT(v);
	v.check_invariants();
	check_empty_list_vector(v);

#if 1
	v.push_back(0);
	DUMP_IT(v);
	v.check_invariants();
	v.push_back(1);
	DUMP_IT(v);
	v.check_invariants();
	v.pop_back();
	DUMP_IT(v);
	v.check_invariants();
	v.pop_back();
	DUMP_IT(v);
	v.check_invariants();
	check_empty_list_vector(v);


	v.push_back(0);
	DUMP_IT(v);
	v.check_invariants();
	v.push_back(1);
	DUMP_IT(v);
	v.check_invariants();
	v.clear();
	DUMP_IT(v);
	v.check_invariants();
	check_empty_list_vector(v);
#endif

	dump_list_vector(v, cerr);
	// for-loops
	size_t i = 0;
	for ( ; i < 16; i++) {
		complex_push_list_vector(v, i);
		assert(v.size() == i+1);
		assert(v.front() == 0);
		assert(v.back() == i);
		assert(*(--v.end()) == i);
		cerr << "original:" << endl;
		dump_list_vector(v, cerr);
		check_iterators(v.begin(), v.end());
		cerr << "default copy:" << endl;
		const vec_type dv(v);
		dump_list_vector(dv, cerr);
		check_iterators(dv.begin(), dv.end());
		cerr << "flattened copy:" << endl;
		const vec_type fv(v);
		dump_list_vector(fv, cerr);
		check_iterators(fv.begin(), fv.end());
	}
	dump_list_vector(v, cerr);

}


