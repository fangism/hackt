/**
	\file "ring_node_test.cc"
	Test for ring_node structures.  
	$Id: ring_node_test.cc,v 1.2 2005/02/27 22:54:33 fang Exp $
 */

#include <iostream>
#include <string>
#include "ring_node.tcc"

#include "using_ostream.h"
using std::string;
using util::ring_node;
using util::ring_node_derived;

// typedef	ring_node<string>	ring_type;

template <class RingType>
ostream&
dump_ring_node(ostream& o, const RingType& r) {
	// could even use ostream_iterator
	typename RingType::const_iterator i = r.begin();
	const typename RingType::const_iterator e = r.end();
	for ( ; i!=e; i++)
		cout << *i << endl;
	return o;
}

template <template <class> class>
static
void
ring_type_test(void);

//=============================================================================
int
main(int argc, char* argv[]) {
	cout << "========= testing ring_node<string> =========" << endl;
	ring_type_test<ring_node>();
	cout << "===== testing ring_node_derived<string> =====" << endl;
	ring_type_test<ring_node_derived>();
	return 0;
}

//-----------------------------------------------------------------------------
template <template <class> class R>
static
void
ring_type_test(void) {
	typedef	R<string>	ring_type;
{
	ring_type first("Hello.");
	dump_ring_node(cout, first) << endl;
	assert(first.size() == 1);
}
{
	ring_type first("Hello.");
	ring_type second("Goodbye.");
	first.merge(second);
	assert(first.contains(second));
	assert(second.contains(first));
	assert(first.size() == 2);
	dump_ring_node(cout, first) << endl;
	dump_ring_node(cout, second) << endl;
}
{
	ring_type first("Hello.");
	ring_type second("Yoo-hoo.");
	{
		ring_type third("Goodbye.");
		first.merge(second);
		second.merge(third);
		assert(first.size() == 3);
		assert(first.contains(second));
		assert(first.contains(third));
		assert(second.contains(first));
		assert(second.contains(third));
		assert(third.contains(first));
		assert(third.contains(second));
		dump_ring_node(cout, first) << endl;
		dump_ring_node(cout, second) << endl;
		dump_ring_node(cout, third) << endl;
	}
	assert(first.size() == 2);
	dump_ring_node(cout, first) << endl;
	dump_ring_node(cout, second) << endl;
}
{
	ring_type first("Hello.");
	ring_type second("Yoo-hoo.");
	first.merge(second);
	assert(first.size() == 2);
	dump_ring_node(cout, first) << endl;
	dump_ring_node(cout, second) << endl;
{
	ring_type third("Durhhhh...");
	ring_type fourth("Goodbye.");
	third.merge(fourth);
	assert(third.size() == 2);
	dump_ring_node(cout, third) << endl;
	dump_ring_node(cout, fourth) << endl;
	second.merge(fourth);
	assert(first.contains(third));
	assert(first.contains(fourth));
	assert(second.contains(third));
	assert(second.contains(fourth));
	assert(third.contains(first));
	assert(third.contains(second));
	assert(fourth.contains(first));
	assert(fourth.contains(second));
	dump_ring_node(cout, first) << endl;
	dump_ring_node(cout, second) << endl;
	dump_ring_node(cout, third) << endl;
	dump_ring_node(cout, fourth) << endl;
	fourth.merge(first);	// already included, but safely handled
	dump_ring_node(cout, fourth) << endl;
}
	// after losing a few nodes, structure is still a ring!
	dump_ring_node(cout, first) << endl;
	dump_ring_node(cout, second) << endl;
}
}

