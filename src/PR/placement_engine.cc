/**
	\file "PR/placement_engine.cc"
	$Id: placement_engine.cc,v 1.1.2.1 2011/04/11 18:38:38 fang Exp $
 */

#include <algorithm>
#include <functional>
#include "PR/placement_engine.h"
#include "PR/tile_type.h"
#include "util/vector_ops.h"
#include "util/array.tcc"

namespace PR {
using std::for_each;
using namespace util::vector_ops;		// for overloads

//=============================================================================
// class placement_engine method definitions

placement_engine::placement_engine(const size_t d) :
		temperature(0.0),
		friction(0.0),
		proximity_radius(0.0),
		space(d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
placement_engine::~placement_engine() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::auto_proximity_radius(void) {
	proximity_radius = space.auto_proximity_radius();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should return some information about this step for 
	determining convergence.
	TODO: support different spring types
 */
void
placement_engine::iterate(void) {
//	const time_type& dt(time_step);
	// first, zero out forces on objects
	for_each(space.objects.begin(), space.objects.end(), 
		std::mem_fun_ref(&tile_instance::zero_force));
{
	// compute spring tensions (attraction)
	typedef	vector<channel_instance>::iterator	iterator;
	iterator i(space.springs.begin()), e(space.springs.end());
for ( ; i!=e; ++i) {
	const int_type& si(i->source);
	const int_type& di(i->destination);
	tile_instance& sobj(space.objects[si]);
	tile_instance& dobj(space.objects[di]);
	const bool sf = sobj.is_fixed();
	const bool df = dobj.is_fixed();
	if (!(sf && df)) {
		// at least one end not fixed
		// TODO: optimize away number of mathematical operations
		// TODO: use delta - sizeof(objects)
		//	or distance between ellipsoids
		const position_type delta(dobj.position -sobj.position);
		const force_type force_vec(delta * i->spring_coeff);
		const real_type dist = norm(delta);
		const real_type dthresh = (sobj.type->properties.maximum_dimension()
			+dobj.type->properties.maximum_dimension()) / 2.0;
		if (dist > dthresh) {
		const real_type ffrac = (dist -dthresh) / dist;
#if 0
		position_type ndel(delta);
		ndel /= dist;
		const real_type tension = i->spring_coeff * dist;
#else
		const real_type tension = norm(force_vec) * ffrac;
#endif
		i->tension = tension;
		if (sf) {
			// only source is fixed
			dobj.acceleration -= force_vec * (ffrac / dobj.mass);
		} else if (df) {
			// only dest is fixed
			sobj.acceleration += force_vec * (ffrac / sobj.mass);
		} else {
			// neither fixed
			const real_type massfrac = dobj.mass / (dobj.mass + sobj.mass);
			sobj.acceleration +=
				force_vec * (ffrac * massfrac / sobj.mass);
			dobj.acceleration +=
				force_vec * (ffrac * (1.0 -massfrac) / dobj.mass);
		}
		}	// else objects too close to attract
	}
	// else don't bother computing if both ends are fixed
}	// end for each spring
}{
	// compute proximity repulsions, using proximity cache
}
	// update position and velocity
{
	// update proximity cache
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	\param dt time step size
	\param ptol position tolerance, below which is considered converged
	\param ftol force tolerance, below which is considered converged
	\return the number of iterations advanced until convergence.
 */
int_type
placement_engine::solve(void) {
}
#endif

//=============================================================================
}	// end namespace PR

