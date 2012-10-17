/**
	\file "PR/gravity_well.cc"
	$Id: gravity_well.cc,v 1.2 2011/05/03 19:20:44 fang Exp $
 */

#include <iostream>
#include <limits>
#include <algorithm>
#include <iterator>
#include "PR/gravity_well.hh"
#include "util/IO_utils.tcc"

namespace HAC {
namespace PR {
using std::set;
using std::ostream_iterator;
using std::numeric_limits;
#include "util/using_ostream.hh"

//=============================================================================
// class gravity well method definitions

gravity_well::gravity_well() : nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
gravity_well::~gravity_well() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void
gravity_well::add_single_well(const real_type& r) {
	nodes.insert(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void
gravity_well::add_strided_wells(const real_type& min, 
		const real_type& del, const real_type& max) {
	real_type x = min;
	while (x <= max) {
		nodes.insert(x);
		x += del;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void
gravity_well::remove_well(const real_type& r) {
	nodes.erase(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#if 0
void
gravity_well::remove_wells(const real_type& min, const real_type& max) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
ostream&
gravity_well::dump(ostream& o) const {
	o << "{ ";
	copy(nodes.begin(), nodes.end(), ostream_iterator<real_type>(o, ", "));
	o << "}" << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
gravity_well::save_checkpoint(ostream& o) const {
	util::write_sequence(o, nodes);
	// regions should be reconstructed
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
gravity_well::load_checkpoint(istream& i) {
	util::read_sequence_set_insert(i, nodes);
}

//=============================================================================
}	// end namespace PR
}	// end namespace HAC

