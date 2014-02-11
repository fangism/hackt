/**
	\file "util/test/digraph_SCC.cc"
 */

#include <vector>
#include <iostream>
#include <iterator>
#include <cassert>
#include <algorithm>
#include "util/graph/bare_digraph.hh"

using util::graph::SCC_type;
using util::graph::bare_digraph;
using std::pair;
using std::make_pair;
using std::vector;
using std::ostream;
using std::cout;
using std::endl;

typedef	vector<pair<size_t, size_t> >	edges_type;

void
compute_and_print_SCCs(const size_t N, const edges_type& E, ostream& o) {
	bare_digraph G;
	SCC_type sccs;
	G.resize(N);
{
	edges_type::const_iterator i(E.begin()), e(E.end());
	for ( ; i!=e; ++i) {
		assert(i->first < N);
		assert(i->second < N);
		G.__add_edge(i->first, i->second);
	}
}
	G.strongly_connected_components(sccs);
	util::graph::dump_SCCs(o, sccs);
	G.transitive_closure();	// modifies G
	o << "G+:" << endl;
	G.dump(o);
}

int
main(int, char*[]) {
{
	edges_type E;
	compute_and_print_SCCs(1, E, cout);
	cout << endl;
}{
	edges_type E;
	compute_and_print_SCCs(2, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(1, 0));
	compute_and_print_SCCs(2, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 1));
	E.push_back(make_pair(0, 2));
	E.push_back(make_pair(1, 2));
	compute_and_print_SCCs(3, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 1));
	E.push_back(make_pair(2, 0));
	E.push_back(make_pair(1, 2));
	compute_and_print_SCCs(3, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 1));
	E.push_back(make_pair(1, 0));
	E.push_back(make_pair(1, 2));
	compute_and_print_SCCs(3, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 2));
	E.push_back(make_pair(2, 0));
	E.push_back(make_pair(1, 2));
	compute_and_print_SCCs(3, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(2, 0));
	E.push_back(make_pair(0, 2));
	E.push_back(make_pair(1, 0));
	compute_and_print_SCCs(3, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(2, 0));
	E.push_back(make_pair(0, 2));
	E.push_back(make_pair(0, 1));
	compute_and_print_SCCs(3, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(1, 0));
	E.push_back(make_pair(1, 2));
	E.push_back(make_pair(2, 1));
	compute_and_print_SCCs(3, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 1));
	E.push_back(make_pair(1, 2));
	E.push_back(make_pair(2, 1));
	compute_and_print_SCCs(3, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 1));
	E.push_back(make_pair(0, 2));
	E.push_back(make_pair(1, 2));
	E.push_back(make_pair(1, 3));
	E.push_back(make_pair(2, 3));
	compute_and_print_SCCs(4, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 1));
	E.push_back(make_pair(0, 2));
	E.push_back(make_pair(1, 2));
	E.push_back(make_pair(1, 3));
	E.push_back(make_pair(2, 0));
	compute_and_print_SCCs(4, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 1));
	E.push_back(make_pair(0, 3));
	E.push_back(make_pair(1, 2));
	E.push_back(make_pair(2, 3));
	E.push_back(make_pair(3, 1));
	compute_and_print_SCCs(4, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 1));
	E.push_back(make_pair(0, 3));
	E.push_back(make_pair(1, 0));
	E.push_back(make_pair(2, 3));
	E.push_back(make_pair(3, 2));
	compute_and_print_SCCs(4, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 1));
	E.push_back(make_pair(0, 3));
	E.push_back(make_pair(1, 3));
	E.push_back(make_pair(2, 3));
	E.push_back(make_pair(3, 2));
	compute_and_print_SCCs(4, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 1));
	E.push_back(make_pair(1, 0));
	E.push_back(make_pair(1, 3));
	E.push_back(make_pair(2, 3));
	E.push_back(make_pair(3, 2));
	compute_and_print_SCCs(4, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 2));
	E.push_back(make_pair(2, 4));
	E.push_back(make_pair(4, 1));
	E.push_back(make_pair(1, 3));
	E.push_back(make_pair(3, 0));
	compute_and_print_SCCs(5, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(0, 2));
	E.push_back(make_pair(2, 0));
	E.push_back(make_pair(4, 1));
	E.push_back(make_pair(1, 3));
	E.push_back(make_pair(3, 0));
	compute_and_print_SCCs(5, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(2, 0));
	E.push_back(make_pair(3, 2));
	E.push_back(make_pair(4, 1));
	E.push_back(make_pair(1, 3));
	E.push_back(make_pair(3, 0));
	compute_and_print_SCCs(5, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(2, 0));
	E.push_back(make_pair(3, 2));
	E.push_back(make_pair(4, 1));
	E.push_back(make_pair(2, 4));
	E.push_back(make_pair(1, 3));
	E.push_back(make_pair(3, 0));
	compute_and_print_SCCs(5, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(2, 0));
	E.push_back(make_pair(3, 2));
	E.push_back(make_pair(2, 3));
	E.push_back(make_pair(1, 4));
	E.push_back(make_pair(4, 1));
	compute_and_print_SCCs(5, E, cout);
	cout << endl;
}{
	edges_type E;
	E.push_back(make_pair(2, 0));
	E.push_back(make_pair(3, 2));
	E.push_back(make_pair(2, 3));
	E.push_back(make_pair(3, 1));
	E.push_back(make_pair(1, 4));
	E.push_back(make_pair(4, 1));
	compute_and_print_SCCs(5, E, cout);
	cout << endl;
}
	return 0;
}
