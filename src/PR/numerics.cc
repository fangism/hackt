/**
	\file "PR/numerics.cc"
	$Id: numerics.cc,v 1.1.2.1 2011/04/11 18:38:35 fang Exp $
 */

#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>
#include "PR/numerics.h"

namespace PR {
using std::transform;
using std::accumulate;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
real_type
normsq(const position_type& p) {
	position_type sq(p);
#if VARIABLE_DIMENSIONS
	sq *= p;
	return sq.sum();
#else
	transform(p.begin(), p.end(), p.begin(), sq.begin(), 
		std::multiplies<real_type>());
	return accumulate(sq.begin(), sq.end(), 0.0);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
real_type
norm(const position_type& p) {
	return sqrt(normsq(p));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace PR

