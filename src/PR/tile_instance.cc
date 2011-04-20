/**
	\file "PR/tile_instance.cc"
	$Id: tile_instance.cc,v 1.1.2.6 2011/04/20 01:09:42 fang Exp $
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
		position(__zero), previous_position(__zero), 
		velocity(__zero), previous_velocity(__zero),
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
	previous_position = position;
	previous_velocity = velocity;
	velocity += (acceleration -previous_velocity *v) *dt;
//	position += previous_velocity *dt + acceleration *(dt*dt*0.5);
	position += (previous_velocity + acceleration *(dt*0.5)) *dt;
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
	write_value(o, previous_position);
	write_value(o, velocity);
	write_value(o, previous_velocity);
	write_value(o, acceleration);
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
object_state::load_checkpoint(istream& i) {
	read_value(i, position);
	read_value(i, previous_position);
	read_value(i, velocity);
	read_value(i, previous_velocity);
	read_value(i, acceleration);
	return !i;
}


//=============================================================================
// class tile_instance method definitions

bool tile_instance::dump_properties = true;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
tile_instance::tile_instance() :
		object_state(), 
		properties(),
#if PR_LOCAL_PROXIMITY_CACHE
		proximity_cache(),
#endif
		fixed(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
tile_instance::tile_instance(const tile_type& t) :
		object_state(), 
		properties(t),
#if PR_LOCAL_PROXIMITY_CACHE
		proximity_cache(),
#endif
		fixed(false) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
tile_instance::~tile_instance() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
tile_instance::dump(ostream& o) const {
	if (fixed) {
		o << "@=" << position << " (fixed)";
	} else {
		object_state::dump(o);
	}
	if (dump_properties) {
		properties.dump(o << " [") << ']';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
tile_instance::save_checkpoint(ostream& o) const {
	object_state::save_checkpoint(o);
	properties.save_checkpoint(o);
	write_value(o, fixed);
	// proximity cache should be regenerated
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
tile_instance::load_checkpoint(istream& i) {
	object_state::load_checkpoint(i);
	properties.load_checkpoint(i);
	read_value(i, fixed);
	// proximity cache should be regenerated
	return !i;
}

//=============================================================================
}	// end namespace PR

