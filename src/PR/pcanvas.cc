/**
	\file "PR/pcanvas.cc"
 */

#include "PR/pcanvas.h"
#include "PR/tile_type.h"
#include "util/macros.h"

namespace PR {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pcanvas::pcanvas(const size_t d) :
		dimensions(d), objects(), springs(), nets() {
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
}	// end namespace PR

