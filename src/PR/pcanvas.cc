/**
	\file "PR/pcanvas.cc"
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <functional>
#include "PR/pcanvas.h"
#include "PR/tile_type.h"
#include "PR/placer_options.h"
#include "PR/pr_utils.h"
#include "util/vector_ops.h"
#include "util/macros.h"
#include "util/stacktrace.h"

namespace PR {
#include "util/using_ostream.h"
using namespace util::vector_ops;
using std::mem_fun_ref;

//=============================================================================
// class pcanvas method definitions

pcanvas::pcanvas(const size_t d) :
#if PR_VARIABLE_DIMENSIONS
		dimensions(d),
#endif
		objects(), springs()
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
/**
	Updates potential energy without applying forces to nodes.  
 */
const real_type&
pcanvas::update_potential_energy(void) {
	STACKTRACE_VERBOSE;
	spring_potential_energy = 0.0;
	// compute spring tensions (attraction)
	typedef	vector<channel_instance>::iterator	iterator;
	iterator i(springs.begin()), e(springs.end());
	for ( ; i!=e; ++i) {
		const int_type& si(i->source);
		const int_type& di(i->destination);
		const tile_instance& sobj(objects[si]);
		const tile_instance& dobj(objects[di]);
		// this is *pre-update* potential energy
		i->potential_energy =
			tile_instance::current_attraction_potential_energy(
				sobj, dobj, i->properties);
		spring_potential_energy += i->potential_energy;
	}	// end for each spring
	spring_potential_energy *= 0.5;
	return spring_potential_energy;
}	// end update_potential_energy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should return some information about this step for 
	determining convergence.
	TODO: compute effective force vector, accumulate per object
	TODO: support different spring types
 */
void
pcanvas::compute_spring_forces(void) {
	STACKTRACE_VERBOSE;
	spring_potential_energy = 0.0;
	// compute spring tensions (attraction)
	typedef	vector<channel_instance>::iterator	iterator;
	iterator i(springs.begin()), e(springs.end());
	for ( ; i!=e; ++i) {
		const int_type& si(i->source);
		const int_type& di(i->destination);
		tile_instance& sobj(objects[si]);
		tile_instance& dobj(objects[di]);
		// this is *pre-update* potential energy
		i->potential_energy =
			tile_instance::apply_attraction_forces(
				sobj, dobj, i->properties);
		spring_potential_energy += i->potential_energy;
	}	// end for each spring
	spring_potential_energy *= 0.5;
}	// end compute_spring_forces

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Change in position/velocity are recorded and updated. 
        From kinetic theory of molecules (gas):
                mv^2/2 = 3kT/2
 */
void
pcanvas::update_objects(const placer_options& opt) {
	STACKTRACE_VERBOSE;
#if 0
	_max_delta_position = 0.0;
	_max_delta_velocity = 0.0;
#endif
	object_kinetic_energy = 0.0;
	typedef vector<tile_instance>::iterator		iterator;
	iterator i(objects.begin()), e(objects.end());
	const real_type tt = opt.temperature *opt.time_step;
#if !PR_TILE_MASS
	const real_type sqrttt(sqrt(tt));
#endif
	for ( ; i!=e; ++i) {
	if (!i->is_fixed()) {
		// apply force and momentum, update kinetic energy
		i->update(opt.time_step, opt.viscous_damping);
		// enforce bounds
		opt.clamp_position(i->position);
		// record maximum change, not counting randomness
		// don't bother with euclidean distance
		// rectilinear or maximum_dimension shall suffice
#if 0
		const real_type rdx(i->rectilinear_delta_position());
		const real_type rdv(i->rectilinear_delta_velocity());
		if (rdx > _max_delta_position)
			_max_delta_position = rdx;
		if (rdv > _max_delta_velocity)
			_max_delta_velocity = rdv;
#endif
		// We do NOT include thermal displacement in the 
		// kinetic energy calculation!
		if (opt.temperature > 0.0) {
			// alter position or velocity?
			i->position += random_unit_vector() *
#if PR_TILE_MASS
				sqrt(tt / i->properties.mass);
#else
				sqrttt;
#endif
		}
		object_kinetic_energy += i->update_kinetic_energy_2();
	}
	}	// end for all objects
	// finally correct factor of 1.2
	object_kinetic_energy *= 0.5;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pcanvas::kill_momentum(void) {
	for_each(objects.begin(), objects.end(),
		mem_fun_ref(&tile_instance::kill_momentum));
	object_kinetic_energy = 0.0;	// easy calculation!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pcanvas::save_checkpoint(ostream& o) const {
#if PR_VARIABLE_DIMENSIONS
	write_value(o, dimensions);
#endif
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
#if PR_VARIABLE_DIMENSIONS
	size_t d;
	read_value(i, d);
	if (d != dimensions) {
		cerr << "Error: number of dimensions mismatch!" << endl;
		return true;
	}
#endif
	load_array(i, objects);
	load_array(i, springs);
#if PR_MULTINETS
#error	"FINISH ME!"
#endif
	return !i;
}

//=============================================================================
}	// end namespace PR

