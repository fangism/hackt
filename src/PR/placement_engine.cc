/**
	\file "PR/placement_engine.cc"
	$Id: placement_engine.cc,v 1.1.2.3 2011/04/16 01:51:53 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include "PR/placement_engine.h"
#include "PR/tile_type.h"
#include "PR/pr_utils.h"
#include "common/TODO.h"
#include "util/vector_ops.h"
#include "util/array.tcc"
#include "util/indent.h"
#include "util/string.h"
#include "util/numeric/random.h"
#include "util/IO_utils.tcc"
#include "util/stacktrace.h"

namespace PR {
using std::for_each;
using util::auto_indent;
using util::strings::string_to_num;
using util::numeric::rand48;
using util::write_value;
using util::read_value;
using namespace util::vector_ops;		// for many operator overloads
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
		lower_bound(__default_lower_bound),
		upper_bound(__default_upper_bound),
		time_step(1e-3),
		pos_tol(1e-3),
		vel_tol(1e-3),
		accel_tol(1e-3),
		space(d),
		ifstreams(),
		autosave_name() {
	initialize_default_types();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
placement_engine::~placement_engine() {
if (autosave_name.size()) {
	std::ofstream o(autosave_name.c_str());
	if (o) {
	try {
		save_checkpoint(o);
	} catch (...) {
		cerr << "Fatal error during checkpoint save." << endl;
	}
	} else {
		cerr << "Error opening \'" << autosave_name <<
			"\' for saving checkpoint." << endl;
	}
}
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
	o << "  bounds=" << lower_bound << ','  << upper_bound << endl;
	o << "  damping=" << viscous_damping << endl;
	o << "  temperature=" << temperature << endl;
	o << "  time_step=" << time_step << endl;
	o << "  position_tolerance=" << pos_tol << endl;
	o << "  velocity_tolerance=" << vel_tol << endl;
	o << "  acceleration_tolerance=" << vel_tol << endl;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// identifier string for checkpoints
static
const string magic_string("hackt-ipple-ckpt");

// bump this whenever file format is updated
static
const size_t	checkpoint_version = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Saves entire session to be restored later (checkpoint).
 */
bool
placement_engine::save_checkpoint(ostream& o) const {
	write_value(o, magic_string);
	write_value(o, checkpoint_version);
	util::numeric::write_seed48(o);
// write global parameters
	write_value(o, temperature);
	write_value(o, viscous_damping);
	write_value(o, proximity_radius);
	write_value(o, lower_bound);
	write_value(o, upper_bound);
	write_value(o, time_step);
	write_value(o, pos_tol);
	write_value(o, vel_tol);
	write_value(o, accel_tol);
// write object types
	save_array(o, object_types);
// write channel types
	save_array(o, channel_types);
// write objects and channels
	space.save_checkpoint(o);

	write_value(o, magic_string);
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::load_checkpoint(istream& i) {
	static const char bad_ckpt[] =
		"ERROR: not a valid ipple checkpoint file.";
try {
	string header_check;
	read_value(i, header_check);
	if (header_check != magic_string) {
		cerr << bad_ckpt << endl;
		return true;
	}
	size_t version_check;
	read_value(i, version_check);
	if (version_check != checkpoint_version) {
		cerr << "Expecting checkpoint compatibility version " <<
			checkpoint_version << ", but got " <<
			version_check << "." << endl;
		return true;
	}
} catch (...) {
	cerr << bad_ckpt << endl;
	return true;
}
	util::numeric::read_seed48(i);
// read global parameters
	read_value(i, temperature);
	read_value(i, viscous_damping);
	read_value(i, proximity_radius);
	read_value(i, lower_bound);
	read_value(i, upper_bound);
	read_value(i, time_step);
	read_value(i, pos_tol);
	read_value(i, vel_tol);
	read_value(i, accel_tol);
// read object types
	load_array(i, object_types);
// read channel types
	load_array(i, channel_types);
// read objects and channels
	space.load_checkpoint(i);

{
	string temp;
	read_value(i, temp);
	if (temp != magic_string) {
		cerr << "ERROR: detected checkpoint misalignment!" << endl;
		return true;
	}
}
	return !i;
}

// TODO: dump_checkpoint

//=============================================================================
}	// end namespace PR

