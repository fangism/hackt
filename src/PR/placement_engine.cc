/**
	\file "PR/placement_engine.cc"
	$Id: placement_engine.cc,v 1.1.2.2 2011/04/15 00:52:02 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <algorithm>
#include <functional>
#include "PR/placement_engine.h"
#include "PR/tile_type.h"
#include "common/TODO.h"
#include "util/vector_ops.h"
#include "util/array.tcc"
#include "util/indent.h"
#include "util/string.h"
#include "util/numeric/random.h"
#include "util/stacktrace.h"

namespace PR {
using std::for_each;
using util::auto_indent;
using util::strings::string_to_num;
using util::numeric::rand48;
using namespace util::vector_ops;		// for overloads
#include "util/using_ostream.h"

//=============================================================================
static
const real_type __default_lower_bound[] = {0.0, 0.0, -50.0};
static
const real_type __default_upper_bound[] = {100.0, 100.0, 50.0};

//=============================================================================
// class placement_engine method definitions

placement_engine::placement_engine(const size_t d) :
		object_types(),
		channel_types(),
		temperature(0.0),	// brrrr-r-r-r!!!!
		viscous_damping(0.1),	// gooey
		proximity_radius(0.0),	// keep-away!
		space(d),
		lower_bound(__default_lower_bound),
		upper_bound(__default_upper_bound),
		ifstreams(),
		time_step(1e-3),
		pos_tol(1e-3),
		vel_tol(1e-3),
		accel_tol(1e-3) {
	initialize_default_types();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
placement_engine::~placement_engine() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::initialize_default_types(void) {
	object_types.clear();
	object_types.reserve(64);
	object_types.push_back(tile_type());
	channel_types.clear();
	channel_types.reserve(64);
	channel_types.push_back(channel_type());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::auto_proximity_radius(void) {
	proximity_radius = space.auto_proximity_radius();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::add_object_type(const tile_type& t) {
	object_types.push_back(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::add_channel_type(const channel_type& t) {
	channel_types.push_back(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::add_object(const tile_instance& o) {
	space.objects.push_back(o);
}

#define	CHECK_OBJECT_INDEX(i)						\
	if (i >= space.objects.size()) {				\
		cerr << "Error: object index out-of-bounds." << endl;	\
		return true;						\
	}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Check that object indices are in-bounds.
	\return true on error
 */
bool
placement_engine::add_channel(const channel_instance& c) {
	CHECK_OBJECT_INDEX(c.source)
	CHECK_OBJECT_INDEX(c.destination)
	space.springs.push_back(c);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::pin_object(const size_t i) {
	CHECK_OBJECT_INDEX(i)
	space.objects[i].fix();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::unpin_object(const size_t i) {
	CHECK_OBJECT_INDEX(i)
	space.objects[i].unfix();
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Enforce bounds on position.
 */
void
placement_engine::clamp_position(real_vector& v) const {
	min_clamp_elements(v, lower_bound);
	max_clamp_elements(v, upper_bound);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::place_object(const size_t i, const real_vector& v) {
	CHECK_OBJECT_INDEX(i)
	real_vector p(v);
	clamp_position(p);
	space.objects[i].place(p);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::parse_parameter(const string& s) {
	return parse_parameter(optparse(s));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: list keys from option_map
 */
ostream&
placement_engine::list_parameters(ostream& o) {
	o << "parameters:\n"
"  damping\n"
"  temperature";
	return o << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_parameters(ostream& o) const {
	o << "parameters:\n";
	o << "  damping=" << viscous_damping << endl;
	o << "  temperature=" << temperature << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::parse_parameter(const option_value& o) {
	// viscous_damping
	// temperature
	if (o.key == "damping") {
		if (o.values.empty()) {
			cerr << "physics.property.parse: missing value"
				<< endl;
			// or print current value
			return true;
		}
		if (string_to_num(o.values.front(), viscous_damping)) {
			cerr << "physics.property.parse: bad value"
				<< endl;
			return true;
		}
		return false;
	}
	else if (o.key == "temperature") {
		if (o.values.empty()) {
			cerr << "physics.property.parse: missing value"
				<< endl;
			// or print current value
			return true;
		}
		if (string_to_num(o.values.front(), temperature)) {
			cerr << "physics.property.parse: bad value"
				<< endl;
			return true;
		}
		return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Place all free objects somewhere randomly within the bounding box.
 */
void
placement_engine::scatter(void) {
	const real_vector box_size(upper_bound -lower_bound);
	typedef	rand48<double>			random_generator;
	random_generator g;
	vector<tile_instance>::iterator
		i(space.objects.begin()), e(space.objects.end());
	for ( ; i!=e; ++i) {
	if (!i->is_fixed()) {
		real_vector r;
		// fixed for 3D
		r[0] = g();
		r[1] = g();
		r[2] = g();
		r *= box_size;
		r += lower_bound;
		i->place(r);
	}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should return some information about this step for 
	determining convergence.
	TODO: compute effective force vector, accumulate per object
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
#if PR_TILE_MASS
	const tile_type& sp(sobj.properties);
	const tile_type& dp(dobj.properties);
#endif
	if (!(sf && df)) {
		// at least one end not fixed
		// TODO: optimize away number of mathematical operations
		// TODO: use delta - sizeof(objects)
		//	or distance between ellipsoids
		const position_type delta(dobj.position -sobj.position);
		const force_type force_vec(delta * i->properties.spring_coeff);
		const real_type dist = norm(delta);
		// TODO: use rectilinear distance as an option
		const real_type dthresh = (sobj.properties.maximum_dimension()
			+dobj.properties.maximum_dimension()) / 2.0;
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
			dobj.acceleration -= force_vec *
#if PR_TILE_MASS
				(ffrac / dp.mass);
#else
				ffrac;
#endif
		} else if (df) {
			// only dest is fixed
			sobj.acceleration += force_vec *
#if PR_TILE_MASS
				(ffrac / sp.mass);
#else
				ffrac;
#endif
		} else {
			// neither fixed
#if PR_TILE_MASS
			const real_type massfrac = dp.mass / (dp.mass + sp.mass);
#else
			static const real_type massfrac = 0.5;
#endif
			sobj.acceleration +=
#if PR_TILE_MASS
				force_vec * (ffrac * massfrac / sp.mass);
#else
				force_vec * (ffrac * massfrac);
#endif
			dobj.acceleration +=
#if PR_TILE_MASS
				force_vec * (ffrac * (1.0 -massfrac) / dp.mass);
#else
				force_vec * (ffrac * massfrac);
#endif
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
static
ostream&
__dump_array(ostream& o, const T& a) {
	typedef	typename T::const_iterator	const_iterator;
	INDENT_SECTION(o);
	const_iterator i(a.begin()), e(a.end());
	size_t j;
	for (j=0; i!=e; ++i, ++j) {
		i->dump(o << auto_indent << '[' << j << "]: ") << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_object_types(ostream& o) const {
	o << "object types:" << endl;
	return __dump_array(o, object_types);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_channel_types(ostream& o) const {
	o << "channel types:" << endl;
	return __dump_array(o, channel_types);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_objects(ostream& o) const {
	o << "objects:" << endl;
	return __dump_array(o, space.objects);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_channels(ostream& o) const {
	o << "channels:" << endl;
	return __dump_array(o, space.springs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump(ostream& o) const {
	dump_parameters(o);
	dump_object_types(o);
	dump_channel_types(o);
	dump_objects(o);
	dump_channels(o);
	return o;
}

//=============================================================================
}	// end namespace PR

