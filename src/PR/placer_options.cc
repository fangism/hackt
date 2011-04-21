/**
	\file "PR/placer_options.cc"
	$Id: placer_options.cc,v 1.1.2.2 2011/04/21 01:32:13 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include "PR/placer_options.h"
#include "util/indent.h"
#include "util/string.h"
#include "util/optparse.tcc"
#include "util/IO_utils.tcc"
#include "util/vector_ops.h"
#include "util/stacktrace.h"

namespace PR {
using util::auto_indent;
using util::strings::string_to_num;
using util::write_value;
using util::read_value;
#include "util/using_ostream.h"

//=============================================================================
// class placer_options method definitions

static
const real_type __default_lower_corner[] = {0.0, 0.0, -50.0};
static
const real_type __default_upper_corner[] = {100.0, 100.0, 50.0};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
placer_options::placer_options() :
		temperature(0.0),	// brrrr-r-r-r!!!!
		viscous_damping(0.1),	// gooiness (> 0)
		proximity_radius(0.0),	// for collision scanning
		repulsion_coeff(1.0),
		lower_corner(__default_lower_corner),
		upper_corner(__default_upper_corner),
		time_step(1e-3),
		pos_tol(1e-3),
		vel_tol(1e-3),
//		accel_tol(1e-3),
		precision(4),
		watch_objects(false),
		watch_deltas(false),
		watch_energy(false)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placer_options::save_checkpoint(ostream& o) const {
// write global parameters
	write_value(o, temperature);
	write_value(o, viscous_damping);
	write_value(o, proximity_radius);
	write_value(o, repulsion_coeff);
	write_value(o, lower_corner);
	write_value(o, upper_corner);
	write_value(o, time_step);
	write_value(o, pos_tol);
	write_value(o, vel_tol);
//	write_value(o, accel_tol);
	write_value(o, precision);
	write_value(o, watch_objects);
	write_value(o, watch_deltas);
	write_value(o, watch_energy);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placer_options::load_checkpoint(istream& i) {
// read global parameters
	read_value(i, temperature);
	read_value(i, viscous_damping);
	read_value(i, proximity_radius);
	read_value(i, repulsion_coeff);
	read_value(i, lower_corner);
	read_value(i, upper_corner);
	read_value(i, time_step);
	read_value(i, pos_tol);
	read_value(i, vel_tol);
//	read_value(i, accel_tol);
	read_value(i, precision);
	read_value(i, watch_objects);
	read_value(i, watch_deltas);
	read_value(i, watch_energy);
}

//------------------------------------------------------------------------------
// options map setup
typedef	placer_options				options_struct_type;
typedef	util::options_map_impl<options_struct_type>	options_map_impl_type;
typedef	options_map_impl_type::opt_func			opt_func;
typedef	options_map_impl_type::opt_entry		opt_entry;
typedef	options_map_impl_type::opt_map_type		opt_map_type;
static	options_map_impl_type				options_map_wrapper;
static	opt_map_type&	PE_option_map(options_map_wrapper.options_map);

template <typename T>
static
bool
__set_member_default(const option_value& opt,
		options_struct_type& PE, T options_struct_type::*mem) {
	return util::set_option_member_single_numeric_value(opt, PE, mem);
}
static
const
string
__bool_type__("bool"),
__int_type__("int"),
__real_type__("real"),
__str_type__("string");

template <typename T>
static const string&
__string_type_of(T options_struct_type::*);

static const string&
__string_type_of(bool options_struct_type::*) { return __bool_type__; }
#if 0
static const string&
__string_type_of(size_t options_struct_type::*) { return __int_type__; }
#endif
static const string&
__string_type_of(int_type options_struct_type::*) { return __int_type__; }
static const string&
__string_type_of(real_type options_struct_type::*) { return __real_type__; }
#if 0
static const string&
__string_type_of(string options_struct_type::*) { return __str_type__; }
#endif

template <typename T>
static
ostream&
__print_member_default(ostream& o, const options_struct_type& n_opt,
		T options_struct_type::*mem) {
	return options_map_impl_type::print_member_default(o, n_opt, mem);
}


// macros for registering options

#define	DEFINE_SET_MEMBER(member)					\
static									\
bool									\
__set_ ## member (const option_value& v, options_struct_type& o) {	\
	return __set_member_default(v, o, &options_struct_type::member);	\
}

#define	DEFINE_CALL_MEMBER_FUNCTION(memfun)				\
static									\
bool									\
__set_ ## memfun (const option_value& v, options_struct_type& o) {	\
	o.memfun(v);							\
	return false;							\
}

#define	DEFINE_PRINT_MEMBER(member)					\
static									\
ostream&								\
__print_ ## member (ostream& o, const options_struct_type& n) {		\
	return __print_member_default(o, n, &options_struct_type::member); \
}

#if 0
#define	DEFINE_PRINT_MEMBER_SEQUENCE(member)				\
static									\
ostream&								\
__print_ ## member (ostream& o, const options_struct_type& n) {		\
	return __print_member_sequence(o, n, &options_struct_type::member); \
}

#define	DEFINE_PRINT_POLICY_MEMBER(member)				\
	DEFINE_PRINT_MEMBER(member ## _policy)

#define	DEFINE_PRINT_MISC_OPTION(key)					\
static									\
ostream&								\
__print_ ## key (ostream& o, const options_struct_type& n) {		\
	static const string k(STRINGIFY(key));				\
	return __print_misc_option(o, n, k);				\
}
#endif

#define	DEFINE_TYPE_MEMBER(member)					\
static									\
const string&								\
__type_ ## member (void) {						\
	return __string_type_of(&options_struct_type::member);		\
}

#if 0
#define	DEFINE_TYPE_POLICY_MEMBER(member)				\
	DEFINE_TYPE_MEMBER(member ## _policy)
#endif

#define	REGISTER_OPTION_DEFAULT(member, key, help)			\
static const opt_entry& __receipt ## member				\
__ATTRIBUTE_UNUSED_CTOR__((PE_option_map[key] =				\
	opt_entry(& __set_ ## member, &__print_ ## member, 		\
	&__type_ ## member(), help)));

#if 0
#define	REGISTER_OPTION_POLICY(member, key, help)			\
	REGISTER_OPTION_DEFAULT(member ## _policy, key, help)
#endif

#define	REGISTER_PSEUDO_OPTION(memfun, key, help)			\
static const opt_entry& __receipt ## memfun				\
__ATTRIBUTE_UNUSED_CTOR__((PE_option_map[key] =				\
	opt_entry(& __set_ ## memfun, NULL, NULL, help)));

#if 0
#define	REGISTER_MISC_OPTION(key, help)					\
static const opt_entry& __receipt ## key				\
__ATTRIBUTE_UNUSED_CTOR__((PE_option_map[STRINGIFY(key)] =		\
	opt_entry(&__set_misc_option, &__print_ ## key,	 		\
	&__str_type__, help)));
#endif

// define option functions
#define	DEFINE_OPTION_DEFAULT(member, key, help)			\
	DEFINE_SET_MEMBER(member)					\
	DEFINE_PRINT_MEMBER(member)					\
	DEFINE_TYPE_MEMBER(member)					\
	REGISTER_OPTION_DEFAULT(member, key, help)

#if 0
#define	DEFINE_OPTION_SEQUENCE(member, key, help)			\
	DEFINE_SET_MEMBER(member)					\
	DEFINE_PRINT_MEMBER_SEQUENCE(member)				\
	DEFINE_TYPE_MEMBER(member)					\
	REGISTER_OPTION_DEFAULT(member, key, help)
#endif

// for member function calls

// for member function calls
#define	DEFINE_OPTION_MEMFUN(memfun, key, help)				\
	DEFINE_CALL_MEMBER_FUNCTION(memfun)				\
	REGISTER_PSEUDO_OPTION(memfun, key, help)

// for policy members
#define	DEFINE_MISC_OPTION(key, help)					\
	DEFINE_PRINT_MISC_OPTION(key)					\
	REGISTER_MISC_OPTION(key, help)

#define	DEFINE_PRESET_OPTION(memfun, key, help)				\
	DEFINE_OPTION_MEMFUN(memfun, key, help)


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DEFINE_OPTION_DEFAULT(viscous_damping, "damping", 
	"viscous damping coefficient, linear with velocity")
DEFINE_OPTION_DEFAULT(temperature, "temperature", 
	"annealing temperature, for additive random velocity")
DEFINE_OPTION_DEFAULT(repulsion_coeff, "repulsion_coeff", 
	"repulsive spring coefficient for (near-)colliding objects")
DEFINE_OPTION_DEFAULT(precision, "precision", 
	"time interval over which to integrate per iteration")
DEFINE_OPTION_DEFAULT(time_step, "time_step", 
	"time interval over which to integrate per iteration")
DEFINE_OPTION_DEFAULT(pos_tol, "position_tolerance", 
	"position change tolerance for convergence")
DEFINE_OPTION_DEFAULT(vel_tol, "velocity_tolerance", 
	"velocity change tolerance for convergence")

DEFINE_OPTION_DEFAULT(watch_objects, "watch_objects", 
	"print coordinates of objects after each iteration")
DEFINE_OPTION_DEFAULT(watch_deltas, "watch_deltas", 
	"report changes in position and velocity after each iteration")
DEFINE_OPTION_DEFAULT(watch_energy, "watch_energy", 
	"report kinetic and potential energy after each iteration")

#if 0
DEFINE_OPTION_MEMFUN(parse_corners, "geometry", 
	"set bounds/corners of simulation space")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placer_options::parse_corners(const option_value& v) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placer_options::parse_parameter(const string& s) {
	return parse_parameter(optparse(s));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placer_options::parse_parameter(const option_value& o) {
if (o.key.length()) {
	typedef opt_map_type::const_iterator    map_iterator;
	const map_iterator me(PE_option_map.end());
	const map_iterator mf(PE_option_map.find(o.key));
	if (mf != me) {
		if ((*mf->second.func)(o, *this)) {
			return true;
		}
	} else {
		cerr << "Error: unknown option \'"
			<< o.key << "\'." << endl;
		return true;
	}
}
	return false;
}	// end placement_engine::parse_parameter

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: list keys from option_map
 */
ostream&
placer_options::list_parameters(ostream& o) {
	o << "parameters [default values]:" << endl;
	return options_map_wrapper.help(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placer_options::dump_parameters(ostream& o) const {
	o << auto_indent << "bounds=" <<
		lower_corner << ';' << upper_corner << endl;
	options_map_wrapper.dump(o, *this);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placer_options::clamp_position(real_vector& v) const {
	util::vector_ops::min_clamp_elements(v, lower_corner);
	util::vector_ops::max_clamp_elements(v, upper_corner);
}

//=============================================================================
}	// end namespace PR

