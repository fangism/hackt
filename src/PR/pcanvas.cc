/**
	\file "PR/pcanvas.cc"
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <functional>
#include "PR/pcanvas.hh"
#include "PR/tile_type.hh"
#include "PR/placer_options.hh"
#include "PR/pr_utils.hh"
#include "util/vector_ops.hh"
#include "util/macros.h"
#include "util/stacktrace.hh"

namespace HAC {
namespace PR {
#include "util/using_ostream.hh"
using namespace util::vector_ops;
using std::mem_fun_ref;

//=============================================================================
// class pcanvas method definitions

pcanvas::pcanvas(const size_t d) :
#if PR_VARIABLE_DIMENSIONS
		dimensions(d),
#endif
		objects(), springs(),
		current(),
		object_kinetic_energy(0.0),
		spring_potential_energy(0.0) {
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
		const object_state& ss(current[si]);
		const object_state& ds(current[di]);
		// this is *pre-update* potential energy
		i->potential_energy =
			tile_instance::current_attraction_potential_energy(
				sobj, dobj, i->properties, ss, ds);
		spring_potential_energy += i->potential_energy;
	}	// end for each spring
	spring_potential_energy *= 0.5;
	return spring_potential_energy;
}	// end update_potential_energy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should return some information about this step for 
	determining convergence.
	This compute the potential energy *prior* to position updates, 
	thus is one step behind.
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
		const tile_instance& sobj(objects[si]);
		const tile_instance& dobj(objects[di]);
		object_state& ss(current[si]);
		object_state& ds(current[di]);
		// this is *pre-update* potential energy
		i->potential_energy =
			tile_instance::apply_attraction_forces(
				sobj, dobj, i->properties, ss, ds);
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
delta_type
pcanvas::update_objects(const placer_options& opt) {
	STACKTRACE_VERBOSE;
	delta_type delta(0.0, 0.0);
	object_kinetic_energy = 0.0;
	typedef	vector<object_state>::iterator		state_iterator;
	state_iterator j(current.begin());
	typedef vector<tile_instance>::iterator		object_iterator;
	object_iterator i(objects.begin()), e(objects.end());
	const real_type tt = opt.temperature *opt.time_step;
#if !PR_TILE_MASS
	const real_type sqrttt(sqrt(tt));
#endif
	for ( ; i!=e; ++i, ++j) {
	if (!i->is_fixed()) {
		// apply force and momentum, update kinetic energy
		const position_type prev_p(j->position);
		const velocity_type prev_v(j->velocity);
		j->update(opt.time_step, opt.viscous_damping);
		// enforce bounds
		opt.clamp_position(j->position);
		// record maximum change, not counting randomness
		// don't bother with euclidean distance
		// rectilinear or maximum_dimension shall suffice
		const real_type rdx(rectilinear_distance(prev_p, j->position));
		const real_type rdv(rectilinear_distance(prev_v, j->velocity));
		if (rdx > delta.first)
			delta.first = rdx;
		if (rdv > delta.second)
			delta.second = rdv;
		// We do NOT include thermal displacement in the 
		// kinetic energy calculation!
		if (opt.temperature > 0.0) {
			// alter position or velocity?
			j->position
				+= random_scaled_vector(sqrttt)
#if PR_TILE_MASS
					/ sqrt(i->properties.mass)
#endif
				;
		}
		object_kinetic_energy +=
			i->update_kinetic_energy_2(j->velocity);
	}
	}	// end for all objects
	// finally correct factor of 1.2
	object_kinetic_energy *= 0.5;
	return delta;
}	// end pcanvas::update_objects

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Zero-out kinetic energy and velocity.
 */
void
pcanvas::kill_momentum(void) {
	STACKTRACE_VERBOSE;
	for_each(objects.begin(), objects.end(),
		mem_fun_ref(&tile_instance::kill_momentum));
	for_each(current.begin(), current.end(),
		mem_fun_ref(&object_state::kill_momentum));
	object_kinetic_energy = 0.0;	// easy calculation!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Perturb positions of all objects in some random direction/distance, 
	bounded by distance r.
	This ignores mass entirely.  
 */
void
pcanvas::shake(const real_type& r) {
	vector<object_state>::iterator
		i(current.begin()), e(current.end());
	for ( ; i!=e; ++i) {
		i->position += random_scaled_vector(r);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: scale-factor
 */
ostream&
pcanvas::emit_dot(ostream& o, const placer_options& opt) const {
	vector<tile_instance>::const_iterator
		ti(objects.begin()), te(objects.end());
	vector<object_state>::const_iterator
		oi(current.begin()), oe(current.end());
	// nodes
	size_t j = 0;
	for ( ; ti!=te; ++ti, ++oi, ++j) {
		o << "N" << j << " [";
		// box sizes slightly less than 0.5 to see edges
		ti->properties.emit_dot(o, 0.45) << ", ";
		oi->emit_dot(o, opt);
		o << "];" << endl;
	}
	INVARIANT(oi == oe);
	// edges
	vector<channel_instance>::const_iterator
		ci(springs.begin()), ce(springs.end());
	for ( ; ci!=ce; ++ci) {
		// leave edges as automatic
		ci->emit_dot(o, opt) << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: scale-factor
 */
ostream&
pcanvas::emit_fig(ostream& o, const placer_options& opt) const {
	vector<tile_instance>::const_iterator
		ti(objects.begin()), te(objects.end());
	vector<object_state>::const_iterator
		oi(current.begin()), oe(current.end());
	// nodes
	size_t j = 0;
	for ( ; ti!=te; ++ti, ++oi, ++j) {
		o << "# N" << j << endl;	// comment
		oi->emit_fig(o, *ti, opt);
	}
	INVARIANT(oi == oe);
	// edges
	vector<channel_instance>::const_iterator
		ci(springs.begin()), ce(springs.end());
	for ( ; ci!=ce; ++ci) {
		// comment
		o << "# N" << ci->source << " -> N" << ci->destination << endl;
		ci->emit_fig(o, current[ci->source].position,
			current[ci->destination].position, opt);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pcanvas::save_checkpoint(ostream& o) const {
#if PR_VARIABLE_DIMENSIONS
	write_value(o, dimensions);
#endif
	save_array(o, objects);
	save_array(o, springs);
	save_array(o, current);
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
	load_array(i, current);
	return !i;
}

//=============================================================================
}	// end namespace PR
}	// end namespace HAC

