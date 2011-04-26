/**
	\file "PR/tile_instance.cc"
	$Id: tile_instance.cc,v 1.1.2.13 2011/04/26 01:20:29 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "PR/tile_instance.h"
#include "util/array.tcc"
#include "util/vector_ops.h"
#include "util/optparse.h"
// #include "util/optparse.tcc"
#include "util/IO_utils.tcc"
#include "util/stacktrace.h"

namespace PR {
// using PR::optparse;
using namespace util::vector_ops;
using util::option_value;
using util::read_value;
using util::write_value;
#include "util/using_ostream.h"

//=============================================================================
// class tile_properties method definitions

// #if !VARIABLE_DIMENSIONS
static
const real_type __default_size[] = {2.0, 2.0, 2.0};

#if 0
static
const real_vector default_size(__default_size);
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
tile_properties::tile_properties() :
		size(__default_size)
#if PR_TILE_MASS
		, mass(1.0)
#endif
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
tile_properties::~tile_properties() { }


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
real_type
tile_properties::maximum_dimension(void) const {
	return util::vector_ops::max(size);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: use property function map,
		see hacknet::netlist_options implementation
	\param s is a string of the form "KEY=VALUE"
	\return true on error
 */
bool
tile_properties::parse_property(const string& s) {
	return parse_property(optparse(s));
}

bool
tile_properties::parse_property(const option_value& o) {
	STACKTRACE_VERBOSE;
	if (o.key == "size") {
		if (o.values.empty()) {
			cerr << "object_type.property.parse: missing value"
				<< endl;
			// or print current value
			return true;
		}
		if (parse_real_vector(o.values.front(), size)) {
			cerr << "object_type.property.parse: bad vector value"
				<< endl;
			return true;
		}
		return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
tile_properties::dump(ostream& o) const {
	o << "size=" << size;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
tile_properties::save_checkpoint(ostream& o) const {
	write_value(o, size);
#if PR_TILE_MASS
	write_value(o, mass);
#endif
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
tile_properties::load_checkpoint(istream& i) {
	read_value(i, size);
#if PR_TILE_MASS
	read_value(i, mass);
#endif
	return !i;
}

//=============================================================================
// class object_state method definitions

static
const real_type __zero[] = {0.0, 0.0, 0.0};

object_state::object_state() :
		position(__zero), 
		velocity(__zero), 
		acceleration(__zero) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Numerical integration assuming momentarily constant acceleration.
	Optimize by passing dt^2?
	\param dt is the time step
	\param v is the viscous damping coefficient
 */
void
object_state::update(const time_type& dt, const real_type& v) {
//	position += previous_velocity *dt + acceleration *(dt*dt*0.5);
	position += (velocity + acceleration *(dt*0.5)) *dt;
	velocity += (acceleration -velocity *v) *dt;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
object_state::update(const time_type& dt, const real_type& v,
		const object_state& previous) {
	velocity = previous.velocity
		+(acceleration -previous.velocity *v) *dt;
	position = previous.position
		+(previous.velocity + acceleration *(dt*0.5)) *dt;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
object_state::dump_position(ostream& o) const {
	return o << "@=" << position;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
object_state::dump(ostream& o) const {
	o << "@=" << position;
	o << " @'=" << velocity;
	o << " @\"=" << acceleration;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
object_state::save_checkpoint(ostream& o) const {
	write_value(o, position);
	write_value(o, velocity);
	write_value(o, acceleration);
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
object_state::load_checkpoint(istream& i) {
	read_value(i, position);
	read_value(i, velocity);
	read_value(i, acceleration);
	return !i;
}


//=============================================================================
// class tile_instance method definitions

bool tile_instance::dump_properties = true;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
tile_instance::tile_instance() :
		properties(),
		fixed(false)
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
tile_instance::tile_instance(const tile_type& t) :
		properties(t),
		fixed(false)
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
tile_instance::~tile_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Applies a force vector on two connected objects, 
	where force can be attractive or repulsive.
 */
void
tile_instance::apply_pairwise_force(
		const tile_instance& sobj, const tile_instance& dobj,
		const force_type& force_vec,
		object_state& ss, object_state& ds) {
	const bool sf = sobj.is_fixed();
	const bool df = dobj.is_fixed();
#if PR_TILE_MASS
	const tile_type& sp(sobj.properties);
	const tile_type& dp(dobj.properties);
#endif
//	static const real_type ffrac = 1.0;
	if (sf && !df) {
		// only source is fixed
		ds.acceleration -= force_vec
#if PR_TILE_MASS
			/ dp.mass
#endif
			;
	} else if (df && !sf) {
		// only dest is fixed
		ss.acceleration += force_vec
#if PR_TILE_MASS
			/ sp.mass
#endif
			;
	} else if (!sf && !df) {
		// neither fixed, account for mass ratio
#if PR_TILE_MASS
		const real_type massfrac = dp.mass / (dp.mass +sp.mass);
#else
		static const real_type massfrac = 0.5;
		const force_type half_force = force_vec * massfrac;
#endif
		ss.acceleration +=
#if PR_TILE_MASS
			force_vec * (massfrac / sp.mass);
#else
			half_force;
#endif
		ds.acceleration -=
#if PR_TILE_MASS
			force_vec * ((1.0 -massfrac) / dp.mass);
#else
			half_force;
#endif
	}
	// else both fixed, do nothing
}	// end apply_pairwise_force

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Computes potential_energy using the *current* position
	instead of the previous_position.
 */
real_type
tile_instance::current_attraction_potential_energy(
		const tile_instance& sobj, const tile_instance& dobj,
		const channel_properties& cp,
		const object_state& ss, const object_state& ds) {
	const bool sf = sobj.is_fixed();
	const bool df = dobj.is_fixed();
if (!(sf && df)) {
	const position_type delta(ds.position -ss.position);
	const real_type dist = norm(delta);	// distance between centers
	const real_type stretch = dist -cp.equilibrium_distance;
	// TODO: use rectilinear distance as an option?
	if (stretch > 0.0) {
		return stretch * stretch *cp.spring_coeff;
	}       // else objects too close to attract
}
	return 0.0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Computes potential_energy using the *current* position
	instead of the previous_position.
 */
real_type
tile_instance::current_repulsion_potential_energy(
		const tile_instance& sobj, const tile_instance& dobj,
		const channel_properties& cp,
		const object_state& ss, const object_state& ds) {
	const bool sf = sobj.is_fixed();
	const bool df = dobj.is_fixed();
if (!(sf && df)) {
	const position_type delta(ds.position -ss.position);
	const real_type dist = norm(delta);	// distance between centers
	const real_type stretch = dist -cp.equilibrium_distance;
	// TODO: use rectilinear distance as an option?
	if (stretch < 0.0) {
		return stretch * stretch *cp.spring_coeff;
	}       // else objects too far to repel
}
	return 0.0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pair-wise attractive-only force.
	Only need channel_instance to be modifiable for the sake of
	updating tension.  
	NOTE: this is the potential energy *before* position updates.
	\returns the potential energy in the spring (x2).
		If both ends are fixed, return 0 energy (don't count).
 */
real_type
tile_instance::apply_attraction_forces(
		const tile_instance& sobj, const tile_instance& dobj,
		const channel_properties& cp,
		object_state& ss, object_state& ds) {
	STACKTRACE_VERBOSE;
	const bool sf = sobj.is_fixed();
	const bool df = dobj.is_fixed();
if (!(sf && df)) {
	// at least one end not fixed
	// TODO: optimize away number of mathematical operations
	// TODO: use delta - sizeof(objects)
	//	or distance between ellipsoids
	const position_type delta(ds.position -ss.position);
	// optimization: compare square of distances to avoid sqrt() call?
	const real_type dist = norm(delta);	// distance between centers
	const real_type stretch = dist -cp.equilibrium_distance;
	// TODO: use rectilinear distance as an option
	if (stretch > 0.0) {
		const force_type force_vec(delta *
			(cp.spring_coeff *stretch / dist));
		apply_pairwise_force(sobj, dobj, force_vec, ss, ds);
		return stretch * stretch *cp.spring_coeff;
	}       // else objects too close to attract
	// let repulsion forces be computed in different phase
}       // else don't bother computing if both ends are fixed
	return 0.0;
}       // end placement_engine::apply_attraction_forces()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pair-wise repulsion-only force.
	Identical to above, but with flipped threshold.
	NOTE: this is the potential energy *before* position updates.
	\returns potential energy of repulsive spring (x2), 
		which is 0 if spring is not active.
 */
real_type
tile_instance::apply_repulsion_forces(
		const tile_instance& sobj, const tile_instance& dobj,
		const channel_properties& cp,
		object_state& ss, object_state& ds) {
	STACKTRACE_VERBOSE;
	const bool sf = sobj.is_fixed();
	const bool df = dobj.is_fixed();
if (!(sf && df)) {
	// at least one end not fixed
	// TODO: optimize away number of mathematical operations
	// TODO: use delta - sizeof(objects)
	//      or distance between ellipsoids
	const position_type delta(ds.position -ss.position);
	// optimization: compare square of distances to avoid sqrt() call?
	const real_type dist = norm(delta);	// distance between centers
	const real_type stretch = dist -cp.equilibrium_distance;
	// TODO: use rectilinear distance as an option
	if (stretch < 0.0) {
		const force_type force_vec(delta *
			(cp.spring_coeff *stretch / dist));
		apply_pairwise_force(sobj, dobj, force_vec, ss, ds);
		return stretch *stretch *cp.spring_coeff;
	}       // else objects too close to attract
	// let repulsion forces be computed in different phase
}       // else don't bother computing if both ends are fixed
	return 0.0;
}       // end placement_engine::apply_repulsion_forces()


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Updates kinetic energy to mv^2 (missing factor of 1/2).
	To be consistent with old-potential energy, use previous_velocity
 */
const real_type&
tile_instance::update_kinetic_energy_2(const velocity_type& v) {
#if PR_TILE_MASS
	_kinetic_energy_2 = properties.mass * normsq(v);
#else
	_kinetic_energy_2 = normsq(v);
#endif
	return _kinetic_energy_2;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: dump kinetic energy?
 */
ostream&
tile_instance::dump(ostream& o) const {
	if (fixed) {
		o << " (fixed)";
	}
	if (dump_properties) {
		properties.dump(o << " [") << ']';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
tile_instance::save_checkpoint(ostream& o) const {
	properties.save_checkpoint(o);
	write_value(o, fixed);
	write_value(o, _kinetic_energy_2);
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
tile_instance::load_checkpoint(istream& i) {
	properties.load_checkpoint(i);
	read_value(i, fixed);
	read_value(i, _kinetic_energy_2);
	return !i;
}

//=============================================================================
}	// end namespace PR

