/**
	\file "PR/pcanvas.cc"
 */

#include <iostream>
#include "PR/pcanvas.h"
#include "PR/tile_type.h"
#include "PR/pr_utils.h"
#include "util/macros.h"

namespace PR {
#include "util/using_ostream.h"

//=============================================================================
// class pcanvas method definitions

pcanvas::pcanvas(const size_t d) :
		dimensions(d), objects(), springs()
#if PR_MULTINETS
		, nets()
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pcanvas::~pcanvas() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
real_type
pcanvas::auto_proximity_radius(void) const {
	typedef	vector<tile_instance>::const_iterator		const_iterator;
	const_iterator i(objects.begin()), e(objects.end());
	INVARIANT(i!=e);
	real_type ret = i->properties.maximum_dimension();
	for (++i; i!=e; ++i) {
		const real_type d = i->properties.maximum_dimension();
		if (d > ret)
			ret = d;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pcanvas::save_checkpoint(ostream& o) const {
	write_value(o, dimensions);
	save_array(o, objects);
	save_array(o, springs);
#if PR_MULTINETS
#error	"FINISH ME!"
#endif
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pcanvas::load_checkpoint(istream& i) {
	size_t d;
	read_value(i, d);
	if (d != dimensions) {
		cerr << "Error: number of dimensions mismatch!" << endl;
		return true;
	}
	load_array(i, objects);
	load_array(i, springs);
#if PR_MULTINETS
#error	"FINISH ME!"
#endif
	return !i;
}

//=============================================================================
}	// end namespace PR

