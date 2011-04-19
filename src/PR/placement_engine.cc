/**
	\file "PR/placement_engine.cc"
	$Id: placement_engine.cc,v 1.1.2.5 2011/04/19 03:51:48 fang Exp $
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
#include "util/optparse.tcc"
#include "util/numeric/random.h"
#include "util/IO_utils.tcc"
#include "util/iterator_more.h"
#include "util/iomanip.h"
#include "util/value_saver.h"
#include "util/stacktrace.h"

namespace PR {
using std::for_each;
using std::transform;
using std::lower_bound;
using std::upper_bound;
using std::sort;
using std::distance;
using util::auto_indent;
using util::save_precision;
using util::strings::string_to_num;
using util::numeric::rand48;
using util::write_value;
using util::read_value;
using util::value_saver;
using namespace util::vector_ops;		// for many operator overloads
#include "util/using_ostream.h"

//=============================================================================
static
const real_type __default_lower_corner[] = {0.0, 0.0, -50.0};
static
const real_type __default_upper_corner[] = {100.0, 100.0, 50.0};

//=============================================================================
// class placer_options method definitions

placer_options::placer_options() :
		temperature(0.0),	// brrrr-r-r-r!!!!
		viscous_damping(0.1),	// gooey
		proximity_radius(0.0),	// for collision scanning
		repulsion_coeff(1.0),
		lower_corner(__default_lower_corner),
		upper_corner(__default_upper_corner),
		time_step(1e-3),
		pos_tol(1e-3),
		vel_tol(1e-3),
//		accel_tol(1e-3),
		precision(4),
		watch_objects(false)
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
}

//=============================================================================
// class placement_engine method definitions

placement_engine::placement_engine(const size_t d) :
		state_base(), 
		object_types(),
		channel_types(),
		opt(),
		space(d),
#if !PR_LOCAL_PROXIMITY_CACHE
		proximity_cache(),
#endif
		elapsed_time(0.0), 
		autosave_name() {
	initialize_default_types();
#if !PR_LOCAL_PROXIMITY_CACHE
	proximity_cache.reserve(64);
#endif
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
	opt.proximity_radius = space.auto_proximity_radius();
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
	// automatically update proximity radius
	const real_type rad =
		space.objects.back().properties.maximum_dimension();
	if (rad > opt.proximity_radius)
		opt.proximity_radius = rad;
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
	const tile_instance& s(space.objects[c.source]);
	const tile_instance& d(space.objects[c.destination]);
	// uses spherical approximation of objects for now,
	// could be ellipsoid vector...
	space.springs.push_back(c);
	space.springs.back().properties.equilibrium_distance =
		(s.properties.maximum_dimension() +
			d.properties.maximum_dimension()) * 0.5;
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
	min_clamp_elements(v, opt.lower_corner);
	max_clamp_elements(v, opt.upper_corner);
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

#if 0
static const string&
__string_type_of(bool options_struct_type::*) { return __bool_type__; }
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

#if 0
DEFINE_OPTION_MEMFUN(parse_corners, "geometry", 
	"set bounds/corners of simulation space")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::parse_corners(const option_value& v) {
}
#endif

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
	o << "parameters [default values]:" << endl;
	return options_map_wrapper.help(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_parameters(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "parameters:\n";
	INDENT_SECTION(o);
	o << auto_indent << "bounds=" <<
		opt.lower_corner << ';' << opt.upper_corner << endl;
	o << auto_indent << "@time=" << elapsed_time << endl;
	options_map_wrapper.dump(o, opt);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
placement_engine::parse_parameter(const option_value& o) {
if (o.key.length()) {
	typedef opt_map_type::const_iterator    map_iterator;
	const map_iterator me(PE_option_map.end());
	const map_iterator mf(PE_option_map.find(o.key));
	if (mf != me) {
		if ((*mf->second.func)(o, opt)) {
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
	Place all free objects somewhere randomly within the bounding box.
 */
void
placement_engine::scatter(void) {
	STACKTRACE_VERBOSE;
	const real_vector box_size(opt.upper_corner -opt.lower_corner);
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
		r += opt.lower_corner;
		i->place(r);
	}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::zero_forces(void) {
	for_each(space.objects.begin(), space.objects.end(), 
		std::mem_fun_ref(&tile_instance::zero_force));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::apply_pairwise_force(
		tile_instance& sobj, tile_instance& dobj, 
		const force_type& force_vec) {
	const bool sf = sobj.is_fixed();
	const bool df = dobj.is_fixed();
#if PR_TILE_MASS
	const tile_type& sp(sobj.properties);
	const tile_type& dp(dobj.properties);
#endif
#if 0
	const real_type ffrac = (dist -dthresh) / dist;
#else
	static const real_type ffrac = 1.0;
#endif
#if 0
	const real_type tension = norm(force_vec) * ffrac;
	ch.tension = tension;
#endif
	if (sf && !df) {
		// only source is fixed
		dobj.acceleration -= force_vec *
#if PR_TILE_MASS
			(ffrac / dp.mass);
#else
			ffrac;
#endif
	} else if (df && !sf) {
		// only dest is fixed
		sobj.acceleration += force_vec *
#if PR_TILE_MASS
			(ffrac / sp.mass);
#else
			ffrac;
#endif
	} else if (!sf && !df) {
		// neither fixed, account for mass ratio
#if PR_TILE_MASS
		const real_type massfrac = dp.mass / (dp.mass +sp.mass);
#else
		static const real_type massfrac = 0.5;
		const real_type hf = ffrac * massfrac;
#endif
		sobj.acceleration +=
#if PR_TILE_MASS
			force_vec * (ffrac * massfrac / sp.mass);
#else
			force_vec * hf;
#endif
		dobj.acceleration -=
#if PR_TILE_MASS
			force_vec * (ffrac * (1.0 -massfrac) / dp.mass);
#else
			force_vec * hf;
#endif
	}
	// else both fixed, do nothing
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pair-wise attractive-only force.
	Only need channel_instance to be modifiable for the sake of
	updating tension.  
 */
void
placement_engine::apply_attraction_forces(
		tile_instance& sobj, tile_instance& dobj,
#if PR_CHANNEL_TENSION
		const channel_properties& cp
#else
		channel_instance& ch
#endif
		) {
	STACKTRACE_VERBOSE;
	const bool sf = sobj.is_fixed();
	const bool df = dobj.is_fixed();
if (!(sf && df)) {
	// at least one end not fixed
	// TODO: optimize away number of mathematical operations
	// TODO: use delta - sizeof(objects)
	//	or distance between ellipsoids
	const position_type delta(dobj.position -sobj.position);
#if PR_CHANNEL_TENSION
	const force_type force_vec(delta * cp.spring_coeff);
	const real_type& dthresh(cp.equilibrium_distance);
#else
	const force_type force_vec(delta * ch.properties.spring_coeff);
	const real_type& dthresh(ch.properties.equilibrium_distance);
#endif
	const real_type dist = norm(delta);
	// TODO: use rectilinear distance as an option
	if (dist > dthresh) {
		apply_pairwise_force(sobj, dobj, force_vec);
	}	// else objects too close to attract
	// let repulsion forces be computed in different phase
}	// else don't bother computing if both ends are fixed
}	// end placement_engine::apply_object_forces()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pair-wise repulsion-only force.
	Identical to above, but with flipped threshold.
 */
void
placement_engine::apply_repulsion_forces(
		tile_instance& sobj, tile_instance& dobj,
		const channel_properties& cp
		) {
	STACKTRACE_VERBOSE;
	const bool sf = sobj.is_fixed();
	const bool df = dobj.is_fixed();
if (!(sf && df)) {
	// at least one end not fixed
	// TODO: optimize away number of mathematical operations
	// TODO: use delta - sizeof(objects)
	//	or distance between ellipsoids
	const position_type delta(dobj.position -sobj.position);
	const force_type force_vec(delta * cp.spring_coeff);
	const real_type& dthresh(cp.equilibrium_distance);
	const real_type dist = norm(delta);
	// TODO: use rectilinear distance as an option
	if (dist < dthresh) {
		apply_pairwise_force(sobj, dobj, force_vec);
	}	// else objects too close to attract
	// let repulsion forces be computed in different phase
}	// else don't bother computing if both ends are fixed
}	// end placement_engine::apply_object_forces()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should return some information about this step for 
	determining convergence.
	TODO: compute effective force vector, accumulate per object
	TODO: support different spring types
 */
void
placement_engine::compute_spring_forces(void) {
	STACKTRACE_VERBOSE;
	// compute spring tensions (attraction)
	typedef	vector<channel_instance>::iterator	iterator;
	iterator i(space.springs.begin()), e(space.springs.end());
	for ( ; i!=e; ++i) {
		const int_type& si(i->source);
		const int_type& di(i->destination);
		tile_instance& sobj(space.objects[si]);
		tile_instance& dobj(space.objects[di]);
		apply_attraction_forces(sobj, dobj, *i);
	}	// end for each spring
}	// end compute_spring_forces

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D>
static
bool
dim_less(const vector<tile_instance>::const_iterator l,
		const vector<tile_instance>::const_iterator r) {
	return l->position[D] < r->position[D];
}

template <size_t D>
static
bool
dim_comp(const vector<tile_instance>::const_iterator l, const real_type& r) {
	return l->position[D] < r;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This could go to util lib.
	\param T is an iterator type, preferably random_access.
 */
template <class T>
struct array_offset {
	typedef	T		iterator_type;
	const iterator_type		begin;

	explicit
	array_offset(const T i) : begin(i) { }

	size_t
	operator () (const T i) const {
		return distance(begin, i);
	}

};	// end struct array_offset

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really want to avoid this.
 */
void
placement_engine::clear_proximity_cache(void) {
#if PR_LOCAL_PROXIMITY_CACHE
	for_each(space.objects.begin(), space.objects.end(),
		std::mem_fun_ref(&tile_instance::clear_proximity_cache));
#else
	proximity_cache.clear();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Update dynamic graph of near-neighbors.  
	Use a sliding-window cube in each dimension.
	TODO: maintain order as object positions are updated
		and only *incrementally* sort.
	Or do sector binning, near constant time!
 */
void
placement_engine::refresh_proximity_cache(void) {
	STACKTRACE_VERBOSE;
	clear_proximity_cache();		// wipe before recomputing
	// sort by each dimension/index
	typedef	vector<tile_instance>::iterator		iterator;
	vector<iterator> obj_x;
	obj_x.reserve(space.objects.size());
	iterator i(space.objects.begin()), e(space.objects.end());
	for ( ; i!=e; ++i) {
		obj_x.push_back(i);
	}
	// perform in-place sort on each
	// first, sorted by x-coordinates only
	sort(obj_x.begin(), obj_x.end(), &dim_less<0>);
	// use proximity_radius to find sets of objects in the same window
	vector<iterator>::iterator xb(obj_x.begin()), xe(obj_x.end());
	// Q: is binary search worth it? linear-incremental may suffice
	// A: linear! b/c overall cost O(N) vs. O(N lg N)
	vector<iterator>::iterator xu(xb);
	const array_offset<iterator> vo(space.objects.begin());
	const size_t i1 = vo(*xb);
#if 0
	std::set<size_t> window_set_x;	// a queue is good enough?
	transform(xb, xu, util::set_inserter(window_set_x), vo);
 	INVARIANT(window_set_x.size() >= 1);
#endif
	// x-sweep
for ( ; xb!=xe; ++xb) {
	const real_type x = (*xb)->position[0];
	// [xb, xu] defines a sliding window along the x-dimension
	// linear scan overall costs less than repeated (lg N) binary searches
	while (xu!=xe && (*xu)->position[0] < x+opt.proximity_radius) {
		++xu;
	}
const size_t xw_size = distance(xb, xu);
if (xw_size > 1) {
	STACKTRACE_INDENT_PRINT("have " << xw_size <<
		" x-collision candidates." << endl);
	// recurse in y-dimension, z-dimensions
	// reduce size of window_set in each dimension
	// copy range of iterators, and re-sort by y-dimension
	vector<iterator> obj_y(xb, xu);
	sort(obj_y.begin(), obj_y.end(), &dim_less<1>);
	const real_type& y_ref((*xb)->position[1]);
	const vector<iterator>::iterator
		yb(obj_y.begin()), ye(obj_y.end());
	const vector<iterator>::iterator
		yl(lower_bound(yb, ye, y_ref-opt.proximity_radius, &dim_comp<1>)),
		yu(lower_bound(yl, ye, y_ref+opt.proximity_radius, &dim_comp<1>));
	const size_t yw_size = distance(yl, yu);
	if (yw_size > 1) {
		STACKTRACE_INDENT_PRINT("have " << yw_size <<
			" xy-collision candidates." << endl);
		// copy range of iterators, re-sort by z-dimension
		vector<iterator> obj_z(yl, yu);
		sort(obj_z.begin(), obj_z.end(), &dim_less<2>);
		const real_type& z_ref((*xb)->position[2]);
		const vector<iterator>::iterator
			zb(obj_y.begin()), ze(obj_y.end());
		const vector<iterator>::iterator
			zl(lower_bound(zb, ze, z_ref-opt.proximity_radius,
				&dim_comp<2>)),
			zu(lower_bound(zl, ze, z_ref+opt.proximity_radius,
				&dim_comp<2>));
		const size_t zw_size = distance(zl, zu);
		if (zw_size > 1) {
			STACKTRACE_INDENT_PRINT("have " << zw_size <<
				" xyz-collision candidates." << endl);
			// then we are within proximity
			vector<iterator>::iterator zi(zl);
			for ( ; zi!=zu; ++zi) {
			if (*zi != *xb) {
				const size_t i2 = vo(*zi);
				STACKTRACE_INDENT_PRINT(
					"caching collision candidate pair ("
					<< i1 << ',' << i2 << ")." << endl);
				INVARIANT(i1 != i2);
				// avoid double counting with index ordering
#if PR_LOCAL_PROXIMITY_CACHE
				if (i1 < i2) {
				space.objects[i1].proximity_cache.insert(i2);
				} else {
				space.objects[i2].proximity_cache.insert(i1);
				}
#else
				if (i1 < i2) {
					proximity_cache.push_back(
						proximity_edge(i1, i2));
				} else {
					proximity_cache.push_back(
						proximity_edge(i2, i1));
				}
#endif
				}
			}
		}
	}
}	// else no collision candidates
}	// end for each x-ordered object
}	// end placement_engine::refresh_proximity_cache

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Compute repulsive forces between near-neighbors.  
 */
void
placement_engine::compute_collision_forces(void) {
	STACKTRACE_VERBOSE;
	typedef	vector<tile_instance>::iterator		iterator;
	iterator i(space.objects.begin()), e(space.objects.end());
	const array_offset<iterator> vo(i);
#if PR_LOCAL_PROXIMITY_CACHE
	for ( ; i!=e; ++i) {
		const size_t j1 = vo(i);
		set<int_type>::const_iterator
			ci(i->proximity_cache.begin()),
			ce(i->proximity_cache.end());
		tile_instance& o1(space.objects[j1]);
		for ( ; ci!=ce; ++ci) {
			const size_t j2 = *ci;
#else
	vector<proximity_edge>::const_iterator
		pi(proximity_cache.begin()), pe(proximity_cache.end());
	for ( ; pi!=pe; ++pi) {
		const size_t& j1(pi->first);
		const size_t& j2(pi->second);
			tile_instance& o1(space.objects[j1]);
#endif
			// avoid double counting
			INVARIANT(j1 < j2);
			STACKTRACE_INDENT_PRINT("repelling objects " <<
				j1 << " and " << j2 << endl);
			tile_instance& o2(space.objects[j2]);
			channel_properties dummy;
			// TODO: configure later
			dummy.spring_coeff = opt.repulsion_coeff;
			dummy.equilibrium_distance =
				(o2.properties.maximum_dimension()
				+o1.properties.maximum_dimension()) *0.5;
			apply_repulsion_forces(o1, o2, dummy);
#if PR_LOCAL_PROXIMITY_CACHE
		}	// end for each outgoing edge in local cache
	}	// end for each object/instance
#else
	}	// end for each proximity_edge
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: return maximum delta for chekcing for convergence.
 */
void
placement_engine::update_velocity_and_position(void) {
	STACKTRACE_VERBOSE;
	typedef	vector<tile_instance>::iterator		iterator;
	iterator i(space.objects.begin()), e(space.objects.end());
	for ( ; i!=e; ++i) {
	if (!i->is_fixed()) {
		i->update(opt.time_step);
	}
	}
	elapsed_time += opt.time_step;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
placement_engine::iterate(void) {
	STACKTRACE_VERBOSE;
	zero_forces();		// reset forces
	compute_spring_forces();
	// compute proximity repulsions on 'close' objects
	refresh_proximity_cache();
	compute_collision_forces();
	update_velocity_and_position();
	// TODO: enforce bounds on object positions: clamp_position
	if (opt.watch_objects) {
		const value_saver<bool>
			_x_(tile_instance::dump_properties, false);
		cout << "@time=" << elapsed_time << endl;
		dump_objects(cout);
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
	const save_precision p(o, opt.precision);
	o << "object types:" << endl;
	return __dump_array(o, object_types);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_channel_types(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "channel types:" << endl;
	return __dump_array(o, channel_types);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_objects(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "objects:" << endl;
	return __dump_array(o, space.objects);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump_channels(ostream& o) const {
	const save_precision p(o, opt.precision);
	o << "channels:" << endl;
	return __dump_array(o, space.springs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
placement_engine::dump(ostream& o) const {
	const save_precision p(o, opt.precision);
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
	opt.save_checkpoint(o);
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
	opt.load_checkpoint(i);
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

