/**
	\file "ring_node_test.cc"
	Test for ring_node structures.  
	$Id: ring_node_test.cc,v 1.1.2.1 2005/02/04 05:40:25 fang Exp $
 */

#include <iostream>
#include <string>
#include "ring_node.tcc"

#include "using_ostream.h"
using std::string;
using util::ring_node;

typedef	ring_node<string>	ring_type;

ostream&
dump_ring_node(ostream& o, const ring_type& r) {
	// could even use ostream_iterator
	ring_type::const_iterator i = r.begin();
	const ring_type::const_iterator e = r.end();
	for ( ; i!=e; i++)
		cout << *i << endl;
	return o;
}

int
main(int argc, char* argv[]) {
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
	return 0;
}

